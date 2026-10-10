/*
 * File: z_en_bomjimb.c
 * Overlay: ovl_En_Bomjimb
 * Description: Bombers when chasing them
 */

#include "z_en_bomjimb.h"
#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBomjimb_Init(Actor* thisx, PlayState* play);
void EnBomjimb_Destroy(Actor* thisx, PlayState* play);
void EnBomjimb_Update(Actor* thisx, PlayState* play2);
void EnBomjimb_Draw(Actor* thisx, PlayState* play);

void func_80C01494(EnBomjimb* this);
void func_80C014E4(EnBomjimb* this, PlayState* play);
void func_80C01984(EnBomjimb* this, PlayState* play);
void func_80C01A24(EnBomjimb* this, PlayState* play);
void func_80C01B40(EnBomjimb* this);
void func_80C01B74(EnBomjimb* this, PlayState* play);
void func_80C01C18(EnBomjimb* this, PlayState* play);
void func_80C01CD0(EnBomjimb* this, PlayState* play);
void func_80C0201C(EnBomjimb* this, PlayState* play);
void func_80C02108(EnBomjimb* this);
void func_80C0217C(EnBomjimb* this, PlayState* play);
void func_80C0250C(EnBomjimb* this);
void func_80C02570(EnBomjimb* this, PlayState* play);
void func_80C0267C(EnBomjimb* this);
void func_80C02704(EnBomjimb* this, PlayState* play);
void func_80C02740(EnBomjimb* this, PlayState* play);
void func_80C02A14(EnBomjimb* this, PlayState* play);
void func_80C02BCC(EnBomjimb* this, PlayState* play);
void func_80C02CA4(EnBomjimb* this, PlayState* play);
void func_80C02DAC(EnBomjimb* this, PlayState* play);

static Actor* D_80C03170 = NULL;

ActorProfile En_Bomjimb_Profile = {
    /**/ ACTOR_EN_BOMJIMB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBomjimb),
    /**/ EnBomjimb_Init,
    /**/ EnBomjimb_Destroy,
    /**/ EnBomjimb_Update,
    /**/ EnBomjimb_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 30, 0, { 0, 0, 0 } },
};

typedef enum {
    /*   -1 */ ENBOMJIMB_ANIM_NONE = -1,
    /* 0x00 */ ENBOMJIMB_ANIM_IDLE,
    /* 0x01 */ ENBOMJIMB_ANIM_HANDS_ON_HIPS,
    /* 0x02 */ ENBOMJIMB_ANIM_TALK_NORMAL,
    /* 0x03 */ ENBOMJIMB_ANIM_TALK_HANDS_ON_HIPS,
    /* 0x04 */ ENBOMJIMB_ANIM_BLOWGUN,
    /* 0x05 */ ENBOMJIMB_ANIM_WALK,
    /* 0x06 */ ENBOMJIMB_ANIM_SURPRISE,
    /* 0x07 */ ENBOMJIMB_ANIM_HIDE_UNDER_BOX,
    /* 0x08 */ ENBOMJIMB_ANIM_DAZED,
    /* 0x09 */ ENBOMJIMB_ANIM_SIT,
    /* 0x0A */ ENBOMJIMB_ANIM_CLIMB,
    /* 0x0B */ ENBOMJIMB_ANIM_HOLD_CUCCO,
    /* 0x0C */ ENBOMJIMB_ANIM_TURN_AROUND,
    /* 0x0D */ ENBOMJIMB_ANIM_SHOW_NUMBER,
    /* 0x0E */ ENBOMJIMB_ANIM_TALK_SHOWING_NUMBER,
    /* 0x0F */ ENBOMJIMB_ANIM_TAUNT,
    /* 0x10 */ ENBOMJIMB_ANIM_JUMP,
    /* 0x11 */ ENBOMJIMB_ANIM_ARM_SWIPE,
    /* 0x12 */ ENBOMJIMB_ANIM_SALUTE,
    /* 0x13 */ ENBOMJIMB_ANIM_RUN,
    /* 0x14 */ ENBOMJIMB_ANIM_LOOK_AROUND,
    /* 0x15 */ ENBOMJIMB_ANIM_CAUGHT,
    /* 0x16 */ ENBOMJIMB_ANIM_MAX
} EnBomjimbAnimation;

static AnimationHeader* sAnimations[ENBOMJIMB_ANIM_MAX] = {
    &gBomberIdleAnim,              // ENBOMJIMB_ANIM_IDLE
    &gBomberHandsOnHipsAnim,       // ENBOMJIMB_ANIM_HANDS_ON_HIPS
    &gBomberTalkNormalAnim,        // ENBOMJIMB_ANIM_TALK_NORMAL
    &gBomberTalkHandsOnHipsAnim,   // ENBOMJIMB_ANIM_TALK_HANDS_ON_HIPS
    &gBomberBlowgunAnim,           // ENBOMJIMB_ANIM_BLOWGUN
    &gBomberWalkAnim,              // ENBOMJIMB_ANIM_WALK
    &gBomberSurpriseAnim,          // ENBOMJIMB_ANIM_SURPRISE
    &gBomberHideUnderBoxAnim,      // ENBOMJIMB_ANIM_HIDE_UNDER_BOX
    &gBomberDazedAnim,             // ENBOMJIMB_ANIM_DAZED
    &gBomberSitAnim,               // ENBOMJIMB_ANIM_SIT
    &gBomberClimbAnim,             // ENBOMJIMB_ANIM_CLIMB
    &gBomberHoldCuccoAnim,         // ENBOMJIMB_ANIM_HOLD_CUCCO
    &gBomberTurnAroundAnim,        // ENBOMJIMB_ANIM_TURN_AROUND
    &gBomberShowNumberAnim,        // ENBOMJIMB_ANIM_SHOW_NUMBER
    &gBomberTalkShowingNumberAnim, // ENBOMJIMB_ANIM_TALK_SHOWING_NUMBER
    &gBomberTauntAnim,             // ENBOMJIMB_ANIM_TAUNT
    &gBomberJumpAnim,              // ENBOMJIMB_ANIM_JUMP
    &gBomberArmSwipeAnim,          // ENBOMJIMB_ANIM_ARM_SWIPE
    &gBomberSaluteAnim,            // ENBOMJIMB_ANIM_SALUTE
    &gBomberRunAnim,               // ENBOMJIMB_ANIM_RUN
    &gBomberLookAroundAnim,        // ENBOMJIMB_ANIM_LOOK_AROUND
    &gBomberCaughtAnim,            // ENBOMJIMB_ANIM_CAUGHT
};

static u8 sAnimationModes[ENBOMJIMB_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_IDLE
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_HANDS_ON_HIPS
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_TALK_NORMAL
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_TALK_HANDS_ON_HIPS
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_BLOWGUN
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_WALK
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_SURPRISE
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_HIDE_UNDER_BOX
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_DAZED
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_SIT
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_CLIMB
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_HOLD_CUCCO
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_TURN_AROUND
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_SHOW_NUMBER
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_TALK_SHOWING_NUMBER
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_TAUNT
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_JUMP
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_ARM_SWIPE
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_SALUTE
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_RUN
    ANIMMODE_LOOP, // ENBOMJIMB_ANIM_LOOK_AROUND
    ANIMMODE_ONCE, // ENBOMJIMB_ANIM_CAUGHT
};

void EnBomjimb_Init(Actor* thisx, PlayState* play) {
    EnBomjimb* this = (EnBomjimb*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    this->actor.gravity = -2.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gBomberSkel, &gBomberIdleAnim, this->jointTable, this->morphTable,
                       OBJECT_CS_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_SetScale(&this->actor, 0.01f);

    this->unk_2C6 = ENBOMJIMB_GET_F0(&this->actor);
    this->unk_2B2 = ENBOMJIMB_GET_FF00(&this->actor);

    if (this->unk_2C6 != ENBOMJIMB_F0_0) {
        this->unk_2C8 = ENBOMJIMB_GET_F(&this->actor) + 1;
    }

    if (this->unk_2C8 < ENBOMJIMB_F_0) {
        this->unk_2C8 = ENBOMJIMB_F_0;
    }

    if (this->unk_2C6 < ENBOMJIMB_F0_0) {
        this->unk_2C6 = ENBOMJIMB_F0_0;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
        switch (this->unk_2C8) {
            case ENBOMJIMB_F_0:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_11_01)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                break;

            case ENBOMJIMB_F_1:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_11_02)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                break;

            case ENBOMJIMB_F_2:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_11_04)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                break;

            case ENBOMJIMB_F_3:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_11_08)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                break;

            case ENBOMJIMB_F_4:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_11_10)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                break;

            default:
                break;
        }
    }

    if ((!CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40)) {
        Actor_Kill(&this->actor);
        return;
    }

    Math_Vec3f_Copy(&this->unk_2A0, &this->actor.home.pos);
    if ((this->unk_2C6 == ENBOMJIMB_F0_0) || (this->unk_2C6 == ENBOMJIMB_F0_1) || (this->unk_2C6 != ENBOMJIMB_F0_2)) {
        func_80C01494(this);
    } else {
        func_80C01984(this, play);
    }
}

void EnBomjimb_Destroy(Actor* thisx, PlayState* play) {
    EnBomjimb* this = (EnBomjimb*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBomjimb_ChangeAnim(EnBomjimb* this, s32 animIndex, f32 playSpeed) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

void func_80C011CC(EnBomjimb* this) {
    if ((this->animIndex == ENBOMJIMB_ANIM_WALK) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMJIMB_ANIM_RUN) &&
        (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMJIMB_ANIM_SALUTE) && Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }

    if ((this->animIndex == ENBOMJIMB_ANIM_HIDE_UNDER_BOX) && Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }
}

void func_80C012E0(EnBomjimb* this) {
    this->unk_2C0 = 0;
    this->unk_2AE = 0;
    this->unk_2B0 = 0;
    this->unk_2CC = 0;
    this->unk_290 = 0;
}

s32 func_80C012FC(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Play_InCsMode(play) && (this->actor.xzDistToPlayer < 40.0f) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 50.0f) && (play->msgCtx.msgLength == 0)) {
        this->actor.speed = 0.0f;
        func_80C02740(this, play);
        return true;
    }
    return false;
}

s32 func_80C013B4(EnBomjimb* this) {
    if (this->collider.base.acFlags & AC_HIT) {
        func_80C0267C(this);
        return true;
    }
    return false;
}

s32 func_80C013F0(EnBomjimb* this, PlayState* play) {
    Actor* sp1C;

    if (this->collider.base.ocFlags1 & OC1_HIT) {
        sp1C = this->collider.base.oc;

        if ((sp1C->id == ACTOR_OBJ_KIBAKO) && (D_80C03170 == NULL) && (sp1C->update != NULL) &&
            !Actor_HasParent(sp1C, play)) {
            D_80C03170 = sp1C;
            this->unk_2E4 = sp1C;
            func_80C01C18(this, play);
            return true;
        }
    }
    return false;
}

void func_80C01494(EnBomjimb* this) {
    func_80C012E0(this);
    Math_Vec3f_Copy(&this->unk_2A0, &this->actor.world.pos);
    this->unk_2CA = 3;
    this->actionFunc = func_80C014E4;
}

void func_80C014E4(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    CollisionPoly* colPoly;
    s16 abs;
    f32 x;
    f32 z;
    f32 phi_f0;
    Vec3f sp60;
    Vec3f sp54;
    Vec3f sp48;
    s32 bgId;

    if (func_80C012FC(this, play) || func_80C013B4(this) || func_80C013F0(this, play)) {
        return;
    }

    switch (this->unk_2CC) {
        case 0:
            if (this->unk_2AE == 0) {
                Math_Vec3f_Copy(&sp48, &this->actor.home.pos);
                sp48.x += Rand_CenteredFloat(150.0f);
                sp48.z += Rand_CenteredFloat(150.0f);

                abs = ABS_ALT(BINANG_SUB(this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp48)));
                if ((abs < 0x4000) && !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp48, &sp60,
                                                               &colPoly, true, false, false, true, &bgId)) {
                    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_WALK, 1.0f);
                    Math_Vec3f_Copy(&this->unk_294, &sp48);
                    this->unk_2B0 = Rand_S16Offset(30, 50);
                    this->unk_2CC++;
                }
            }
            break;

        case 1:
            if (curFrame >= 0.0f) {
                this->unk_2D6 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_294);
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2D6, 10, 0x7D0, 0x14);
            }

            if ((s16)ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->unk_2D6)) < 0x100) {
                Math_Vec3f_Copy(&sp54, &this->actor.world.pos);
                sp54.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
                sp54.z += Math_CosS(this->actor.world.rot.y) * 60.0f;
                if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp54, &sp60, &colPoly, true, false,
                                            false, true, &bgId)) {
                    this->unk_2AE = 0;
                    if (Rand_ZeroOne() < 0.5f) {
                        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_LOOK_AROUND, 1.0f);
                    } else {
                        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_IDLE, 1.0f);
                    }
                    this->unk_2CC = 0;
                    this->unk_2B4 = 0.0f;
                    break;
                }
            }

            x = this->unk_294.x - this->actor.world.pos.x;
            z = this->unk_294.z - this->actor.world.pos.z;

            if ((this->unk_2B0 == 0) || (sqrtf(SQ(x) + SQ(z)) < 4.0f)) {
                this->unk_2AE = Rand_S16Offset(20, 20);
                if (!(this->unk_2AE & 1)) {
                    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_LOOK_AROUND, 1.0f);
                } else {
                    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_IDLE, 1.0f);
                }
                this->unk_2CC = 0;
                this->unk_2B4 = 0.0f;
            } else if (curFrame >= 0.0f) {
                Math_ApproachF(&this->actor.world.pos.x, this->unk_294.x, 0.3f, this->unk_2B4);
                Math_ApproachF(&this->actor.world.pos.z, this->unk_294.z, 0.3f, this->unk_2B4);
                Math_ApproachF(&this->unk_2B4, 1.0f, 0.3f, 0.5f);
            }
            break;

        default:
            break;
    }

    if (player->stateFlags3 != PLAYER_STATE3_1000000) {
        phi_f0 = 200.0f;

        abs = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));
        if (abs > 0x2890) {
            phi_f0 = 150.0f;
        }

        if (this->actor.xzDistToPlayer < phi_f0) {
            func_80C02108(this);
        }
    }
}

void func_80C01984(EnBomjimb* this, PlayState* play) {
    func_80C012E0(this);
    this->unk_2E4 =
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x, this->actor.world.pos.y + 30.0f,
                    this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, 2);
    if (this->unk_2E4 != NULL) {
        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_HOLD_CUCCO, 1.0f);
    }
    this->unk_2CA = 0;
    this->actionFunc = func_80C01A24;
}

void func_80C01A24(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.gravity = -0.1f;
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if ((this->unk_2E4 != NULL) && (this->unk_2E4->update != NULL)) {
            ((EnNiw*)this->unk_2E4)->unk2BC.z = 90000.0f;
        }
        this->actor.speed = 0.0f;
        this->unk_2E4 = NULL;
        this->actor.gravity = -2.0f;
        func_80C02108(this);
        return;
    }

    if ((this->actor.xzDistToPlayer < 200.0f) && (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)) {
        this->unk_2C0 = 1;
    }

    if (this->unk_2C0 != 0) {
        Math_ApproachF(&this->actor.speed, 6.0f, 0.5f, 2.0f);
    }

    if ((this->unk_2C0 != 0) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        func_80C01B40(this);
    }
}

void func_80C01B40(EnBomjimb* this) {
    func_80C012E0(this);
    this->unk_2CA = 0;
    this->actionFunc = func_80C01B74;
}

void func_80C01B74(EnBomjimb* this, PlayState* play) {
    Math_ApproachF(&this->actor.speed, 6.0f, 0.5f, 2.0f);
    if ((this->collider.base.acFlags & AC_HIT) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if ((this->unk_2E4 != NULL) && (this->unk_2E4->update != NULL)) {
            ((EnNiw*)this->unk_2E4)->unk2BC.z = 90000.0f;
        }
        this->actor.speed = 0.0f;
        this->unk_2E4 = NULL;
        this->actor.gravity = -2.0f;
        func_80C02108(this);
    }
}

void func_80C01C18(EnBomjimb* this, PlayState* play) {
    func_80C012E0(this);
    if (this->unk_2E4 != NULL) {
        if (this->unk_2E4->update != NULL) {
            this->unk_2E4->velocity.y = 8.0f;
            this->unk_2E4->shape.rot.z = BINANG_SUB(4000, ((s16)(play->gameplayFrames & 2) * 4000));
            this->unk_294.x = this->unk_2E4->world.pos.x;
            this->unk_294.y = this->actor.world.pos.y;
            this->unk_294.z = this->unk_2E4->world.pos.z;
        }
    }
    this->actor.speed = 0.0f;
    this->unk_2CA = 2;
    this->actionFunc = func_80C01CD0;
}

void func_80C01CD0(EnBomjimb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 pad[2];

    if ((this->unk_2E4 != NULL) && ((this->unk_2E4->update == NULL) || Actor_HasParent(this->unk_2E4, play))) {
        this->actor.draw = EnBomjimb_Draw;
        D_80C03170 = NULL;
        this->unk_2E4 = NULL;
        func_80C02108(this);
        return;
    }

    if (this->animIndex != ENBOMJIMB_ANIM_HIDE_UNDER_BOX) {
        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_294), 1, 0xBB8,
                           0);
        Math_ApproachF(&this->actor.world.pos.x, this->unk_294.x, 0.3f, 2.0f);
        Math_ApproachF(&this->actor.world.pos.z, this->unk_294.z, 0.3f, 2.0f);
        if (sqrtf(SQ(this->actor.world.pos.x - this->unk_294.x) + SQ(this->actor.world.pos.z - this->unk_294.z)) <
            3.0f) {
            EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_HIDE_UNDER_BOX, 1.0f);
            Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_294);
        }
    } else if (curFrame >= this->animEndFrame) {
        this->actor.draw = NULL;
    }

    if ((this->unk_2C0 == 0) && (this->unk_2E4->bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_PUT_DOWN_WOODBOX);
        this->unk_2C0 = 1;
    }

    if (this->unk_2AE == 0) {
        if (this->actor.draw == NULL) {
            this->unk_2AE = Rand_S16Offset(20, 30);
            this->unk_2E4->shape.rot.z = 1000;
            return;
        }

        this->unk_2AE = 10;
        if ((this->unk_2E4 != NULL) && (this->unk_2E4->update != NULL)) {
            if ((play->gameplayFrames % 16) == 0) {
                this->unk_2E4->shape.rot.z = -this->unk_2E4->shape.rot.z;
                this->unk_2E4->shape.rot.z *= 0.1f;
            }

            if (fabsf(this->unk_2E4->shape.rot.z) < 100.0f) {
                this->unk_2E4->shape.rot.z = 0;
            }
        }
        return;
    }

    if (this->actor.draw == NULL) {
        this->unk_2E4->shape.rot.z = -this->unk_2E4->shape.rot.z;
        this->unk_2E4->shape.rot.z *= 0.1f;
        if (fabsf(this->unk_2E4->shape.rot.z) < 100.0f) {
            this->unk_2E4->shape.rot.z = 0;
        }
    }
}

void func_80C01FD4(EnBomjimb* this) {
    this->actor.textId = 0x72D;
    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_SIT, 1.0f);
    this->unk_2CA = 4;
    this->actionFunc = func_80C0201C;
}

void func_80C0201C(EnBomjimb* this, PlayState* play) {
    s16 abs = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));

    this->unk_290 = 0;
    if ((this->actor.xzDistToPlayer < 200.0f) && (abs < 0x61A8)) {
        this->unk_290 = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (this->unk_290 > 10000) {
            this->unk_290 = 10000;
        } else if (this->unk_290 < -10000) {
            this->unk_290 = -10000;
        }
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2CA = 10;
        this->actionFunc = func_80C02A14;
    } else {
        Actor_OfferTalk(&this->actor, play, 40.0f);
    }
}

void func_80C02108(EnBomjimb* this) {
    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_SURPRISE, 1.0f);
    this->unk_2D6 = BINANG_ROT180(this->actor.yawTowardsPlayer);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    func_80C012E0(this);
    Math_Vec3f_Copy(&this->unk_2A0, &this->actor.world.pos);
    this->unk_2D4 = 0;
    this->unk_2CA = 5;
    this->actionFunc = func_80C0217C;
}

void func_80C0217C(EnBomjimb* this, PlayState* play) {
    Vec3f sp74;
    CollisionPoly* sp70;
    Vec3f sp64;
    s32 bgId;
    s32 floorBgId = this->actor.floorBgId;
    CollisionPoly* sp58 = this->actor.floorPoly;
    Player* player = GET_PLAYER(play);
    s32 sp50 = false;

    if (func_80C012FC(this, play) || func_80C013B4(this) || func_80C013F0(this, play)) {
        return;
    }

    if (player->stateFlags3 == PLAYER_STATE3_1000000) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
        func_80C01494(this);
        return;
    }

    if (this->animIndex == ENBOMJIMB_ANIM_SURPRISE) {
        f32 curFrame = this->skelAnime.curFrame;

        if (curFrame >= this->animEndFrame) {
            EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_RUN, 2.0f);
        }
        return;
    }

    Math_ApproachF(&this->actor.speed, 8.0f, 0.5f, 2.0f);
    Math_Vec3f_Copy(&sp74, &this->actor.world.pos);

    sp74.x += Math_SinS(this->actor.world.rot.y) * 50.0f;
    sp74.y += 20.0f;
    sp74.z += Math_CosS(this->actor.world.rot.y) * 50.0f;

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp74, &sp64, &sp70, true, false, false, true,
                                &bgId)) {
        s16 temp = BINANG_SUB((this->actor.world.rot.y - this->actor.yawTowardsPlayer), 0x8000);
        this->unk_2D6 = temp;

        if (temp < 0) {
            this->unk_2D6 += BINANG_ADD((s32)Rand_ZeroFloat(0x200), 0x4000);
            if (this->unk_2D0 != 0) {
                this->unk_2D6 += BINANG_ADD((s32)Rand_ZeroFloat(0x200), 0x4000);
            }
            this->unk_2D0 = 1;
        } else {
            this->unk_2D6 -= BINANG_ADD((s32)Rand_ZeroFloat(0x200), 0x4000);
            if (this->unk_2D0 != 0) {
                this->unk_2D6 -= BINANG_ADD((s32)Rand_ZeroFloat(0x200), 0x4000);
            }
            this->unk_2D0 = 1;
        }
        sp50 = true;
    }

    if (!sp50) {
        this->unk_2D0 = 0;
        if (this->actor.xzDistToPlayer < 60.0f) {
            this->unk_2D6 = BINANG_ROT180(this->actor.yawTowardsPlayer);
        }
    }

    this->actor.world.rot.y = this->unk_2D6 + this->unk_2D4;

    if (SurfaceType_GetSceneExitIndex(&play->colCtx, sp58, floorBgId) != 0) {
        s16 temp = BINANG_SUB(this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x8000);

        if (temp < 0) {
            this->unk_2D4 -= 0x1000;
        } else {
            this->unk_2D4 += 0x1000;
        }
        this->actor.world.rot.y = this->unk_2D6 + this->unk_2D4;
        return;
    }

    this->unk_2D4 = 0;
    if (!sp50 && (this->actor.xzDistToPlayer > 200.0f)) {
        func_80C0250C(this);
    }
}

void func_80C0250C(EnBomjimb* this) {
    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_TAUNT, 1.0f);
    this->unk_2D4 = 0;
    this->actor.speed = 0.0f;
    this->unk_2D6 = BINANG_ROT180(this->actor.yawTowardsPlayer);
    func_80C012E0(this);
    this->unk_2CA = 6;
    this->actionFunc = func_80C02570;
}

void func_80C02570(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (func_80C012FC(this, play) || func_80C013B4(this) || func_80C013F0(this, play)) {
        return;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x1388, 0);

    if (this->actor.xzDistToPlayer < 200.0f) {
        this->unk_2D6 = BINANG_ROT180(this->actor.yawTowardsPlayer);
        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_RUN, 2.0f);
        this->actionFunc = func_80C0217C;
    } else if ((player->stateFlags3 == PLAYER_STATE3_1000000) || (this->actor.xzDistToPlayer > 410.0f)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
        func_80C01494(this);
    }
}

void func_80C0267C(EnBomjimb* this) {
    func_80C012E0(this);
    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_DAZED, 1.0f);
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->unk_2AE = 40;
    this->unk_2C2 = 0;
    this->unk_2C4 = (s32)Rand_ZeroFloat(60.0f) + 20;
    this->unk_2CA = 7;
    this->actionFunc = func_80C02704;
}

void func_80C02704(EnBomjimb* this, PlayState* play) {
    if (!func_80C012FC(this, play) && (this->unk_2AE == 0)) {
        func_80C0250C(this);
    }
}

u16 D_80C03230[] = {
    0x0721, 0x0722, 0x0723, 0x0724, 0x072C,
};
void func_80C02740(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80C012E0(this);
    EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_CAUGHT, 1.0f);
    if ((player->transformation != PLAYER_FORM_DEKU) && (player->transformation != PLAYER_FORM_HUMAN)) {
        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_ARM_SWIPE, 1.0f);
        Message_StartTextbox(play, 0x72E, &this->actor);
        player->stateFlags1 |= PLAYER_STATE1_10000000;
        player->actor.freezeTimer = 3;
        func_80C012E0(this);
        this->unk_2CA = 9;
        this->actionFunc = func_80C02BCC;
        return;
    }

    if (((player->transformation == PLAYER_FORM_DEKU) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10)) ||
        ((player->transformation == PLAYER_FORM_HUMAN) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02))) {
        EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_ARM_SWIPE, 1.0f);
        Message_StartTextbox(play, 0x72E, &this->actor);
        player->stateFlags1 |= PLAYER_STATE1_10000000;
        player->actor.freezeTimer = 3;
        func_80C012E0(this);
        this->unk_2CA = 9;
        this->actionFunc = func_80C02BCC;
        return;
    }

    Message_StartTextbox(play, D_80C03230[((void)0, gSaveContext.save.saveInfo.bombersCaughtNum)], &this->actor);
    gSaveContext.save.saveInfo.bombersCaughtOrder[((void)0, gSaveContext.save.saveInfo.bombersCaughtNum)] =
        this->unk_2C8 + 1;
    gSaveContext.save.saveInfo.bombersCaughtNum++;

    if (gSaveContext.save.saveInfo.bombersCaughtNum > 4) {
        // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to do
        // anything bad.
        Audio_PlayFanfare(NA_BGM_GET_ITEM);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_SY_PIECE_OF_HEART);
    }

    switch (this->unk_2C8) {
        case ENBOMJIMB_F_0:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_01);
            SET_WEEKEVENTREG(WEEKEVENTREG_11_01);
            break;

        case ENBOMJIMB_F_1:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_02);
            SET_WEEKEVENTREG(WEEKEVENTREG_11_02);
            break;

        case ENBOMJIMB_F_2:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_04);
            SET_WEEKEVENTREG(WEEKEVENTREG_11_04);
            break;

        case ENBOMJIMB_F_3:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_11_08);
            break;

        case ENBOMJIMB_F_4:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_10);
            SET_WEEKEVENTREG(WEEKEVENTREG_11_10);
            break;

        default:
            break;
    }

    if (!Play_InCsMode(play)) {
        Player* player = GET_PLAYER(play);

        player->stateFlags1 |= PLAYER_STATE1_10000000;
        player->actor.freezeTimer = 3;
    }
    this->unk_2CA = 8;
    this->actionFunc = func_80C02A14;
}

void func_80C02A14(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    if (this->animIndex == ENBOMJIMB_ANIM_CAUGHT) {
        player->actor.freezeTimer = 3;
        if (curFrame >= this->animEndFrame) {
            EnBomjimb_ChangeAnim(this, ENBOMJIMB_ANIM_SIT, 1.0f);
        }
        return;
    }

    if ((this->unk_2CA == 8) && (this->animIndex == ENBOMJIMB_ANIM_SIT)) {
        player->actor.freezeTimer = 3;
        if (this->unk_2E0 == 0) {
            if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_HUMAN_BOUND);
                this->unk_2E0 = 1;
            }
        }
    }

    if ((this->animIndex == ENBOMJIMB_ANIM_TAUNT) && (this->unk_2CA == 8)) {
        player->actor.freezeTimer = 3;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if ((this->unk_2CA == 8) && (gSaveContext.save.saveInfo.bombersCaughtNum >= 5)) {
            func_80C02CA4(this, play);
        } else {
            if (this->unk_2CA == 8) {
                player->stateFlags1 &= ~PLAYER_STATE1_10000000;
            }
            func_80C01FD4(this);
        }
    }
}

void func_80C02BCC(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x1388, 0);
    if (this->unk_2C0 == 0) {
        player->actor.freezeTimer = 3;
        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            Message_CloseTextbox(play);
            this->unk_2C0 = 1;
            player->stateFlags1 &= ~PLAYER_STATE1_10000000;
        }
    } else if (this->actor.xzDistToPlayer > 200.0f) {
        func_80C01494(this);
    }
}

void func_80C02CA4(EnBomjimb* this, PlayState* play) {
    if (BREG(13) == 0) {
        play->nextEntrance = play->setupExitList[this->unk_2B2];
        gSaveContext.nextCutsceneIndex = 0;
        Scene_SetExitFade(play);
        play->transitionTrigger = TRANS_TRIGGER_START;
    } else {
        play->nextEntrance = Entrance_CreateFromSpawn(5);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_86;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    }
    SET_WEEKEVENTREG(WEEKEVENTREG_75_40);
    SET_WEEKEVENTREG(WEEKEVENTREG_83_04);
    this->actionFunc = func_80C02DAC;
}

void func_80C02DAC(EnBomjimb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 3;
}

void EnBomjimb_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBomjimb* this = (EnBomjimb*)thisx;

    if (this->unk_2B0 != 0) {
        this->unk_2B0--;
    }

    if (this->unk_2AE != 0) {
        this->unk_2AE--;
    }

    if (this->unk_2AC != 0) {
        this->unk_2AC--;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    func_80C011CC(this);
    Actor_SetFocus(&this->actor, 20.0f);

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);

    if (this->unk_2CA == 0) {
        if ((this->unk_2E4 != NULL) && (this->unk_2E4->update != NULL)) {
            Math_Vec3f_Copy(&this->unk_2E4->world.pos, &this->actor.world.pos);
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->unk_2E4->world.pos.y = this->actor.world.pos.y + 35.0f;
            } else {
                this->unk_2E4->world.pos.y = this->actor.world.pos.y + 25.0f;
            }
        }
    }

    Math_SmoothStepToS(&this->unk_28A, this->unk_290, 1, 0x1388, 0);

    if (this->unk_2C4 == 0) {
        this->unk_2C2++;
        if (this->unk_2C2 > 2) {
            this->unk_2C2 = 0;
            this->unk_2C4 = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    if (this->unk_2CA != 2) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnBomjimb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBomjimb* this = (EnBomjimb*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_0F) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->unk_28A;
        rot->z += this->unk_288;
    }

    if ((limbIndex == OBJECT_CS_LIMB_13) && (this->unk_2C8 != ENBOMJIMB_F_0)) {
        *dList = NULL;
    }

    if ((limbIndex == OBJECT_CS_LIMB_14) && (this->unk_2C8 == ENBOMJIMB_F_0)) {
        *dList = NULL;
    }

    return false;
}

#include "overlays/ovl_En_Bomjimb/ovl_En_Bomjimb.h"

void EnBomjimb_Draw(Actor* thisx, PlayState* play) {
    static Gfx* D_80C03260[] = {
        gBomberCatchRedBandanaDL,  gBomberCatchBlueBandanaDL, gBomberCatchBlueBandanaDL,
        gBomberCatchBlueBandanaDL, gBomberCatchBlueBandanaDL,
    };
    static TexturePtr D_80C03274[] = {
        gBomberEyeOpenTex,
        gBomberEyeHalfTex,
        gBomberEyeClosedTex,
    };
    static TexturePtr D_80C03280[] = {
        gBomberNumber1Tex, gBomberNumber2Tex, gBomberNumber3Tex, gBomberNumber4Tex, gBomberNumber5Tex,
    };
    EnBomjimb* this = (EnBomjimb*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80C03274[this->unk_2C2]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C03280[this->unk_2C8]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(D_80C03260[this->unk_2C8]));

    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBomjimb_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
