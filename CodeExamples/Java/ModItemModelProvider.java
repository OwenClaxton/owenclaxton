package net.deddybones.techplusplus.datagen;

import net.deddybones.techplusplus.TechPlusPlus;
import net.deddybones.techplusplus.block.ModBlocks;
import net.deddybones.techplusplus.item.ModItems;
import net.minecraft.data.PackOutput;
import net.minecraft.resources.ResourceKey;
import net.minecraft.resources.ResourceLocation;
import net.minecraft.server.packs.PackType;
import net.minecraft.world.item.ArmorItem;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.armortrim.TrimMaterial;
import net.minecraft.world.item.armortrim.TrimMaterials;
import net.minecraft.world.level.block.Block;
import net.minecraft.world.level.block.Blocks;
import net.minecraftforge.client.model.generators.ItemModelBuilder;
import net.minecraftforge.client.model.generators.ItemModelProvider;
import net.minecraftforge.client.model.generators.ModelFile;
import net.minecraftforge.common.data.ExistingFileHelper;
import net.minecraftforge.registries.ForgeRegistries;

import java.util.LinkedHashMap;

public class ModItemModelProvider extends ItemModelProvider {
    private static LinkedHashMap<ResourceKey<TrimMaterial>, Float> trimMaterials = new LinkedHashMap<>();
    static {
        trimMaterials.put(TrimMaterials.QUARTZ, 0.1F);
        trimMaterials.put(TrimMaterials.IRON, 0.2F);
        trimMaterials.put(TrimMaterials.NETHERITE, 0.3F);
        trimMaterials.put(TrimMaterials.REDSTONE, 0.4F);
        trimMaterials.put(TrimMaterials.COPPER, 0.5F);
        trimMaterials.put(TrimMaterials.GOLD, 0.6F);
        trimMaterials.put(TrimMaterials.EMERALD, 0.7F);
        trimMaterials.put(TrimMaterials.DIAMOND, 0.8F);
        trimMaterials.put(TrimMaterials.LAPIS, 0.9F);
        trimMaterials.put(TrimMaterials.AMETHYST, 1.0F);
    }
    public ModItemModelProvider(PackOutput output, ExistingFileHelper existingFileHelper) {
        super(output, TechPlusPlus.MOD_ID, existingFileHelper);
    }

    @Override
    protected void registerModels() {
        simpleItem(ModItems.SAPPHIRE.get());

        simpleItem(ModItems.PLASTIMETAL_INGOT.get());
        simpleItem(ModItems.PLASTIMETAL_NUGGET.get());
        simpleItem(ModItems.RAW_PLASTIMETAL.get());

        simpleItem(ModItems.COPPER_NUGGET.get());

        simpleItem(ModItems.TIN_INGOT.get());
        simpleItem(ModItems.TIN_NUGGET.get());
        simpleItem(ModItems.RAW_TIN.get());

        simpleItem(ModItems.BRONZE_INGOT.get());
        simpleItem(ModItems.BRONZE_NUGGET.get());
        simpleItem(ModItems.RAW_BRONZE.get());

        simpleItem(ModItems.GPS_TOOL.get());

        simpleItem(ModItems.COFFEE_FOOD.get());
        simpleItem(ModItems.COFFEE_BEANS.get());
        simpleItem(ModItems.PLANT_FIBERS.get());
        simpleItem(ModItems.FIBROSIA_SEEDS.get());

        simpleItem(ModItems.STICK_BUNDLE.get());
        simpleItem(ModItems.KNAPPED_FLINT.get());
        simpleItem(ModItems.WOODEN_HANDLE.get());

        evenSimplerBlockItem(ModBlocks.GLASS_STAIRS.get());
        evenSimplerBlockItem(ModBlocks.GLASS_SLAB.get());
        evenSimplerBlockItem(ModBlocks.GLASS_PRESSURE_PLATE.get());
        evenSimplerBlockItem(ModBlocks.GLASS_FENCE_GATE.get());
        fenceItem(ModBlocks.GLASS_FENCE.get(), Blocks.GLASS);
        buttonItem(ModBlocks.GLASS_BUTTON.get(), Blocks.GLASS);
        wallItem(ModBlocks.GLASS_WALL.get(), Blocks.GLASS);
        simpleBlockItem(ModBlocks.GLASS_DOOR.get());
        trapdoorItem(ModBlocks.GLASS_TRAPDOOR.get());

        simpleBlockItem(ModBlocks.PLASTIMETAL_DOOR.get());
        trapdoorItem(ModBlocks.PLASTIMETAL_TRAPDOOR.get());
        handheldBlock(ModBlocks.PLASTIMETAL_BARS.get());

        handheldItem(ModItems.PLASTIMETAL_SWORD.get());
        handheldItem(ModItems.PLASTIMETAL_PICKAXE.get());
        handheldItem(ModItems.PLASTIMETAL_AXE.get());
        handheldItem(ModItems.PLASTIMETAL_SHOVEL.get());
        handheldItem(ModItems.PLASTIMETAL_HOE.get());

//        handheldItem(TweakedVanillaItems.IRON_AXE.get(), "minecraft");

        trimmedArmorItem(ModItems.PLASTIMETAL_HELMET.get());
        trimmedArmorItem(ModItems.PLASTIMETAL_CHESTPLATE.get());
        trimmedArmorItem(ModItems.PLASTIMETAL_LEGGINGS.get());
        trimmedArmorItem(ModItems.PLASTIMETAL_BOOTS.get());
        simpleItem(ModItems.PLASTIMETAL_HORSE_ARMOR.get());

//        handheldItem(ModItems.WOODEN_SPEAR.get()); // We have custom jsons for this
        handheldItem(ModItems.FLINT_KNIFE.get());
        handheldItem(ModItems.STONE_MATTOCK.get());

        simpleBlockItem(ModBlocks.TINY_ROCK_BLOCK.get());
        simpleBlockItem(ModBlocks.TINY_LOG_BLOCK.get());
    }

    // Shoutout to El_Redstoniano for making this
    private void trimmedArmorItem(Item item) {
        final String MOD_ID = TechPlusPlus.MOD_ID; // Change this to your mod id

        if (item instanceof ArmorItem armorItem) {
            trimMaterials.entrySet().forEach(entry -> {

                ResourceKey<TrimMaterial> trimMaterial = entry.getKey();
                float trimValue = entry.getValue();

                String armorType = switch (armorItem.getEquipmentSlot()) {
                    case HEAD -> "helmet";
                    case CHEST -> "chestplate";
                    case LEGS -> "leggings";
                    case FEET -> "boots";
                    default -> "";
                };

                String armorItemPath = "item/" + armorItem;
                String trimPath = "trims/items/" + armorType + "_trim_" + trimMaterial.location().getPath();
                String currentTrimName = armorItemPath + "_" + trimMaterial.location().getPath() + "_trim";
                ResourceLocation armorItemResLoc = new ResourceLocation(MOD_ID, armorItemPath);
                ResourceLocation trimResLoc = new ResourceLocation(trimPath); // minecraft namespace
                ResourceLocation trimNameResLoc = new ResourceLocation(MOD_ID, currentTrimName);

                // This is used for making the ExistingFileHelper acknowledge that this texture exist, so this will
                // avoid an IllegalArgumentException
                existingFileHelper.trackGenerated(trimResLoc, PackType.CLIENT_RESOURCES, ".png", "textures");

                // Trimmed armorItem files
                getBuilder(currentTrimName)
                        .parent(new ModelFile.UncheckedModelFile("item/generated"))
                        .texture("layer0", armorItemResLoc)
                        .texture("layer1", trimResLoc);

                // Non-trimmed armorItem file (normal variant)
                this.withExistingParent(ForgeRegistries.ITEMS.getKey(armorItem).getPath(),
                                mcLoc("item/generated"))
                        .override()
                        .model(new ModelFile.UncheckedModelFile(trimNameResLoc))
                        .predicate(mcLoc("trim_type"), trimValue).end()
                        .texture("layer0",
                                new ResourceLocation(MOD_ID,
                                        "item/" + ForgeRegistries.ITEMS.getKey(armorItem).getPath()));
            });
        }
    }

    private ItemModelBuilder simpleItem(Item item) {
        return withExistingParent(ForgeRegistries.ITEMS.getKey(item).getPath(),
                new ResourceLocation("item/generated")).texture("layer0",
                new ResourceLocation(TechPlusPlus.MOD_ID, "item/" + ForgeRegistries.ITEMS.getKey(item).getPath()));
    }

    public void trapdoorItem(Block block) {
        this.withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(),
                modLoc("block/" + ForgeRegistries.BLOCKS.getKey(block).getPath() + "_bottom"));
    }

    public void fenceItem(Block block, Block baseBlock) {
        this.withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(), mcLoc("block/fence_inventory"))
                .texture("texture",  new ResourceLocation(TechPlusPlus.MOD_ID, "block/" + ForgeRegistries.BLOCKS.getKey(baseBlock).getPath()));
    }

    public void buttonItem(Block block, Block baseBlock) {
        this.withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(), mcLoc("block/button_inventory"))
                .texture("texture",  new ResourceLocation(TechPlusPlus.MOD_ID, "block/" + ForgeRegistries.BLOCKS.getKey(baseBlock).getPath()));
    }

    public void wallItem(Block block, Block baseBlock) {
        this.withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(), mcLoc("block/wall_inventory"))
                .texture("wall",  new ResourceLocation(TechPlusPlus.MOD_ID, "block/" + ForgeRegistries.BLOCKS.getKey(baseBlock).getPath()));
    }

    public void evenSimplerBlockItem(Block block) {
        this.withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(),
                new ResourceLocation(TechPlusPlus.MOD_ID, "block/" + ForgeRegistries.BLOCKS.getKey(block).getPath()));
    }

    private ItemModelBuilder handheldBlock(Block block) {
        return withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(),
                new ResourceLocation("item/handheld")).texture("layer0",
                new ResourceLocation(TechPlusPlus.MOD_ID,"block/" + ForgeRegistries.BLOCKS.getKey(block).getPath()));
    }

    private ItemModelBuilder handheldItem(Item item, String modid) {
        return withExistingParent(ForgeRegistries.ITEMS.getKey(item).getPath(),
                new ResourceLocation("item/handheld")).texture("layer0",
                new ResourceLocation(modid,"item/" + ForgeRegistries.ITEMS.getKey(item).getPath()));
    }

    private ItemModelBuilder handheldItem(Item item) {
        return handheldItem(item, TechPlusPlus.MOD_ID);
    }

    private ItemModelBuilder simpleBlockItem(Block block) {
        return withExistingParent(ForgeRegistries.BLOCKS.getKey(block).getPath(),
                new ResourceLocation("item/generated")).texture("layer0",
                new ResourceLocation(TechPlusPlus.MOD_ID,"item/" + ForgeRegistries.BLOCKS.getKey(block).getPath()));
    }
}
