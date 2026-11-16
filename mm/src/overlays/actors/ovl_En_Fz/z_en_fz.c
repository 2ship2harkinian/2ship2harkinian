/*
 * File: z_en_fz.c
 * Overlay: ovl_En_Fz
 * Description: Freezard
 */

#include "z_en_fz.h"
#include "overlays/actors/ovl_En_Wiz/z_en_wiz.h"
#include "objects/object_fz/object_fz.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnFz_Init(Actor* thisx, PlayState* play);
void EnFz_Destroy(Actor* thisx, PlayState* play);
void EnFz_Update(Actor* thisx, PlayState* play);
void EnFz_Draw(Actor* thisx, PlayState* play);

void EnFz_UpdateTargetPos(EnFz* this, PlayState* play);
void EnFz_SpawnMistHidden(EnFz* this);
void EnFz_SpawnMistChanging(EnFz* this);
void EnFz_SpawnMistFullSize(EnFz* this);
void EnFz_SetupDisappear(EnFz* this);
void EnFz_Disappear(EnFz* this, PlayState* play);
void EnFz_SetupWait(EnFz* this);
void EnFz_Wait(EnFz* this, PlayState* play);
void EnFz_SetupAppear(EnFz* this);
void EnFz_Appear(EnFz* this, PlayState* play);
void EnFz_SetupAimForSkate(EnFz* this);
void EnFz_AimForSkate(EnFz* this, PlayState* play);
void EnFz_SetupSkateTowardPlayer(EnFz* this);
void EnFz_SkateTowardPlayer(EnFz* this, PlayState* play);
void EnFz_SetupSkatingAimFreeze(EnFz* this);
void EnFz_SkatingAimFreeze(EnFz* this, PlayState* play);
void EnFz_SetupSkatingFreeze(EnFz* this, PlayState* play);
void EnFz_SkatingFreeze(EnFz* this, PlayState* play);
void EnFz_SetupDie(EnFz* this, PlayState* play);
void EnFz_Die(EnFz* this, PlayState* play);
void EnFz_SetupMelt(EnFz* this);
void EnFz_Melt(EnFz* this, PlayState* play);
void EnFz_SetupIdleStationary(EnFz* this);
void EnFz_IdleStationary(EnFz* this, PlayState* play);
void EnFz_SetupPassive(EnFz* this);
void EnFz_Passive(EnFz* this, PlayState* play);
void EnFz_SpawnMistAura(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale);
void EnFz_SpawnBreath(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                      s16 primAlpha, u8 damaging);
void EnFz_UpdateEffects(EnFz* this, PlayState* play);
void EnFz_DrawEffects(EnFz* this, PlayState* play);

ActorProfile En_Fz_Profile = {
    /**/ ACTOR_EN_FZ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FZ,
    /**/ sizeof(EnFz),
    /**/ EnFz_Init,
    /**/ EnFz_Destroy,
    /**/ EnFz_Update,
    /**/ EnFz_Draw,
};

static s16 sTurningLimits[] = { 0, 0x2000, 0x4000, 0 };

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFEFDD, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x00001022, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 35, 80, 0, { 0, 0, 0 } },
};

// Used by the breath attack
static ColliderCylinderInitType1 sCylinderInit3 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x02, 0x04 }, // DMG_UNBLOCKABLE
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

typedef enum EnFzDamageEffect {
    /* 0x0 */ FZ_DMGEFF_NONE,
    /* 0x2 */ FZ_DMGEFF_FIRE = 0x2,
    /* 0x4 */ FZ_DMGEFF_LIGHT = 0x4,
    /* 0xD */ FZ_DMGEFF_BOUNCE = 0xD,
    /* 0xE */ FZ_DMGEFF_E,
    /* 0xF */ FZ_DMGEFF_CHIP
} EnFzDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(0, FZ_DMGEFF_BOUNCE),
    /* Horse trample  */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(2, FZ_DMGEFF_CHIP),
    /* Zora boomerang */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(0, FZ_DMGEFF_BOUNCE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, FZ_DMGEFF_CHIP),
    /* Hookshot       */ DMG_ENTRY(3, FZ_DMGEFF_CHIP),
    /* Goron punch    */ DMG_ENTRY(2, FZ_DMGEFF_CHIP),
    /* Sword          */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
    /* Goron pound    */ DMG_ENTRY(3, FZ_DMGEFF_CHIP),
    /* Fire arrow     */ DMG_ENTRY(2, FZ_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(2, FZ_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
    /* Deku spin      */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, FZ_DMGEFF_E), // not coded in damage handle, not in OOT
    /* Thrown object  */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
    /* Zora punch     */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
    /* Spin attack    */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
    /* Sword beam     */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, FZ_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, FZ_DMGEFF_CHIP),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_FREEZARD, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void EnFz_Init(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.health = 3; // decreased from 6 in OOT

    Collider_InitCylinder(play, &this->collider1);
    Collider_SetCylinderType1(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitCylinder(play, &this->collider2);
    Collider_SetCylinderType1(play, &this->collider2, &this->actor, &sCylinderInit2);
    Collider_InitCylinder(play, &this->collider3);
    Collider_SetCylinderType1(play, &this->collider3, &this->actor, &sCylinderInit3);

    Actor_SetScale(&this->actor, 0.008f);
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unusedTimer = 0;
    this->hitCounter = 0;
    this->isBgEnabled = true;
    this->isMoving = false;
    this->isColliderActive = false;
    this->drawBody = true;
    this->isDying = false;
    this->actor.speed = 0.0f;
    this->actor.cullingVolumeScale = 400.0f;
    this->originPos.x = this->actor.world.pos.x;
    this->originPos.y = this->actor.world.pos.y;
    this->originPos.z = this->actor.world.pos.z;
    this->originPosY = this->actor.world.pos.y;
    this->actor.velocity.y = this->actor.gravity;
    this->unkBB8 = 135.0f;

    if (ENFZ_GET_TRACK_TYPE(&this->actor)) {
        this->envAlpha = 0;
        this->actor.scale.y = 0.0f;
        EnFz_SetupWait(this);
    } else {
        this->envAlpha = 255;
        if (ENFZ_GETZ_CLOCK(thisx) == 0) {
            this->internalClock = 192 + (s32)Rand_ZeroFloat(64.0f);
        } else {
            if (ENFZ_GETZ_CLOCK(thisx) < 0) {
                ENFZ_GETZ_CLOCK(thisx) = 1;
            } else if (ENFZ_GETZ_CLOCK(thisx) > 0x10) {
                ENFZ_GETZ_CLOCK(thisx) = 0x10;
            }
            ENFZ_GETZ_CLOCK(thisx) -= 1;
            this->internalClock = ENFZ_GETZ_CLOCK(thisx) * 0x10;
        }
        this->actor.shape.rot.z = 0; // reset after parameter use

        if (ENFZ_GET_APPEAR_TYPE(&this->actor)) {
            this->envAlpha = 0;
            this->actor.scale.y = 0.0f;
            EnFz_SetupWait(this);
        } else if (ENFZ_GET_POWER(&this->actor) == FZ_POWER_PASSIVE) {
            EnFz_SetupPassive(this);
        } else {
            EnFz_SetupIdleStationary(this);
        }
    }

    this->drawDmgEffTimer = 0;
    this->drawDmgEffScale = 0.0f;
    this->drawDmgEffAlpha = 0.0f;
    EnFz_UpdateTargetPos(this, play);
}

void EnFz_Destroy(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
    Collider_DestroyCylinder(play, &this->collider3);

    if ((this->actor.parent != NULL) && (this->wizrobeFlag == 0) && (this->actor.parent->id == ACTOR_EN_WIZ) &&
        (this->actor.parent->update != NULL) && (((EnWiz*)this->actor.parent)->freezard != NULL)) {
        EnWiz* wiz = (EnWiz*)this->actor.parent;

        wiz->freezard = NULL;
    }
}

void EnFz_UpdateTargetPos(EnFz* this, PlayState* play) {
    Vec3f pos;
    Vec3f hitPos;
    Vec3f baseVelocity;
    s32 bgId;
    CollisionPoly* hitPoly;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 20.0f;
    pos.z = this->actor.world.pos.z;

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, MTXMODE_APPLY);

    baseVelocity.x = baseVelocity.y = 0.0f;
    baseVelocity.z = 440.0f;

    Matrix_MultVec3f(&baseVelocity, &this->wallHitPos);
    if (BgCheck_EntityLineTest1(&play->colCtx, &pos, &this->wallHitPos, &hitPos, &hitPoly, true, false, false, true,
                                &bgId)) {
        Math_Vec3f_Copy(&this->wallHitPos, &hitPos);
    }

    pos.x = this->actor.world.pos.x - this->wallHitPos.x;
    pos.z = this->actor.world.pos.z - this->wallHitPos.z;
    this->distToTargetSq = SQ(pos.x) + SQ(pos.z);
}

s32 EnFz_ReachedTarget(EnFz* this, Vec3f* vec) {
    f32 distX = this->actor.world.pos.x - vec->x;
    f32 distZ = this->actor.world.pos.z - vec->z;

    if (this->distToTargetSq <= (SQ(distX) + SQ(distZ))) {
        return true;
    } else {
        return false;
    }
}

// Spawn ice chunks on damage taken
void EnFz_Damaged(EnFz* this, PlayState* play, Vec3f* hitPos, s32 numEffects, f32 randPosRange) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    f32 scale;
    s32 life;

    accel.x = accel.z = 0.0f;
    accel.y = -1.0f;
    primColor.r = 155;
    primColor.g = 255;
    primColor.b = 255;
    primColor.a = 255;
    envColor.r = 200;
    envColor.g = 200;
    envColor.b = 200;

    for (i = 0; i < numEffects; i++) {
        scale = 0.6f + Rand_CenteredFloat(0.3f);
        life = 12 + (s32)Rand_CenteredFloat(5.0f);
        pos.x = hitPos->x + Rand_CenteredFloat(randPosRange);
        pos.y = hitPos->y + Rand_ZeroFloat(randPosRange);
        pos.z = hitPos->z + Rand_CenteredFloat(randPosRange);
        velocity.x = Rand_CenteredFloat(10.0f);
        velocity.y = Rand_ZeroFloat(10.0f) + 2.0f;
        velocity.z = Rand_CenteredFloat(10.0f);
        EffectSsEnIce_Spawn(play, &pos, scale, &velocity, &accel, &primColor, &envColor, life);
    }

    CollisionCheck_SpawnShieldParticles(play, hitPos);
}

void EnFz_SpawnMistHidden(EnFz* this) {
}

void EnFz_SpawnMistChanging(EnFz* this) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((this->internalClock % 16) == 0) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(40.0f);
        pos.y = this->actor.world.pos.y + Rand_CenteredFloat(40.0f) + 30.0f;
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(40.0f);
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        EnFz_SpawnMistAura(this, &pos, &velocity, &accel, 15.0f + Rand_ZeroFloat(7.5f));
    }
}

void EnFz_SpawnMistFullSize(EnFz* this) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((this->internalClock % 4) == 0) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(40.0f);
        pos.y = this->originPosY;
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(40.0f);
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        EnFz_SpawnMistAura(this, &pos, &velocity, &accel, Rand_ZeroFloat(7.5f) + 15.0f);
    }
}

void EnFz_ApplyDamage(EnFz* this, PlayState* play) {
    Vec3f currentPos;

    if (this->isMoving && ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
                           !Actor_TestFloorInDirection(&this->actor, play, 60.0f, this->actor.world.rot.y))) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->isMoving = false;
        this->speedXZ = 0.0f;
        this->actor.speed = 0.0f;
    }

    // Unfinished Wizrobe collaboration?
    if (this->actor.parent != NULL) {
        if ((this->wizrobeFlag != 5) && (this->actor.parent->id == ACTOR_EN_WIZ)) {
            Actor* parent = this->actor.parent;

            if ((parent->update == NULL) || (parent->colChkInfo.health <= 0)) {
                this->actor.colChkInfo.health = 0;
                this->wizrobeFlag = 5;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                currentPos.x = this->actor.world.pos.x;
                currentPos.y = this->actor.world.pos.y;
                currentPos.z = this->actor.world.pos.z;
                EnFz_Damaged(this, play, &currentPos, 30, 10.0f);
                EnFz_SetupDie(this, play);
                return;
            }

            if ((this->actor.colChkInfo.health != 0) && (this->wizrobeFlag == 1)) {
                this->actor.colChkInfo.health = 0;
                this->wizrobeFlag = 5;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                currentPos.x = this->actor.world.pos.x;
                currentPos.y = this->actor.world.pos.y;
                currentPos.z = this->actor.world.pos.z;
                EnFz_Damaged(this, play, &currentPos, 30, 10.0f);
                EnFz_SetupDie(this, play);
                return;
            }
        }
    }

    if (this->isColliderActive) {
        if (ENFZ_GET_TRACK_TYPE(&this->actor) && (this->collider1.base.atFlags & AT_HIT)) {
            this->isMoving = false;
            this->speedXZ = 0.0f;
            this->collider1.base.acFlags &= ~AC_HIT;
            this->actor.speed = 0.0f;
            this->mainTimer = 10;
            EnFz_SetupDisappear(this);
        } else if (this->collider2.base.acFlags & AC_BOUNCED) {
            this->collider2.base.acFlags &= ~AC_BOUNCED;
            this->collider1.base.acFlags &= ~AC_HIT;
        } else if (this->collider1.base.acFlags & AC_HIT) {
            this->collider1.base.acFlags &= ~AC_HIT;
            switch (this->actor.colChkInfo.damageEffect) {
                case FZ_DMGEFF_LIGHT:
                    this->drawDmgEffTimer = 2 * 20;
                    this->drawDmgEffAlpha = 1.0f;
                    FALLTHROUGH;
                case FZ_DMGEFF_CHIP:
                    Actor_ApplyDamage(&this->actor);
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 8);
                    if (this->actor.colChkInfo.health != 0) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DAMAGE);
                        currentPos.x = this->actor.world.pos.x;
                        currentPos.y = this->actor.world.pos.y;
                        currentPos.z = this->actor.world.pos.z;
                        EnFz_Damaged(this, play, &currentPos, 10, 0.0f);
                        this->hitCounter++;
                        break;
                    }
                    Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                    Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                    currentPos.x = this->actor.world.pos.x;
                    currentPos.y = this->actor.world.pos.y;
                    currentPos.z = this->actor.world.pos.z;
                    EnFz_Damaged(this, play, &currentPos, 30, 10.0f);
                    EnFz_SetupDie(this, play);
                    break;

                case FZ_DMGEFF_FIRE:
                    Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                    EnFz_SetupMelt(this);
                    break;
            }
        }
    }
}

void EnFz_SetYawTowardsPlayer(EnFz* this) {
    s16 yaw = this->actor.yawTowardsPlayer;
    s32 limitIndex = ENFZ_GET_ROTATION_LIMIT(&this->actor);

    if (!ENFZ_GET_TRACK_TYPE(&this->actor)) {
        s32 homeYaw = this->actor.home.rot.y;

        if (limitIndex != 3) {
            s16 homeYawDiff = yaw - homeYaw;

            if (sTurningLimits[limitIndex] < ABS_ALT(homeYawDiff)) {
                s16 angleLimit = (homeYawDiff > 0) ? sTurningLimits[limitIndex] : -sTurningLimits[limitIndex];

                yaw = this->actor.home.rot.y + angleLimit;
            }
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, yaw, 10, 0x7D0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

/**
 * Disappearing into the ground to re-appear at actor.home
 */
void EnFz_SetupDisappear(EnFz* this) {
    this->state = FZ_STATE_CHANGING;
    this->isColliderActive = false;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnFz_Disappear;
}

void EnFz_Disappear(EnFz* this, PlayState* play) {
    this->envAlpha -= 16;
    if (this->envAlpha > 255) {
        this->envAlpha = 0;
    }

    if (Math_SmoothStepToF(&this->actor.scale.y, 0, 1.0f, 0.0005f, 0.0f) == 0.0f) {
        EnFz_SetupWait(this);
    }
}

void EnFz_SetupWait(EnFz* this) {
    this->state = FZ_STATE_HIDDEN;
    this->unkBD2 = 0;
    this->unkBD0 = 0;
    this->mainTimer = 5 * 20;

    this->actor.world.pos.x = this->originPos.x;
    this->actor.world.pos.y = this->originPos.y;
    this->actor.world.pos.z = this->originPos.z;

    if (ENFZ_GET_APPEAR_TYPE(&this->actor)) {
        this->state = FZ_STATE_CHANGING;
        this->mainTimer = 10; // slightly shorter timer for EnFz_Appear
        this->unkBD2 = 4000;
        this->actionFunc = EnFz_Appear; // skip SetupAppear
    } else {
        // ENFZ_GET_TRACK_TYPE
        this->actionFunc = EnFz_Wait;
    }
}

/**
 * Waiting in hiding for the player go get close
 */
void EnFz_Wait(EnFz* this, PlayState* play) {
    if ((this->mainTimer == 0) && (this->actor.xzDistToPlayer < 400.0f)) {
        EnFz_SetupAppear(this);
    }
}

void EnFz_SetupAppear(EnFz* this) {
    this->state = FZ_STATE_CHANGING;
    this->mainTimer = 20;
    this->unkBD2 = 4000;
    this->actionFunc = EnFz_Appear;
}

/**
 * Appearing out of the ground to attack player
 */
void EnFz_Appear(EnFz* this, PlayState* play) {
    if (this->mainTimer == 0) {

        this->envAlpha += 8;
        if (this->envAlpha > 255) {
            this->envAlpha = 255;
        }

        if (Math_SmoothStepToF(&this->actor.scale.y, 0.008f, 1.0f, 0.0005f, 0.0f) == 0.0f) {
            if (ENFZ_GET_APPEAR_TYPE(&this->actor)) {
                EnFz_SetupIdleStationary(this);
            } else { // ENFZ_GET_TRACK_TYPE
                EnFz_SetupAimForSkate(this);
            }
        }
    }
}

void EnFz_SetupAimForSkate(EnFz* this) {
    this->state = FZ_STATE_FULLSIZE;
    this->mainTimer = 2 * 20;
    this->isBgEnabled = true;
    this->isColliderActive = true;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = EnFz_AimForSkate;
}

void EnFz_AimForSkate(EnFz* this, PlayState* play) {
    EnFz_SetYawTowardsPlayer(this);
    if (this->mainTimer == 0) {
        EnFz_SetupSkateTowardPlayer(this);
    }
}

void EnFz_SetupSkateTowardPlayer(EnFz* this) {
    this->state = FZ_STATE_FULLSIZE;
    this->isMoving = true;
    this->mainTimer = 5 * 20;
    this->speedXZ = 4.0f;
    this->actionFunc = EnFz_SkateTowardPlayer;
}

void EnFz_SkateTowardPlayer(EnFz* this, PlayState* play) {
    // isMoving gets set false in EnFz_ApplyDamage
    if ((this->mainTimer == 0) || !this->isMoving) {
        EnFz_SetupSkatingAimFreeze(this);
    }
}

void EnFz_SetupSkatingAimFreeze(EnFz* this) {
    this->state = FZ_STATE_FULLSIZE;
    this->speedXZ = 0.0f;
    this->actor.speed = 0.0f;
    this->mainTimer = 2 * 20;
    this->actionFunc = EnFz_SkatingAimFreeze;
}

void EnFz_SkatingAimFreeze(EnFz* this, PlayState* play) {
    EnFz_SetYawTowardsPlayer(this);
    if (this->mainTimer == 0) {
        EnFz_SetupSkatingFreeze(this, play);
    }
}

void EnFz_SetupSkatingFreeze(EnFz* this, PlayState* play) {
    this->state = FZ_STATE_FULLSIZE;
    this->mainTimer = 4 * 20;
    this->actionFunc = EnFz_SkatingFreeze;
    EnFz_UpdateTargetPos(this, play);
}

void EnFz_SkatingFreeze(EnFz* this, PlayState* play) {
    Vec3f baseVelocity;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if (this->mainTimer == 0) {
        EnFz_SetupDisappear(this);
        return;
    }

    if (this->mainTimer > 10) {
        u8 damaging = false;
        s16 primAlpha = 150;

        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);
        if ((this->mainTimer - 10) < 16) {
            primAlpha = (this->mainTimer * 10) - 100;
        }

        accel.x = accel.z = 0.0f;
        accel.y = 0.6f;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;

        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

        baseVelocity.x = 0.0f;
        baseVelocity.y = -2.0f;
        baseVelocity.z = ((ENFZ_GET_POWER(&this->actor) == FZ_POWER_1)   ? 10.0f
                          : (ENFZ_GET_POWER(&this->actor) == FZ_POWER_2) ? 20.0f
                                                                         : 0.0f) +
                         20;

        Matrix_MultVec3f(&baseVelocity, &velocity);

        if ((this->mainTimer % 8) == 0) {
            damaging = true;
        }

        EnFz_SpawnBreath(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, damaging);

        //! @bug: this does nothing, the above function already assigned all unused effects
        pos.x += velocity.x * 0.5f;
        pos.y += velocity.y * 0.5f;
        pos.z += velocity.z * 0.5f;
        EnFz_SpawnBreath(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
    }
}

void EnFz_SetupDie(EnFz* this, PlayState* play) {
    this->state = FZ_STATE_HIDDEN;
    this->speedXZ = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->isBgEnabled = true;
    this->isColliderActive = false;
    this->isDying = true;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->drawBody = false;
    this->mainTimer = 3 * 20;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, (0xA << 4)); // drop table 0xA
    this->actionFunc = EnFz_Die;
}

void EnFz_Die(EnFz* this, PlayState* play) {
    if (this->mainTimer == 0) {
        Actor_Kill(&this->actor);
    }
}

/**
 * Fire arrow specific death
 */
void EnFz_SetupMelt(EnFz* this) {
    this->state = FZ_STATE_MELTING;
    this->isColliderActive = false;
    this->isDying = true;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->speedXZ = 0.0f;
    this->actionFunc = EnFz_Melt;
}

void EnFz_Melt(EnFz* this, PlayState* play) {
    Math_StepToF(&this->actor.scale.y, 0.0006f, 0.0006f);

    if (this->actor.scale.y < 0.006f) {
        this->actor.scale.x += 0.001f;
        this->actor.scale.z += 0.001f;
    }

    if (this->actor.scale.y < 0.004f) {
        this->envAlpha -= 20;
        if (this->envAlpha > 255) {
            this->envAlpha = 0;
        }
    }

    if (this->envAlpha == 0) {
        EnFz_SetupDie(this, play);
    }
}

void EnFz_SetupIdleStationary(EnFz* this) {
    this->state = FZ_STATE_FULLSIZE;
    this->mainTimer = 2 * 20;
    this->isBgEnabled = true;
    this->isColliderActive = true;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = EnFz_IdleStationary;
}

void EnFz_IdleStationary(EnFz* this, PlayState* play) {
    Vec3f baseVelocity;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 damaging;
    s16 primAlpha;

    if (this->internalClock & 0xC0) {
        EnFz_SetYawTowardsPlayer(this);
        EnFz_UpdateTargetPos(this, play);
        return;
    }

    damaging = false;
    primAlpha = 150;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

    if ((this->internalClock & 0x3F) >= 0x30) {
        primAlpha = 630 - ((this->internalClock & 0x3F) * 10);
    }

    accel.x = accel.z = 0.0f;
    accel.y = 0.6f;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 20.0f;
    pos.z = this->actor.world.pos.z;

    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

    baseVelocity.x = 0.0f;
    baseVelocity.y = -2.0f;
    baseVelocity.z = ((ENFZ_GET_POWER(&this->actor) == FZ_POWER_1)   ? 10.0f
                      : (ENFZ_GET_POWER(&this->actor) == FZ_POWER_2) ? 20.0f
                                                                     : 0.0f) +
                     20;

    Matrix_MultVec3f(&baseVelocity, &velocity);

    if ((this->internalClock % 8) == 0) {
        damaging = true;
    }

    EnFz_SpawnBreath(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, damaging);

    //! @bug: this does nothing, the above function already assigned all unused effects
    pos.x += velocity.x * 0.5f;
    pos.y += velocity.y * 0.5f;
    pos.z += velocity.z * 0.5f;
    EnFz_SpawnBreath(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
}

/**
 * Unfinished and unused, specified by Init as separate power
 * still spawns as a functional non-attacking variant
 */
void EnFz_SetupPassive(EnFz* this) {
    this->state = FZ_STATE_FULLSIZE;
    this->mainTimer = 2 * 20;
    this->isBgEnabled = true;
    this->isColliderActive = true;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = EnFz_Passive;
}

void EnFz_Passive(EnFz* this, PlayState* play) {
}

void EnFz_UpdateLightArrowEffects(EnFz* this, PlayState* play) {
    if (this->drawDmgEffTimer != 0) {
        if (this->drawDmgEffTimer > 0) {
            this->drawDmgEffTimer--;
        }

        if (this->drawDmgEffTimer < 20) {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.03f, 0.0f);
            this->drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;
        } else {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.5f, 0.1f, 0.02f, 0.0f);
        }
    }
}

void EnFz_Update(Actor* thisx, PlayState* play) {
    static EnFzUnkFunc sMistSpawnFunctions[] = {
        EnFz_SpawnMistHidden,
        EnFz_SpawnMistChanging,
        EnFz_SpawnMistFullSize,
        EnFz_SpawnMistFullSize,
    };
    s32 pad;
    EnFz* this = (EnFz*)thisx;

    this->internalClock++;
    DECR(this->unusedTimer);
    DECR(this->mainTimer);
    DECR(this->attackTimer);

    Actor_SetFocus(&this->actor, 50.0f);
    EnFz_ApplyDamage(this, play);

    this->actionFunc(this, play);

    if (!this->isDying) {
        Collider_UpdateCylinder(&this->actor, &this->collider1);
        Collider_UpdateCylinder(&this->actor, &this->collider2);
        if (this->isColliderActive) {
            if (this->actor.colorFilterTimer == 0) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
            }
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        }
    }

    Math_StepToF(&this->actor.speed, this->speedXZ, 0.2f);
    Actor_MoveWithGravity(&this->actor);
    if (this->isBgEnabled) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    sMistSpawnFunctions[this->state](this);
    EnFz_UpdateLightArrowEffects(this, play);
    EnFz_UpdateEffects(this, play);
}

void EnFz_Draw(Actor* thisx, PlayState* play) {
    static Gfx* sBodyDisplayLists[] = { gFreezardBodyIntactDL, gFreezardBodyHornBrokenDL, gFreezardBodyHeadBrokenDL };
    s32 pad;
    EnFz* this = (EnFz*)thisx;
    s32 bodyDlIndex = 3 - this->actor.colChkInfo.health;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actor.colChkInfo.health == 0) {
        bodyDlIndex = 2;
    }

    if (this->drawBody) {
        func_800B8118(&this->actor, play, false);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, play->state.frames % 128, 0x20, 0x20, 1, 0,
                                      (play->state.frames * 2) % 128, 0x20, 0x20, 0, 1, 0, 2));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, ENVIRONMENT, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 155, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, this->envAlpha);
        gSPDisplayList(POLY_XLU_DISP++, sBodyDisplayLists[bodyDlIndex]);
    }

    EnFz_DrawEffects(this, play);

    if (this->drawDmgEffTimer > 0) {
        s32 pad2[6]; // what in tarnation
        Vec3f bodyPartsPos[2];
        s32 pad3;

        bodyPartsPos[0] = this->actor.world.pos;
        bodyPartsPos[1] = this->actor.world.pos;
        bodyPartsPos[0].y += 20.0f;
        bodyPartsPos[1].y += 40.0f;
        if (GameInteractor_Should(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, true, this, bodyPartsPos,
                                  ARRAY_COUNT(bodyPartsPos))) {
            Actor_DrawDamageEffects(play, NULL, bodyPartsPos, ARRAY_COUNT(bodyPartsPos), this->drawDmgEffScale * 4.0f,
                                    0.5f, this->drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * The rising cloud of mist coming from the Freezard's body
 */
void EnFz_SpawnMistAura(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale) {
    s16 i;
    EnFzEffect* effect = &this->effects[0];

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->type == FZ_EFFECT_DISABLED) {
            effect->type = FZ_EFFECT_MIST_AURA;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->primAlphaState = 0;
            effect->xyScale = xyScale / 1000.0f;
            effect->primAlpha = 0;
            effect->timer = 0;
            break;
        }
    }
}

/**
 * The breath attack stream is made up of small steam effects
 */
void EnFz_SpawnBreath(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                      s16 primAlpha, u8 damaging) {
    s16 i;
    EnFzEffect* effect = &this->effects[0];

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->type == FZ_EFFECT_DISABLED) {
            effect->type = FZ_EFFECT_BREATH;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->primAlphaState = 0;
            effect->xyScale = xyScale / 1000.0f;
            effect->xyScaleTarget = xyScaleTarget / 1000.0f;
            effect->primAlpha = primAlpha;
            effect->timer = 0;
            effect->damaging = damaging;
            break;
        }
    }
}

void EnFz_UpdateEffects(EnFz* this, PlayState* play) {
    s16 i;
    EnFzEffect* effect = this->effects;
    Vec3f pos;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->type != FZ_EFFECT_DISABLED) {
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->timer++;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (effect->type == FZ_EFFECT_MIST_AURA) {
                if (effect->primAlphaState == 0) {
                    effect->primAlpha += 10;
                    if (effect->primAlpha >= 100) {
                        effect->primAlphaState++;
                    }
                } else {
                    effect->primAlpha -= 3;
                    if (effect->primAlpha <= 0) {
                        effect->primAlpha = 0;
                        effect->type = FZ_EFFECT_DISABLED;
                    }
                }
            } else if (effect->type == FZ_EFFECT_BREATH) {
                Math_ApproachF(&effect->xyScale, effect->xyScaleTarget, 0.1f, effect->xyScaleTarget / 10.0f);
                if (effect->primAlphaState == 0) {
                    if (effect->timer >= 7) {
                        effect->primAlphaState++;
                    }
                } else {
                    effect->velocity.x *= 0.75f;
                    effect->accel.y = 2.0f;
                    effect->velocity.z *= 0.75f;
                    effect->primAlpha -= 17;
                    if (effect->primAlpha <= 0) {
                        effect->primAlpha = 0;
                        effect->type = FZ_EFFECT_DISABLED;
                    }
                }

                // note: attackTimer is never set, always zero
                if ((this->attackTimer == 0) && (effect->primAlpha > 100) && effect->damaging) {
                    this->collider3.dim.pos.x = effect->pos.x;
                    this->collider3.dim.pos.y = effect->pos.y;
                    this->collider3.dim.pos.z = effect->pos.z;
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider3.base);
                }

                pos.x = effect->pos.x;
                pos.y = effect->pos.y + 10.0f;
                pos.z = effect->pos.z;

                if ((effect->primAlphaState != 2) && EnFz_ReachedTarget(this, &pos)) {
                    effect->primAlphaState = 2;
                    effect->velocity.x = 0.0f;
                    effect->velocity.z = 0.0f;
                }
            }
        }
    }
}

void EnFz_DrawEffects(EnFz* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    u8 materialLoaded = false;
    EnFzEffect* effect = this->effects;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_PATTERN);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->type > FZ_EFFECT_DISABLED) {
            FrameInterpolation_RecordOpenChild(effect, i);
            gDPPipeSync(POLY_XLU_DISP++);

            if (materialLoaded == false) {
                // surely there is a better way to make sure this only goes off if there are effects than to force
                // another branch per loop
                gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamMaterialDL);
                materialLoaded++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, effect->primAlpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (effect->timer + (i * 3)) * 3,
                                          (effect->timer + (i * 3)) * 15, 0x20, 0x40, 1, 0, 0, 0x20, 0x20, 3, 15, 0,
                                          0));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->xyScale, effect->xyScale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
