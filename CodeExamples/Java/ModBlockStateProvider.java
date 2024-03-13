package net.deddybones.techplusplus.datagen;

import net.deddybones.techplusplus.TechPlusPlus;
import net.deddybones.techplusplus.block.ModBlocks;
import static net.deddybones.techplusplus.datagen.custom.ModHelper.*;
import net.deddybones.techplusplus.block.custom.*;
import net.minecraft.core.Direction;
import net.minecraft.data.PackOutput;
import net.minecraft.resources.ResourceLocation;
import net.minecraft.world.level.block.*;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.block.state.properties.Half;
import net.minecraft.world.level.block.state.properties.SlabType;
import net.minecraft.world.level.block.state.properties.StairsShape;
import net.minecraftforge.client.model.generators.*;
import net.minecraftforge.common.data.ExistingFileHelper;
import net.minecraftforge.registries.RegistryObject;

import java.util.function.Function;

public class ModBlockStateProvider extends BlockStateProvider {
    public ModBlockStateProvider(PackOutput output, ExistingFileHelper exFileHelper) {
        super(output, TechPlusPlus.MOD_ID, exFileHelper);
    }

    @Override
    protected void registerStatesAndModels() {
        blockWithItem(ModBlocks.BAUXITE);
        blockWithItem(ModBlocks.POLISHED_BAUXITE);

        blockWithItem(ModBlocks.SAPPHIRE_BLOCK);
        blockWithItem(ModBlocks.PLASTIMETAL_BLOCK);

        blockWithItem(ModBlocks.SAPPHIRE_ORE);
        blockWithItem(ModBlocks.DEEPSLATE_SAPPHIRE_ORE);

        blockWithItem(ModBlocks.RAW_PLASTIMETAL_BLOCK);
        blockWithItem(ModBlocks.RUINED_PLASTIMETAL);
        blockWithItem(ModBlocks.DEEPSLATE_RUINED_PLASTIMETAL);

        blockWithItem(ModBlocks.TEST_BLOCK);

        transparentStairsBlockWithRenderType(((TransparentStairBlock) ModBlocks.GLASS_STAIRS.get()), modLoc("block/glass_grid"), modLoc("block/glass_grid"), modLoc("block/glass_grid"), "cutout");
        transparentSlabBlockWithRenderType(((TransparentSlabBlock) ModBlocks.GLASS_SLAB.get()), blockTexture(Blocks.GLASS), modLoc("block/glass_slab"), blockTexture(Blocks.GLASS), blockTexture(Blocks.GLASS), "cutout");
        buttonBlockWithRenderType(((ButtonBlock) ModBlocks.GLASS_BUTTON.get()), blockTexture(Blocks.GLASS), "cutout");
        pressurePlateBlockWithRenderType(((PressurePlateBlock) ModBlocks.GLASS_PRESSURE_PLATE.get()), blockTexture(Blocks.GLASS), "cutout");
        fenceBlockWithRenderType(((FenceBlock) ModBlocks.GLASS_FENCE.get()), blockTexture(Blocks.GLASS), "cutout");
        fenceGateBlockWithRenderType(((FenceGateBlock) ModBlocks.GLASS_FENCE_GATE.get()), blockTexture(Blocks.GLASS), "cutout");
        wallBlockWithRenderType(((WallBlock) ModBlocks.GLASS_WALL.get()), blockTexture(Blocks.GLASS), "cutout");
        doorBlockWithRenderType(((DoorBlock) ModBlocks.GLASS_DOOR.get()), modLoc("block/glass_door_bottom"), modLoc("block/glass_door_top"), "cutout");
        trapdoorBlockWithRenderType(((TrapDoorBlock) ModBlocks.GLASS_TRAPDOOR.get()), modLoc("block/glass_trapdoor"), true, "cutout");

        doorBlockWithRenderType(((DoorBlock) ModBlocks.PLASTIMETAL_DOOR.get()), modLoc("block/plastimetal_door_bottom"), modLoc("block/plastimetal_door_top"), "cutout");
        trapdoorBlockWithRenderType(((TrapDoorBlock) ModBlocks.PLASTIMETAL_TRAPDOOR.get()), modLoc("block/plastimetal_trapdoor"), true, "cutout");
        paneBlockWithRenderType((IronBarsBlock) ModBlocks.PLASTIMETAL_BARS.get(), modLoc("block/plastimetal_bars"), modLoc("block/plastimetal_bars"), "cutout");

//        makeCrop((ModCropBlock) ModBlocks.COFFEE_CROP.get(), "coffee_stage", "coffee_stage");
        makeCrop((FibrosiaCropBlock) ModBlocks.FIBROSIA_CROP.get(), "fibrosia_stage", "fibrosia_stage");

        goodieBlock((RockGoodieBlock) ModBlocks.TINY_ROCK_BLOCK.get(), new ModelFile.UncheckedModelFile(modLoc("block/tiny_rock_block")));
        goodieBlock((LogGoodieBlock) ModBlocks.TINY_LOG_BLOCK.get(), new ModelFile.UncheckedModelFile(modLoc("block/tiny_log_block")));
    }

    public void goodieBlock(GoodieBlock pBlock, ModelFile pModel) {
        getVariantBuilder(pBlock)
                .forAllStatesExcept(state -> {
                    int yRot = state.getValue(GoodieBlock.LEFT_RIGHT) ? 90 : 0;
                    return ConfiguredModel.builder()
                            .modelFile(pModel)
                            .rotationX(0)
                            .rotationY(yRot)
                            .uvLock(false)
                            .build();
                });
    }

    public <T extends CropBlock> void makeCrop(ModCropBlock block, String modelName, String textureName) {
        Function<BlockState, ConfiguredModel[]> function = state -> cropStates(state, block, modelName, textureName); // purple/underlined bc from outside lambda

        getVariantBuilder(block).forAllStates(function);
    }

    private ConfiguredModel[] cropStates(BlockState state, ModCropBlock block, String modelName, String textureName) {
        ConfiguredModel[] models = new ConfiguredModel[1];
        models[0] = new ConfiguredModel(models().crop(modelName + state.getValue(block.getAgeProperty()),
                new ResourceLocation(TechPlusPlus.MOD_ID, "block/" + textureName + state.getValue((block.getAgeProperty())))).renderType("cutout"));
        return models;
    }

    public void transparentStairsBlock(TransparentStairBlock block, ResourceLocation texture) {
        transparentStairsBlock(block, texture, texture, texture);
    }

    public void transparentStairsBlock(TransparentStairBlock block, String name, ResourceLocation texture) {
        transparentStairsBlock(block, name, texture, texture, texture);
    }

    public void transparentStairsBlock(TransparentStairBlock block, ResourceLocation side, ResourceLocation bottom, ResourceLocation top) {
        transparentStairsBlockInternal(block, key(block).toString(), side, bottom, top);
    }

    public void transparentStairsBlock(TransparentStairBlock block, String name, ResourceLocation side, ResourceLocation bottom, ResourceLocation top) {
        transparentStairsBlockInternal(block, name + "_stairs", side, bottom, top);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, ResourceLocation texture, String renderType) {
        transparentStairsBlockWithRenderType(block, texture, texture, texture, renderType);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, String name, ResourceLocation texture, String renderType) {
        transparentStairsBlockWithRenderType(block, name, texture, texture, texture, renderType);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, String renderType) {
        transparentStairsBlockInternalWithRenderType(block, key(block).toString(), side, bottom, top, ResourceLocation.tryParse(renderType));
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, String name, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, String renderType) {
        transparentStairsBlockInternalWithRenderType(block, name + "_stairs", side, bottom, top, ResourceLocation.tryParse(renderType));
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, ResourceLocation texture, ResourceLocation renderType) {
        transparentStairsBlockWithRenderType(block, texture, texture, texture, renderType);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, String name, ResourceLocation texture, ResourceLocation renderType) {
        transparentStairsBlockWithRenderType(block, name, texture, texture, texture, renderType);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, ResourceLocation renderType) {
        transparentStairsBlockInternalWithRenderType(block, key(block).toString(), side, bottom, top, renderType);
    }

    public void transparentStairsBlockWithRenderType(TransparentStairBlock block, String name, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, ResourceLocation renderType) {
        transparentStairsBlockInternalWithRenderType(block, name + "_stairs", side, bottom, top, renderType);
    }

    private void transparentStairsBlockInternal(TransparentStairBlock block, String baseName, ResourceLocation side, ResourceLocation bottom, ResourceLocation top) {
        ModelFile stairs = models().stairs(baseName, side, bottom, top);
        ModelFile stairsInner = models().stairsInner(baseName + "_inner", side, bottom, top);
        ModelFile stairsOuter = models().stairsOuter(baseName + "_outer", side, bottom, top);
        transparentStairsBlock(block, stairs, stairsInner, stairsOuter);
    }

    private void transparentStairsBlockInternalWithRenderType(TransparentStairBlock block, String baseName, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, ResourceLocation renderType) {
        ModelFile stairs = models().stairs(baseName, side, bottom, top).renderType(renderType);
        ModelFile stairsInner = models().stairsInner(baseName + "_inner", side, bottom, top).renderType(renderType);
        ModelFile stairsOuter = models().stairsOuter(baseName + "_outer", side, bottom, top).renderType(renderType);
        transparentStairsBlock(block, stairs, stairsInner, stairsOuter);
    }

    public void transparentStairsBlock(TransparentStairBlock block, ModelFile stairs, ModelFile stairsInner, ModelFile stairsOuter) {
        getVariantBuilder(block)
                .forAllStatesExcept(state -> {
                    Direction facing = state.getValue(TransparentStairBlock.FACING);
                    Half half = state.getValue(TransparentStairBlock.HALF);
                    StairsShape shape = state.getValue(TransparentStairBlock.SHAPE);
                    int yRot = (int) facing.getClockWise().toYRot(); // Stairs model is rotated 90 degrees clockwise for some reason
                    if (shape == StairsShape.INNER_LEFT || shape == StairsShape.OUTER_LEFT) {
                        yRot += 270; // Left facing stairs are rotated 90 degrees clockwise
                    }
                    if (shape != StairsShape.STRAIGHT && half == Half.TOP) {
                        yRot += 90; // Top stairs are rotated 90 degrees clockwise
                    }
                    yRot %= 360;
                    boolean uvlock = yRot != 0 || half == Half.TOP; // Don't set uvlock for states that have no rotation
                    return ConfiguredModel.builder()
                            .modelFile(shape == StairsShape.STRAIGHT ? stairs : shape == StairsShape.INNER_LEFT || shape == StairsShape.INNER_RIGHT ? stairsInner : stairsOuter)
                            .rotationX(half == Half.BOTTOM ? 0 : 180)
                            .rotationY(yRot)
                            .uvLock(uvlock)
                            .build();
                }, TransparentStairBlock.WATERLOGGED);
    }

    public void transparentSlabBlock(TransparentSlabBlock block, ResourceLocation doubleslab, ResourceLocation texture) {
        transparentSlabBlock(block, doubleslab, texture, texture, texture);
    }

    public void transparentSlabBlock(TransparentSlabBlock block, ResourceLocation doubleslab, ResourceLocation side, ResourceLocation bottom, ResourceLocation top) {
        transparentSlabBlock(block, models().slab(name(block), side, bottom, top), models().slabTop(name(block) + "_top", side, bottom, top), models().getExistingFile(doubleslab));
    }

    public void transparentSlabBlock(TransparentSlabBlock block, ModelFile bottom, ModelFile top, ModelFile doubleslab) {
        getVariantBuilder(block)
                .partialState().with(SlabBlock.TYPE, SlabType.BOTTOM).addModels(new ConfiguredModel(bottom))
                .partialState().with(SlabBlock.TYPE, SlabType.TOP).addModels(new ConfiguredModel(top))
                .partialState().with(SlabBlock.TYPE, SlabType.DOUBLE).addModels(new ConfiguredModel(doubleslab));
    }

    public void transparentSlabBlockWithRenderType(TransparentSlabBlock block, ResourceLocation doubleslab, ResourceLocation texture, String renderType) {
        transparentSlabBlockWithRenderType(block, doubleslab, texture, texture, texture, renderType);
    }

    public void transparentSlabBlockWithRenderType(TransparentSlabBlock block, ResourceLocation doubleslab, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, String renderType) {
        transparentSlabBlock(block,
                models().slab(name(block), side, bottom, top).renderType(renderType),
                models().slabTop(name(block) + "_top", side, bottom, top).renderType(renderType),
                models().cubeAll(name(block) + "_doubleslab", doubleslab).renderType(renderType));
    }

    public void slabBlockWithRenderType(SlabBlock block, ResourceLocation doubleslab, ResourceLocation texture, String renderType) {
        slabBlockWithRenderType(block, doubleslab, texture, texture, texture, renderType);
    }

    public void slabBlockWithRenderType(SlabBlock block, ResourceLocation doubleslab, ResourceLocation side, ResourceLocation bottom, ResourceLocation top, String renderType) {
        slabBlock(block,
                models().slab(name(block), side, bottom, top).renderType(renderType),
                models().slabTop(name(block) + "_top", side, bottom, top).renderType(renderType),
                models().cubeAll(name(block) + "_doubleslab", doubleslab).renderType(renderType));
    }

    public void pressurePlateBlockWithRenderType(PressurePlateBlock block, ResourceLocation texture, String renderType) {
        ModelFile pressurePlate = models().pressurePlate(name(block), texture).renderType(renderType);
        ModelFile pressurePlateDown = models().pressurePlateDown(name(block) + "_down", texture).renderType(renderType);
        pressurePlateBlock(block, pressurePlate, pressurePlateDown);
    }

    public void buttonBlockWithRenderType(ButtonBlock block, ResourceLocation texture, String renderType) {
        ModelFile button = models().button(name(block), texture).renderType(renderType);
        ModelFile buttonPressed = models().buttonPressed(name(block) + "_pressed", texture).renderType(renderType);
        buttonBlock(block, button, buttonPressed);
    }

    private void blockWithItem(RegistryObject<Block> blockRegistryObject) {
        simpleBlockWithItem(blockRegistryObject.get(), cubeAll(blockRegistryObject.get()));
    }
}
