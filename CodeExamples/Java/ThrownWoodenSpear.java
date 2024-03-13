package net.deddybones.techplusplus.entity.custom;

import javax.annotation.Nullable;

import net.deddybones.techplusplus.item.ModItems;
import net.deddybones.techplusplus.entity.ModEntities;
import net.minecraft.nbt.CompoundTag;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.sounds.SoundEvents;
import net.minecraft.tags.EntityTypeTags;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.entity.projectile.AbstractArrow;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.enchantment.EnchantmentHelper;
import net.minecraft.world.level.Level;
import net.minecraft.world.phys.EntityHitResult;
import net.minecraft.world.phys.Vec3;
import org.jetbrains.annotations.NotNull;

public class ThrownWoodenSpear extends AbstractArrow {
    private static final ItemStack DEFAULT_ARROW_STACK = new ItemStack(ModItems.WOODEN_SPEAR.get());
    private boolean dealtDamage;

    public ThrownWoodenSpear(EntityType<? extends ThrownWoodenSpear> pEntityType, Level pLevel) {
        super(pEntityType, pLevel, DEFAULT_ARROW_STACK);
    }

    public ThrownWoodenSpear(Level p_37569_, LivingEntity p_37570_, ItemStack p_37571_) {
        super(ModEntities.THROWN_WOODEN_SPEAR_ENTITY_TYPE.get(), p_37570_, p_37569_, p_37571_);
    }

    protected void defineSynchedData() {
        super.defineSynchedData();
    }

    public void tick() {
        if (this.inGroundTime > 4) {
            this.dealtDamage = true;
        }

        super.tick();
    }

    @Nullable
    protected EntityHitResult findHitEntity(@NotNull Vec3 p_37575_, @NotNull Vec3 p_37576_) {
        return this.dealtDamage ? null : super.findHitEntity(p_37575_, p_37576_);
    }

    protected void onHitEntity(EntityHitResult p_37573_) {
        Entity entity = p_37573_.getEntity();
        float f = 8.0F;
        if (entity instanceof LivingEntity livingentity) {
            f += EnchantmentHelper.getDamageBonus(this.getPickupItemStackOrigin(), livingentity.getMobType());
        }

        Entity entity1 = this.getOwner();
        DamageSource damagesource = this.damageSources().trident(this, entity1 == null ? this : entity1);
        this.dealtDamage = true;
        SoundEvent soundevent = SoundEvents.TRIDENT_HIT;
        if (entity.hurt(damagesource, f)) {
            if (entity.getType() == EntityType.ENDERMAN) {
                return;
            }

            if (entity instanceof LivingEntity livingEntity) {
                if (entity1 instanceof LivingEntity) {
                    EnchantmentHelper.doPostHurtEffects(livingEntity, entity1);
                    EnchantmentHelper.doPostDamageEffects((LivingEntity)entity1, livingEntity);
                }

                this.doPostHurtEffects(livingEntity);
            }
        } else if (entity.getType().is(EntityTypeTags.DEFLECTS_TRIDENTS)) {
            this.deflect();
            return;
        }

        this.setDeltaMovement(this.getDeltaMovement().multiply(-0.01D, -0.1D, -0.01D));
        this.playSound(soundevent, 1.0F, 1.0F);
    }

    protected boolean tryPickup(@NotNull Player p_150196_) {
        return super.tryPickup(p_150196_) || this.isNoPhysics() && p_150196_.getInventory().add(this.getPickupItem());
    }

    protected @NotNull SoundEvent getDefaultHitGroundSoundEvent() {
        return SoundEvents.TRIDENT_HIT_GROUND;
    }

    public void playerTouch(@NotNull Player p_37580_) {
        super.playerTouch(p_37580_);
    }

    public void readAdditionalSaveData(@NotNull CompoundTag p_37578_) {
        super.readAdditionalSaveData(p_37578_);
        this.dealtDamage = p_37578_.getBoolean("DealtDamage");
    }

    public void addAdditionalSaveData(@NotNull CompoundTag p_37582_) {
        super.addAdditionalSaveData(p_37582_);
        p_37582_.putBoolean("DealtDamage", this.dealtDamage);
    }

    public void tickDespawn() {
        if (this.pickup != AbstractArrow.Pickup.ALLOWED) {
            super.tickDespawn();
        }

    }

    protected float getWaterInertia() {
        return 0.99F;
    }

    public boolean shouldRender(double p_37588_, double p_37589_, double p_37590_) {
        return true;
    }
}
