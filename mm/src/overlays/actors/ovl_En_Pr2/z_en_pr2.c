/*
 * File: z_en_pr2.c
 * Overlay: ovl_En_Pr2
 * Description: Skullfish
 */

#include "z_en_pr2.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPr2_Init(Actor* thisx, PlayState* play);
void EnPr2_Destroy(Actor* thisx, PlayState* play);
void EnPr2_Update(Actor* thisx, PlayState* play);
void EnPr2_Draw(Actor* thisx, PlayState* play);

void EnPr2_SetupFollowPath(EnPr2* this);
void EnPr2_FollowPath(EnPr2* this, PlayState* play);
void EnPr2_SetupIdle(EnPr2* this);
void EnPr2_Idle(EnPr2* this, PlayState* play);
void EnPr2_SetupAttack(EnPr2* this, PlayState* play);
void EnPr2_Attack(EnPr2* this, PlayState* play);
void EnPr2_SetupDie(EnPr2* this);
void EnPr2_Die(EnPr2* this, PlayState* play);

typedef enum EnPr2DamageEffect {
    /* 0x0 */ PR2_DMGEFF_NONE,
    /* 0xF */ PR2_DMGEFF_BREAK = 0xF,
} EnPr2DamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Zora boomerang */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Normal arrow   */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Goron punch    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Ice arrow      */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Light arrow    */ DMG_ENTRY(2, PR2_DMGEFF_BREAK),
    /* Goron spikes   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Deku launch    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Normal shield  */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
    /* Spin attack    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, PR2_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, PR2_DMGEFF_BREAK),
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x08, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 17, 32, -10, { 0, 0, 0 } },
};

ActorProfile En_Pr2_Profile = {
    /**/ ACTOR_EN_PR2,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PR,
    /**/ sizeof(EnPr2),
    /**/ EnPr2_Init,
    /**/ EnPr2_Destroy,
    /**/ EnPr2_Update,
    /**/ EnPr2_Draw,
};

typedef enum EnPr2Animation {
    /* 0 */ PR2_ANIM_SWIM,
    /* 1 */ PR2_ANIM_ATTACK,
    /* 2 */ PR2_ANIM_DIE,
    /* 3 */ PR2_ANIM_MAX
} EnPr2Animation;

static AnimationHeader* sAnimations[PR2_ANIM_MAX] = {
    &gSkullFishSwimAnim,   // PR2_ANIM_SWIM
    &gSkullFishAttackAnim, // PR2_ANIM_ATTACK
    &gSkullFishDieAnim,    // PR2_ANIM_DIE
};

static u8 sAnimationModes[PR2_ANIM_MAX] = {
    ANIMMODE_LOOP, // PR2_ANIM_SWIM
    ANIMMODE_LOOP, // PR2_ANIM_ATTACK
    ANIMMODE_ONCE, // PR2_ANIM_DIE
};

// ... ? why not just lshift the index by 4?
static s16 sDropTables[] = {
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
};

void EnPr2_Init(Actor* thisx, PlayState* play) {
    EnPr2* this = (EnPr2*)thisx;

    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->actor.hintId = TATL_HINT_ID_SKULLFISH;
    this->primColor = 255;
    this->actor.colChkInfo.health = 1;
    this->actor.colChkInfo.damageTable = &sDamageTable;

    SkelAnime_InitFlex(play, &this->skelAnime, &gSkullFishSkel, &gSkullFishSwimAnim, this->jointTable, this->morphTable,
                       PR_2_LIMB_MAX);
    this->type = ENPR2_GET_TYPE(&this->actor);
    this->actor.colChkInfo.mass = 10;
    Math_Vec3f_Copy(&this->newHome, &this->actor.home.pos);

    if (this->type == PR2_TYPE_RESIDENT) {
        this->agroDistance = ENPR2_GET_AGRO_DISTANCE(&this->actor) * 20.0f;
    }

    this->alpha = 255;
    this->actor.shape.yOffset = 500.0f;
    this->actor.shape.shadowScale = 12.0f;

    if (this->type < PR2_TYPE_BROKEN) { // regular spawns
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        this->dropID = -1;
        this->scale = 0.0f;

        // spawned by EnEncount1
        if (this->actor.parent != NULL) {
            Actor* encount1 = this->actor.parent;

            if (encount1->update != NULL) {
                if (ENPR2_GETY_PARENT_DROP_ID(encount1) != 0) {
                    this->dropID = ENPR2_GETY_PARENT_DROP_ID(encount1) - 1;
                }
            }
        } else if (ENPR2_GETZ_DROP_ID(thisx) != 0) {
            this->dropID = ENPR2_GETZ_DROP_ID(thisx) - 1;
            this->actor.world.rot.z = 0;
        }

        if (this->type == PR2_TYPE_PATHING) {
            if (this->actor.parent != NULL) {
                Actor* encount1 = this->actor.parent;

                if (encount1->update != NULL) {
                    this->pathIndex = ((EnEncount1*)encount1)->pathIndex;
                    this->path = SubS_GetPathByIndex(play, this->pathIndex, ENPR2_PATH_INDEX_NONE);
                    this->agroDistance = ENPR2_GETZ_PARENT_AGRO_DISTANCE(encount1) * 20.0f;
                    if (this->agroDistance < 20.0f) {
                        this->agroDistance = 20.0f;
                    }
                }
                EnPr2_SetupFollowPath(this);
            } else {
                Actor_Kill(&this->actor);
                return;
            }
        } else {
            EnPr2_SetupIdle(this);
        }
    } else { // type > PR2_TYPE_BROKEN
        // these are broken pieces floating after death
        this->scale = 0.02f;
        EnPr2_SetupDie(this);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 20.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if (!(this->actor.bgCheckFlags & (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH))) {
        // no spawning in air
        Actor_Kill(&this->actor);
    }
}

void EnPr2_Destroy(Actor* thisx, PlayState* play) {
    EnPr2* this = (EnPr2*)thisx;

    if (this->type < PR2_TYPE_BROKEN) {
        Collider_DestroyCylinder(play, &this->collider);
    }

    if (this->actor.parent != NULL) {
        EnEncount1* encount1 = (EnEncount1*)this->actor.parent;

        if ((encount1->actor.update != NULL) && (encount1->spawnActiveCount > 0)) {
            encount1->spawnActiveCount--;
        }
    }
}

s32 EnPr2_IsOnScreen(EnPr2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 screenPosX;
    s16 screenPosY;

    Actor_GetScreenPos(play, &this->actor, &screenPosX, &screenPosY);

    if ((fabsf(player->actor.world.pos.y - this->actor.world.pos.y) > 160.0f) ||
        (this->actor.projectedPos.z < -40.0f) || (screenPosX < 0) || (screenPosX > SCREEN_WIDTH) || (screenPosY < 0) ||
        (screenPosY > SCREEN_HEIGHT)) {
        return false;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_8000000)) {
        // player is NOT swimming?
        return false;
    } else {
        return true;
    }
}

void EnPr2_HandleYaw(EnPr2* this, s16 targetYRot) {
    s16 yawDiff = targetYRot - this->actor.world.rot.y;

    if (yawDiff > 10000) {
        yawDiff = 10000;
    } else if (yawDiff < -10000) {
        yawDiff = -10000;
    }

    Math_ApproachF(&this->actor.world.pos.y, this->waypointPos.y, 0.3f, 5.0f);

    if (fabsf(this->actor.world.pos.y - this->waypointPos.y) > 10.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.x,
                           Math_Vec3f_Pitch(&this->actor.world.pos, &this->waypointPos) * 0.3f, 20, 0x1388, 0x1F4);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.x, 0, 20, 0x1388, 0x1F4);
    }

    if (fabsf(this->actor.world.rot.y - targetYRot) < 30.0f) {
        Math_ApproachZeroF(&this->slowLimbYaw, 0.5f, 20.0f);
    } else {
        Math_ApproachF(&this->slowLimbYaw, yawDiff, 0.5f, 3000.0f);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, targetYRot, 1, 0x7D0, 300);
}

void EnPr2_ChangeAnim(EnPr2* this, s32 animIndex) {
    f32 playSpeed = 1.0f;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    if (this->animIndex == PR2_ANIM_MAX) {
        playSpeed = 0.0f;
    }
    Animation_Change(&this->skelAnime, sAnimations[animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[animIndex], 0.0f);
}

void EnPr2_SetupFollowPath(EnPr2* this) {
    EnPr2_ChangeAnim(this, PR2_ANIM_SWIM);
    this->state = PR2_STATE_PATHING;
    this->actionFunc = EnPr2_FollowPath;
}

void EnPr2_FollowPath(EnPr2* this, PlayState* play) {
    f32 x;
    f32 y;
    f32 z;
    f32 waypointDist;

    if (fabsf(this->actor.world.rot.y - this->yawTowardsWaypoint) < 200.0f) {
        SkelAnime_Update(&this->skelAnime);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_PIRANHA_EXIST - SFX_FLAG);
    Math_ApproachF(&this->scale, 0.02f, 0.1f, 0.005f);

    if (this->path->customValue < this->waypoint) {
        Math_ApproachF(&this->actor.speed, 5.0f, 0.3f, 1.0f);
    } else {
        Math_ApproachF(&this->actor.speed, 10.0f, 0.3f, 1.0f);
    }

    if ((this->path != NULL) && !SubS_CopyPointFromPath(this->path, this->waypoint, &this->waypointPos)) {
        Actor_Kill(&this->actor);
    }

    Math_ApproachF(&this->actor.world.pos.y, this->waypointPos.y, 0.3f, 5.0f);

    if (fabsf(this->actor.world.pos.y - this->waypointPos.y) > 10.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.x,
                           Math_Vec3f_Pitch(&this->actor.world.pos, &this->waypointPos) * 0.3f, 20, 0x1388, 0x1F4);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.x, 0, 20, 0x1388, 0x1F4);
    }

    x = this->actor.world.pos.x - this->waypointPos.x;
    y = this->actor.world.pos.y - this->waypointPos.y;
    z = this->actor.world.pos.z - this->waypointPos.z;
    waypointDist = sqrtf(SQ(x) + SQ(y) + SQ(z));

    if (waypointDist < (Rand_ZeroFloat(20.0f) + 15.0f)) {
        this->waypoint++;
        Math_Vec3f_Copy(&this->newHome, &this->actor.world.pos);
        if (this->waypoint >= this->path->count) {
            // destination reached
            this->type = PR2_TYPE_RESIDENT;
            EnPr2_SetupIdle(this);
        }
    }

    this->yawTowardsWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
    EnPr2_HandleYaw(this, this->yawTowardsWaypoint);
}

void EnPr2_SetupIdle(EnPr2* this) {
    EnPr2_ChangeAnim(this, PR2_ANIM_SWIM);
    this->idleTimer = 2;
    this->mainTimer = 0;
    Math_Vec3f_Copy(&this->waypointPos, &this->newHome);
    this->state = PR2_STATE_IDLE;
    this->actionFunc = EnPr2_Idle;
}

void EnPr2_Idle(EnPr2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 deltaX; // reused for both dist-to-player and dist-to-home
    f32 deltaZ;
    f32 sqrtXZ;
    s32 changingCourse = false;
    s32 swimmingStraight = false;
    Vec3f targetPos;

    Math_ApproachF(&this->scale, 0.02f, 0.1f, 0.005f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PIRANHA_EXIST - SFX_FLAG);

    if (fabsf(this->actor.world.rot.y - this->yawTowardsWaypoint) < 200.0f) {
        swimmingStraight = true;
        SkelAnime_Update(&this->skelAnime);
    }

    if (this->alpha != 255) { // dying
        this->actor.speed = 0.0f;
        Math_SmoothStepToS(&this->alpha, 0, 1, 30, 100);
        if (this->alpha < 2) {
            Actor_Kill(&this->actor);
        }
    } else {
        switch (this->type) {
            case PR2_TYPE_SPAWNED:
                if (this->targetingTimer == 0) {
                    changingCourse = true;
                    EnPr2_SetupAttack(this, play);
                } else if (!EnPr2_IsOnScreen(this, play) && (this->alpha == 255)) {
                    // despawn if not on screen
                    this->alpha = 254; // triggers actor kill above
                }
                break;

            case PR2_TYPE_RESIDENT:
                if (this->returnHomeTimer == 0) {
                    // distance diff from player to home
                    deltaX = player->actor.world.pos.x - this->newHome.x;
                    deltaZ = player->actor.world.pos.z - this->newHome.z;
                    sqrtXZ = sqrtf(SQ(deltaX) + SQ(deltaZ));

                    if (swimmingStraight && (player->stateFlags1 & PLAYER_STATE1_8000000) &&
                        (sqrtXZ < this->agroDistance)) {
                        changingCourse = true;
                        EnPr2_SetupAttack(this, play);
                    }
                } else {
                    // distance diff from current pos to home
                    deltaX = this->actor.world.pos.x - this->newHome.x;
                    deltaZ = this->actor.world.pos.z - this->newHome.z;
                    sqrtXZ = sqrtf(SQ(deltaX) + SQ(deltaZ));

                    if (sqrtXZ > 20.0f) {
                        this->returnHomeTimer = 5;
                        changingCourse = true;
                        this->targetingTimer = 0;
                        Math_Vec3f_Copy(&this->waypointPos, &this->newHome);
                        Math_ApproachF(&this->actor.speed, 3.0f, 0.3f, 0.2f);
                    }
                }
                break;

            default:
                break;
        }

        // if we already plan to change behavior, ignore this expensive maintanence code
        if (!changingCourse) {
            this->waypointPos.y = this->actor.world.pos.y;
            if (this->idleTimer != 0) {
                if ((Rand_ZeroOne() < 0.3f) && !this->bubbleToggle) {
                    this->bubbleToggle = true;
                }

                Math_ApproachZeroF(&this->actor.speed, 0.1f, 0.2f);

                if (this->idleTimer == 1) {
                    this->mainTimer = Rand_S16Offset(100, 100);
                    Math_Vec3f_Copy(&targetPos, &this->newHome);
                    targetPos.x += Rand_CenteredFloat(300.0f);
                    targetPos.z += Rand_CenteredFloat(300.0f);
                    Math_Vec3f_Copy(&this->waypointPos, &targetPos);
                }
            } else {
                Math_ApproachF(&this->actor.speed, 2.0f, 0.3f, 0.2f);
                Math_Vec3f_Copy(&targetPos, &this->actor.world.pos);
                targetPos.x += Math_SinS(this->actor.world.rot.y) * 20.0f;
                targetPos.z += Math_CosS(this->actor.world.rot.y) * 20.0f;
                if (fabsf(this->actor.world.rot.y - this->yawTowardsWaypoint) < 100.0f) {
                    if (BgCheck_SphVsFirstPoly(&play->colCtx, &targetPos, 20.0f) ||
                        (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
                        this->targetingTimer = 0;
                        this->wallCollisionCounter++;
                        Math_Vec3f_Copy(&this->waypointPos, &this->newHome);
                        if (this->wallCollisionCounter > 10) {
                            this->wallCollisionAngleAdjustment += 0x2000;
                        }
                    } else {
                        Math_SmoothStepToS(&this->wallCollisionAngleAdjustment, 0, 1, 0x3E8, 0x64);
                        this->wallCollisionCounter = 0;
                    }
                }

                if ((this->mainTimer == 0) || ((fabsf(this->waypointPos.x - this->actor.world.pos.x) < 10.0f) &&
                                               (fabsf(this->waypointPos.z - this->actor.world.pos.z) < 10.0f))) {
                    this->idleTimer = Rand_S16Offset(20, 30);
                }
            }
        }

        if (this->idleTimer == 0) {
            this->yawTowardsWaypoint =
                Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos) + this->wallCollisionAngleAdjustment;
            EnPr2_HandleYaw(this, this->yawTowardsWaypoint);
        }
    }
}

void EnPr2_SetupAttack(EnPr2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->wallCollisionAngleAdjustment = 0;
    EnPr2_ChangeAnim(this, PR2_ANIM_ATTACK);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PIRANHA_ATTACK);
    Math_Vec3f_Copy(&this->waypointPos, &player->actor.world.pos);

    this->yawTowardsWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
    this->randomTargetHeightOffset = Rand_ZeroFloat(30.0f);
    this->targetingTimer = 0;
    this->mainTimer = 3.5 * 20;
    this->state = PR2_STATE_ATTACKING;
    this->actionFunc = EnPr2_Attack;
}

void EnPr2_Attack(EnPr2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    WaterBox* waterBox;

    Math_ApproachF(&this->scale, 0.02f, 0.1f, 0.005f);
    if ((this->mainTimer == 0) || !(player->stateFlags1 & PLAYER_STATE1_8000000) || (this->type == PR2_TYPE_PASSIVE)) {
        EnPr2_SetupIdle(this);
        return;
    }

    if (this->alpha != 255) { // dying
        this->actor.speed = 0.0f;
        Math_SmoothStepToS(&this->alpha, 0, 1, 30, 100);
        if (this->alpha < 2) {
            Actor_Kill(&this->actor);
        }
    } else {
        SkelAnime_Update(&this->skelAnime);

        if ((this->targetingTimer == 0) && (fabsf(this->actor.world.rot.y - this->yawTowardsWaypoint) < 200.0f)) {
            Math_Vec3f_Copy(&this->waypointPos, &player->actor.world.pos);
        }

        if ((Rand_ZeroOne() < 0.3f) && !this->bubbleToggle) {
            this->bubbleToggle = true;
            this->randomTargetHeightOffset = Rand_ZeroFloat(30.0f);
        }

        this->waypointPos.y = player->actor.world.pos.y + 30.0f + this->randomTargetHeightOffset;
        Math_ApproachF(&this->actor.speed, 5.0f, 0.3f, 1.0f);
        this->wallCollisionAngleAdjustment = 0;

        if (this->type == PR2_TYPE_RESIDENT) {
            f32 deltaX = this->actor.world.pos.x - this->newHome.x;
            f32 deltaZ = this->actor.world.pos.z - this->newHome.z;
            f32 homeDistance = sqrtf(SQ(deltaX) + SQ(deltaZ));

            if (homeDistance > this->agroDistance) {
                this->returnHomeTimer = 20;
                EnPr2_SetupIdle(this);
                return;
            }
        } else {
            // home moves to follow player, like scuttlebug
            Math_Vec3f_Copy(&this->newHome, &this->actor.world.pos);
        }

        if (BgCheck_GetWaterSurfaceNoBgIdAlt(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                             &this->waterSurfaceHeight, &waterBox)) {
            if (this->waypointPos.y > (this->waterSurfaceHeight - 40.0f)) {
                this->waypointPos.y = this->waterSurfaceHeight - 40.0f;
            }
        }

        if ((this->type == PR2_TYPE_SPAWNED) && !EnPr2_IsOnScreen(this, play)) {
            if (this->alpha == 255) {
                this->alpha = 254; // triggers actor kill at top of this function
            }
        } else {
            if (this->collider.base.atFlags & AT_HIT) {
                this->targetingTimer = Rand_S16Offset(30, 30);
                this->mainTimer = 5 * 20;
                if (this->type != PR2_TYPE_RESIDENT) {
                    EnPr2_SetupIdle(this);
                }
            }
            this->yawTowardsWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
            EnPr2_HandleYaw(this, this->yawTowardsWaypoint);
        }
    }
}

void EnPr2_SetupDie(EnPr2* this) {
    this->primColor = 0;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->type < PR2_TYPE_BROKEN) {
        EnPr2_ChangeAnim(this, PR2_ANIM_DIE);
    } else {
        this->animEndFrame = Animation_GetLastFrame(&gSkullFishDieAnim);
        Animation_Change(&this->skelAnime, &gSkullFishDieAnim, 1.0f, this->animEndFrame, this->animEndFrame,
                         ANIMMODE_ONCE, 0.0f);
        this->mainTimer = Rand_S16Offset(20, 30);
        this->targetZRot = 0x4000;
        if (Rand_ZeroOne() < 0.5f) {
            this->targetZRot = -0x4000;
        }
        this->targetYRot = this->actor.world.rot.y;
        this->actor.shape.rot.x = this->actor.world.rot.x;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.z = this->actor.world.rot.z;
        this->mainTimer = 1.5 * 20;
        this->actor.speed = Rand_ZeroFloat(0.5f);
        this->actor.world.rot.y = Rand_CenteredFloat(0x8000);
    }
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 10);
    this->state = PR2_STATE_DEAD;
    this->actionFunc = EnPr2_Die;
}

void EnPr2_Die(EnPr2* this, PlayState* play) {
    s32 nearSurface;
    f32 curFrame;
    WaterBox* waterBox;

    SkelAnime_Update(&this->skelAnime);
    if (this->type < PR2_TYPE_BROKEN) {
        s32 i;

        curFrame = this->skelAnime.curFrame;

        if (curFrame >= this->animEndFrame) {
            // spawn LIMB_COUNT EnPr2, one for each limb, to draw floating pieces
            for (i = 0; i < ARRAY_COUNT(this->limbPos); i++) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PR2, this->limbPos[i].x, this->limbPos[i].y,
                            this->limbPos[i].z, this->actor.world.rot.x, this->actor.world.rot.y,
                            this->actor.world.rot.z, i + PR2_TYPE_BROKEN);
            }

            Actor_Kill(&this->actor);
            return;
        }
    } else {
        Vec3f bubblePos;
        nearSurface = false;

        Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 5, 0x2710, 0x3E8);
        Math_SmoothStepToS(&this->actor.shape.rot.z, this->targetZRot, 5, 0x2710, 0x3E8);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->targetYRot, 5, 0x2710, 0x3E8);

        if ((Rand_ZeroOne() < 0.3f) && !this->bubbleToggle) {
            this->bubbleToggle = true;
        }

        if (BgCheck_GetWaterSurfaceNoBgIdAlt(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                             &this->waterSurfaceHeight, &waterBox)) {
            if (this->actor.world.pos.y >= (this->waterSurfaceHeight - 15.0f)) {
                nearSurface = true;
            } else {
                Math_ApproachF(&this->actor.world.pos.y, this->waterSurfaceHeight - 15.0f, 0.3f, 1.0f);
            }
        }

        if ((this->mainTimer == 0) || nearSurface) {
            s32 i;

            Math_SmoothStepToS(&this->alpha, 0, 1, 15, 50);

            if (this->alpha < 2) {
                for (i = 0; i < 10; i++) {
                    Math_Vec3f_Copy(&bubblePos, &this->actor.world.pos);

                    bubblePos.x += Rand_CenteredFloat(20.0f);
                    bubblePos.y += Rand_CenteredFloat(5.0f);
                    bubblePos.z += Rand_CenteredFloat(20.0f);

                    EffectSsBubble_Spawn(play, &bubblePos, 0.0f, 5.0f, 5.0f, Rand_ZeroFloat(0.03f) + 0.07f);
                }

                Actor_Kill(&this->actor);
            }
        }
    }
}

void EnPr2_ApplyDamage(EnPr2* this, PlayState* play) {
    s32 pad;

    if (this->collider.base.acFlags & AC_HIT) {
        Actor_ApplyDamage(&this->actor);

        if ((this->actor.colChkInfo.health <= 0) && (this->state != PR2_STATE_DEAD)) {
            Enemy_StartFinishingBlow(play, &this->actor);
            this->actor.speed = 0.0f;
            Actor_PlaySfx(&this->actor, NA_SE_EN_PIRANHA_DEAD);

            if (this->dropID > -1) {
                Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, sDropTables[this->dropID]);
            }

            this->actor.colChkInfo.health = 0;
            EnPr2_SetupDie(this);
        }
    }

    // against mikau shield?
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->actor.speed = -10.0f;
    }
}

void EnPr2_Update(Actor* thisx, PlayState* play) {
    EnPr2* this = (EnPr2*)thisx;
    f32 rand;

    Actor_SetScale(&this->actor, this->scale);

    this->actionFunc(this, play);

    DECR(this->idleTimer);
    DECR(this->mainTimer);
    DECR(this->targetingTimer);
    DECR(this->returnHomeTimer);

    Actor_SetFocus(&this->actor, 10.0f);
    EnPr2_ApplyDamage(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0, 10.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);

    if (this->bubbleToggle) {
        s32 i;
        Vec3f bubblePos;
        f32 bubbleScale;

        Math_Vec3f_Copy(&bubblePos, &this->limbJawPos);
        this->bubbleToggle = false;

        bubblePos.x += Rand_CenteredFloat(20.0f);
        bubblePos.y += Rand_CenteredFloat(5.0f);
        bubblePos.z += Rand_CenteredFloat(20.0f);

        for (i = 0; i < 2; i++) {
            bubbleScale = Rand_ZeroFloat(0.03f) + 0.07f;
            EffectSsBubble_Spawn(play, &bubblePos, 0, 5.0f, 5.0f, bubbleScale);
        }
    }

    if ((this->alpha == 255) && (this->type < PR2_TYPE_BROKEN)) {
        this->actor.shape.rot.x = this->actor.world.rot.x;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.z = this->actor.world.rot.z;
        if (this->state != PR2_STATE_DEAD) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 EnPr2_OverrideLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPr2* this = (EnPr2*)thisx;

    if (this->type < PR2_TYPE_BROKEN) {
        if (limbIndex == PR_2_LIMB_SKULL) {
            rot->y += TRUNCF_BINANG(this->slowLimbYaw) * -1;
        }
    } else if (this->type != limbIndex + PR2_TYPE_BROKEN) {
        // after death we spawn one actor per floating limb:
        // only draw the one limb we represent, NULL the rest
        *dList = NULL;
    }
    return false;
}

void EnPr2_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnPr2* this = (EnPr2*)thisx;

    if (this->type < PR2_TYPE_BROKEN) {
        if (limbIndex == PR_2_LIMB_SKULL) {
            // why use a matrix to save the limb pos? is there no faster method?
            // also we always store limb pos in the array, why have a secondary copy?
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_MultZero(&this->limbJawPos);
        }

        Matrix_MultZero(&this->limbPos[limbIndex]);
    }
}

s32 EnPr2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                           Gfx** gfx) {
    EnPr2* this = (EnPr2*)thisx;

    if (this->type < PR2_TYPE_BROKEN) {
        if (limbIndex == PR_2_LIMB_SKULL) {
            rot->y += TRUNCF_BINANG(this->slowLimbYaw) * -1;
        }
    } else if ((limbIndex + PR2_TYPE_BROKEN) != this->type) {
        // if this is floating limbs, only draw the one this instance represents
        // IE if we are drawing the floating jaw, then 10 + 2 matches this->type, all other limbs are NULL
        *dList = NULL;
    }
    return false;
}

void EnPr2_Draw(Actor* thisx, PlayState* play) {
    EnPr2* this = (EnPr2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->alpha == 255) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->primColor, this->primColor, this->primColor, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);

        Scene_SetRenderModeXlu(play, 0, 0x1);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnPr2_OverrideLimbDrawOpa, EnPr2_PostLimbDrawOpa, &this->actor);
    } else {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);

        Scene_SetRenderModeXlu(play, 1, 0x2);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnPr2_OverrideLimbDraw, NULL, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
