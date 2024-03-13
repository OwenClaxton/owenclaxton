package net.deddybones.techplusplus;

import com.mojang.logging.LogUtils;
import net.deddybones.techplusplus.block.ModBlocks;
import net.deddybones.techplusplus.block.TweakedVanillaBlocks;
import net.deddybones.techplusplus.entity.ModEntities;
import net.deddybones.techplusplus.entity.client.ThrownWoodenSpearRenderer;
import net.deddybones.techplusplus.item.ModCreativeModeTabs;
import net.deddybones.techplusplus.item.ModItems;
import net.deddybones.techplusplus.item.TweakedVanillaItems;
import net.deddybones.techplusplus.loot.ModLootModifiers;
import net.deddybones.techplusplus.recipes.ModRecipes;
import net.deddybones.techplusplus.util.ModTags;
import net.deddybones.techplusplus.worldgen.ModWorldGen;
import net.minecraft.client.renderer.entity.EntityRenderers;
import net.minecraft.client.renderer.item.ItemProperties;
import net.minecraft.core.NonNullList;
import net.minecraft.resources.ResourceLocation;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.*;
import net.minecraft.world.item.CreativeModeTabs;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.crafting.*;
import net.minecraft.world.level.Level;
import net.minecraftforge.api.distmarker.Dist;
import net.minecraftforge.common.MinecraftForge;
import net.minecraftforge.event.BuildCreativeModeTabContentsEvent;
import net.minecraftforge.event.entity.player.EntityItemPickupEvent;
import net.minecraftforge.event.entity.player.PlayerEvent.ItemCraftedEvent;
import net.minecraftforge.event.server.ServerStartingEvent;
import net.minecraftforge.eventbus.api.IEventBus;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import net.minecraftforge.fml.common.Mod;
import net.minecraftforge.fml.event.lifecycle.FMLClientSetupEvent;
import net.minecraftforge.fml.event.lifecycle.FMLCommonSetupEvent;
import net.minecraftforge.fml.javafmlmod.FMLJavaModLoadingContext;
import org.slf4j.Logger;

import java.util.List;
import java.util.stream.Collectors;

// The value here should match an entry in the META-INF/mods.toml file
@Mod(TechPlusPlus.MOD_ID)
public class TechPlusPlus {
    public static final String MOD_ID = "deddymod";
    private static final Logger LOGGER = LogUtils.getLogger();

    public TechPlusPlus() {
        IEventBus modEventBus = FMLJavaModLoadingContext.get().getModEventBus();

        ModCreativeModeTabs.register(modEventBus);
        ModItems.register(modEventBus);
        TweakedVanillaItems.register(modEventBus);
        ModBlocks.register(modEventBus);
        TweakedVanillaBlocks.register(modEventBus);
        ModWorldGen.register(modEventBus);
        ModLootModifiers.register(modEventBus);
        ModEntities.register(modEventBus);
        ModRecipes.register(modEventBus);

        modEventBus.addListener(this::commonSetup);
        MinecraftForge.EVENT_BUS.register(this);
        modEventBus.addListener(this::addCreative);
    }

    private void commonSetup(final FMLCommonSetupEvent event) {
//        ItemBlockRenderTypes.setRenderLayer(ModBlocks.TEST_BLOCK.get(), RenderType.cutout());
        event.enqueueWork(() ->
        {
            ItemProperties.register(ModItems.WOODEN_SPEAR.get(),
                    new ResourceLocation(TechPlusPlus.MOD_ID, "throwing"), (stack, level, living, id) -> {
                        return living != null && living.isUsingItem() && living.getUseItem() == stack ? 1.0F : 0.0F;
                    });
        });
    }

    // Add the example block item to the building blocks tab
    private void addCreative(BuildCreativeModeTabContentsEvent event) {
        if(event.getTabKey() == CreativeModeTabs.INGREDIENTS) {
            event.accept(ModItems.SAPPHIRE);
        }
    }

    // You can use SubscribeEvent and let the Event Bus discover methods to call
    @SubscribeEvent
    public void onServerStarting(ServerStartingEvent event) {

    }

//    @SubscribeEvent
//    public void onOpenContainer(PlayerContainerEvent.Open event) {
//    }
//
//    @SubscribeEvent
//    public void onCloseContainer(PlayerContainerEvent.Close event) {
//    }
//
//    @SubscribeEvent
//    public void onLoggingIn(PlayerEvent.PlayerLoggedInEvent event) {
//    }
//
//    @SubscribeEvent
//    public void onLoggingOut(PlayerEvent.PlayerLoggedOutEvent event) {
//    }

    public boolean isRepairRecipe(Player player, Level level) {
        CraftingContainer craftingSlots;
        if (player.containerMenu.getClass() == InventoryMenu.class) {
            InventoryMenu iMenu = (InventoryMenu) player.containerMenu;
            craftingSlots = iMenu.getCraftSlots();
        } else {
            CraftingMenu cMenu = (CraftingMenu) player.containerMenu;
            NonNullList<ItemStack> craftingSlotContents = cMenu.slots.subList(1, 10).stream().map(Slot::getItem)
                    .collect(Collectors.toCollection(NonNullList::create));
            craftingSlots = new TransientCraftingContainer(cMenu, 3, 3, craftingSlotContents);
        }
        List<RecipeHolder<CraftingRecipe>> recipeList = level.getRecipeManager().getRecipesFor(RecipeType.CRAFTING, craftingSlots, level);
        return recipeList.get(0).id().toString().equals("minecraft:repair_item");
    }

    @SubscribeEvent
    public void itemCrafted(ItemCraftedEvent event) {
        Level level = event.getEntity().level();
        Player player = event.getEntity();
        if (!level.isClientSide()) {

            int startInd    = 1;
            int endInd      = (player.containerMenu.getClass() == InventoryMenu.class) ? 4 : 9;

            ItemStack craftingOutput = event.getCrafting();
            if (craftingOutput.is(ModTags.Items.CARVED_ITEM)) { // Check if we're performing a carving:
                for (int i = startInd; i <= endInd; i++) {
                    ItemStack thisItem = player.containerMenu.slots.get(i).getItem();
                    if (thisItem.is(ModTags.Items.CAN_CARVE)) {
                        thisItem.hurt(1, player.getRandom(), player instanceof ServerPlayer ? (ServerPlayer) player : null);
                        return;
                    }
                }
            } else if (craftingOutput.is(ModTags.Items.CAN_CARVE)) { // Check if we're crafting a carver:
                if (! isRepairRecipe(player, level)) return;
                // past this point for repair recipes; we need to manually purge the input tools
                // because CarverItem will remain by default
                int toolsFound = 0;
                for (int i = startInd; i <= endInd; i++) {
                    ItemStack thisItem = player.containerMenu.slots.get(i).getItem();
                    if (thisItem.is(craftingOutput.getItem())) {
                        player.containerMenu.slots.get(i).set(ItemStack.EMPTY);
                        toolsFound++;
                        if (toolsFound > 1) return;
                    }
                }
            }
        }
    }

    @SubscribeEvent
    public void pickupItem(EntityItemPickupEvent event) {
        System.out.println("Item picked up! :" + event.getResult());
    }

    // You can use EventBusSubscriber to automatically register all static methods in the class annotated with @SubscribeEvent
    @Mod.EventBusSubscriber(modid = MOD_ID, bus = Mod.EventBusSubscriber.Bus.MOD, value = Dist.CLIENT)
    public static class ClientModEvents {
        @SubscribeEvent
        public static void onClientSetup(FMLClientSetupEvent event) {
            EntityRenderers.register(ModEntities.THROWN_WOODEN_SPEAR_ENTITY_TYPE.get(), ThrownWoodenSpearRenderer::new);
        }
    }
}
