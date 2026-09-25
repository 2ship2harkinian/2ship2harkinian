/*
 * File: z_en_bombers.c
 * Overlay: ovl_En_Bombers
 * Description: Bombers - Blue-Hatted Bomber
 */

#include "z_en_bombers.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBombers_Init(Actor* thisx, PlayState* play);
void EnBombers_Destroy(Actor* thisx, PlayState* play);
void EnBombers_Update(Actor* thisx, PlayState* play);
void EnBombers_Draw(Actor* thisx, PlayState* play);

void EnBombers_SetupNpcBehavior(EnBombers* this);
void EnBombers_NpcBehavior(EnBombers* this, PlayState* play);
void EnBombers_StartTalking(EnBombers* this);
void EnBombers_UpdateTalk(EnBombers* this, PlayState* play);
void EnBombers_SetupSitting(EnBombers* this);
void EnBombers_Sitting(EnBombers* this, PlayState* play);
void EnBombers_TalkWhileSitting(EnBombers* this, PlayState* play);

ActorProfile En_Bombers_Profile = {
    /**/ ACTOR_EN_BOMBERS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBombers),
    /**/ EnBombers_Init,
    /**/ EnBombers_Destroy,
    /**/ EnBombers_Update,
    /**/ EnBombers_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

typedef enum {
    /* 0 */ ENBOMBERS_NPC_WALKING,
    /* 1 */ ENBOMBERS_NPC_STANDING
} EnBombersNpcState;

typedef enum {
    /*   -1 */ ENBOMBERS_ANIM_NONE = -1,
    /* 0x00 */ ENBOMBERS_ANIM_IDLE,
    /* 0x01 */ ENBOMBERS_ANIM_TALK_HANDS_ON_HIPS,
    /* 0x02 */ ENBOMBERS_ANIM_WALK,
    /* 0x03 */ ENBOMBERS_ANIM_SURPRISE,
    /* 0x04 */ ENBOMBERS_ANIM_HIDE_UNDER_BOX,
    /* 0x05 */ ENBOMBERS_ANIM_DAZED,
    /* 0x06 */ ENBOMBERS_ANIM_SIT,
    /* 0x07 */ ENBOMBERS_ANIM_HOLD_CUCCO,
    /* 0x08 */ ENBOMBERS_ANIM_TURN_AROUND,
    /* 0x09 */ ENBOMBERS_ANIM_SHOW_NUMBER,
    /* 0x0A */ ENBOMBERS_ANIM_TALK_SHOWING_NUMBER,
    /* 0x0B */ ENBOMBERS_ANIM_TAUNT,
    /* 0x0C */ ENBOMBERS_ANIM_JUMP,
    /* 0x0D */ ENBOMBERS_ANIM_ARM_SWIPE,
    /* 0x0E */ ENBOMBERS_ANIM_SALUTE,
    /* 0x0F */ ENBOMBERS_ANIM_RUN,
    /* 0x10 */ ENBOMBERS_ANIM_LOOK_AROUND,
    /* 0x11 */ ENBOMBERS_ANIM_MAX
} EnBombersAnimation;

static AnimationHeader* sAnimations[ENBOMBERS_ANIM_MAX] = {
    &gBomberIdleAnim,              // ENBOMBERS_ANIM_IDLE
    &gBomberTalkHandsOnHipsAnim,   // ENBOMBERS_ANIM_TALK_HANDS_ON_HIPS
    &gBomberWalkAnim,              // ENBOMBERS_ANIM_WALK
    &gBomberSurpriseAnim,          // ENBOMBERS_ANIM_SURPRISE
    &gBomberHideUnderBoxAnim,      // ENBOMBERS_ANIM_HIDE_UNDER_BOX
    &gBomberDazedAnim,             // ENBOMBERS_ANIM_DAZED
    &gBomberSitAnim,               // ENBOMBERS_ANIM_SIT
    &gBomberHoldCuccoAnim,         // ENBOMBERS_ANIM_HOLD_CUCCO
    &gBomberTurnAroundAnim,        // ENBOMBERS_ANIM_TURN_AROUND
    &gBomberShowNumberAnim,        // ENBOMBERS_ANIM_SHOW_NUMBER
    &gBomberTalkShowingNumberAnim, // ENBOMBERS_ANIM_TALK_SHOWING_NUMBER
    &gBomberTauntAnim,             // ENBOMBERS_ANIM_TAUNT
    &gBomberJumpAnim,              // ENBOMBERS_ANIM_JUMP
    &gBomberArmSwipeAnim,          // ENBOMBERS_ANIM_ARM_SWIPE
    &gBomberSaluteAnim,            // ENBOMBERS_ANIM_SALUTE
    &gBomberRunAnim,               // ENBOMBERS_ANIM_RUN
    &gBomberLookAroundAnim,        // ENBOMBERS_ANIM_LOOK_AROUND
};

static u8 sAnimationModes[ENBOMBERS_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_IDLE
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_TALK_HANDS_ON_HIPS
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_WALK
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_SURPRISE
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_HIDE_UNDER_BOX
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_DAZED
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_SIT
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_HOLD_CUCCO
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_TURN_AROUND
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_SHOW_NUMBER
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_TALK_SHOWING_NUMBER
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_TAUNT
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_JUMP
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_ARM_SWIPE
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_SALUTE
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_RUN
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_LOOK_AROUND
};

#include "overlays/ovl_En_Bombers/ovl_En_Bombers.h"

static Gfx* sBomberDisplayLists[] = {
    gBomberRedBandanaDL, gBomberBlueBandanaDL, gBomberBlueBandanaDL, gBomberBlueBandanaDL, gBomberBlueBandanaDL,
};

static TexturePtr sEyeTextures[] = {
    gBomberEyeOpenTex,
    gBomberEyeHalfTex,
    gBomberEyeClosedTex,
};

static TexturePtr sNumberTextures[] = {
    gBomberNumber1Tex, gBomberNumber2Tex, gBomberNumber3Tex, gBomberNumber4Tex, gBomberNumber5Tex,
};

void EnBombers_Init(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    this->actor.gravity = -1.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gBomberSkel, &gBomberIdleAnim, this->jointTable, this->morphTable,
                       OBJECT_CS_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    Actor_SetScale(&this->actor, 0.01f);

    this->type = ENBOMBERS_GET_TYPE(&this->actor);
    this->number = ENBOMBERS_GET_NUMBER(&this->actor);

    if (this->type == ENBOMBERS_TYPE_NPC) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
            Actor_Kill(&this->actor);
            return;
        }

        this->number++;
        EnBombers_SetupNpcBehavior(this);
    } else if ((CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) &&
               (((this->number == ENBOMBERS_NO_1) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_01)) ||
                ((this->number == ENBOMBERS_NO_2) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_02)) ||
                ((this->number == ENBOMBERS_NO_3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_04)) ||
                ((this->number == ENBOMBERS_NO_4) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_08)) ||
                ((this->number == ENBOMBERS_NO_5) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_10)))) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40)) {
            if (this->number == ENBOMBERS_NO_1) {
                EnBomBowlMan* bomBowlMan = (EnBomBowlMan*)Actor_Spawn(
                    &play->actorCtx, play, ACTOR_EN_BOM_BOWL_MAN, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, 0);

                if (bomBowlMan != NULL) {
                    s32 csId = this->actor.csId;
                    s32 i = 0;

                    // clang-format off
                    while (csId != CS_ID_NONE) { bomBowlMan->csIdList[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId); i++; }
                    // clang-format on

                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_01);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_02);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_04);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_08);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_10);
                }
            }
            Actor_Kill(&this->actor);
            return;
        }

        EnBombers_SetupSitting(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnBombers_Destroy(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBombers_ChangeAnim(EnBombers* this, s32 animIndex, f32 playSpeed) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void EnBombers_PlaySfx(EnBombers* this) {
    if ((this->animIndex == ENBOMBERS_ANIM_WALK) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMBERS_ANIM_RUN) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 2.0f) ||
         Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }
}

void EnBombers_UpdateRegularTextId(EnBombers* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    this->talkState = TEXT_STATE_EVENT;

    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            this->actor.textId = 0x73D;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_80)) {
                this->actor.textId = 0x74B;
            }
            break;

        case PLAYER_FORM_GORON:
            this->actor.textId = 0x73E;
            break;

        case PLAYER_FORM_ZORA:
            this->actor.textId = 0x73F;
            break;

        case PLAYER_FORM_DEKU:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
                this->actor.textId = 0x75A;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEKU_LEARNED_WHERE_BOMBER_JIM_IS)) {
                this->actor.textId = 0x749;
                if (((this->number == ENBOMBERS_NO_2) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_2)) ||
                    ((this->number == ENBOMBERS_NO_3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_3)) ||
                    ((this->number == ENBOMBERS_NO_4) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_4)) ||
                    ((this->number == ENBOMBERS_NO_5) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_5))) {
                    this->actor.textId = 0x74A;
                }
            } else {
                this->actor.textId = 0x744;
            }
            break;
        // #region 2S2H - Enhancements. Fierce Deity gets same dialog as Human without notebook
        case PLAYER_FORM_FIERCE_DEITY:
            this->actor.textId = 0x73D;
            break;
        // #endregion
        default:
            break;
    }
}

void EnBombers_SetupNpcBehavior(EnBombers* this) {
    this->collider.dim.radius = 10;
    this->collider.dim.height = 30;
    this->action = 0;
    this->actionFunc = EnBombers_NpcBehavior;
}

void EnBombers_NpcBehavior(EnBombers* this, PlayState* play) {
    CollisionPoly* colPoly;
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f endPoint;
    Vec3f walkTarget;
    Vec3f walkVec;
    f32 x;
    f32 z;
    s32 bgId;
    s16 abs;

    switch (this->npcState) {
        case ENBOMBERS_NPC_WALKING:
            if (this->idleTimer == 0) {
                Math_Vec3f_Copy(&walkTarget, &this->actor.home.pos);

                walkTarget.x += Rand_CenteredFloat(150.0f);
                walkTarget.z += Rand_CenteredFloat(150.0f);

                abs = ABS_ALT(BINANG_SUB(this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &walkTarget)));
                if ((abs < 0x4000) && !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &walkTarget,
                                                               &endPoint, &colPoly, true, false, false, true, &bgId)) {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_WALK, 1.0f);
                    Math_Vec3f_Copy(&this->walkTarget, &walkTarget);
                    this->walkTimer = Rand_S16Offset(30, 50);
                    this->npcState++;
                }
            }
            break;

        case ENBOMBERS_NPC_STANDING:
            if (curFrame >= 0.0f) {
                this->targetYaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->walkTarget);
                Math_SmoothStepToS(&this->actor.world.rot.y, this->targetYaw, 0xA, 0x7D0, 0x14);
            }

            if ((s16)ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->targetYaw)) < 0x100) {
                Math_Vec3f_Copy(&walkVec, &this->actor.world.pos);
                walkVec.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
                walkVec.z += Math_CosS(this->actor.world.rot.y) * 60.0f;

                if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &walkVec, &endPoint, &colPoly, true,
                                            false, false, true, &bgId)) {
                    this->idleTimer = 0;
                    if (Rand_ZeroOne() < 0.5f) {
                        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_LOOK_AROUND, 1.0f);
                    } else {
                        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_IDLE, 1.0f);
                    }
                    this->npcState = ENBOMBERS_NPC_WALKING;
                    this->walkMaxStep = 0.0f;
                    break;
                }
            }

            x = this->walkTarget.x - this->actor.world.pos.x;
            z = this->walkTarget.z - this->actor.world.pos.z;

            if ((this->walkTimer == 0) || (sqrtf(SQ(x) + SQ(z)) < 4.0f)) {
                this->idleTimer = Rand_S16Offset(20, 20);
                if (!(this->idleTimer & 1)) {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_LOOK_AROUND, 1.0f);
                } else {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_IDLE, 1.0f);
                }
                this->npcState = 0;
                this->walkMaxStep = 0.0f;
            } else if (curFrame >= 0.0f) {
                Math_ApproachF(&this->actor.world.pos.x, this->walkTarget.x, 0.3f, this->walkMaxStep);
                Math_ApproachF(&this->actor.world.pos.z, this->walkTarget.z, 0.3f, this->walkMaxStep);
                Math_ApproachF(&this->walkMaxStep, 1.0f, 0.3f, 0.5f);
            }
            break;

        default:
            break;
    }

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS) == 0) {
        EnBombers_UpdateRegularTextId(this, play);
    } else {
        this->talkState = TEXT_STATE_EVENT;
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->targetYaw = this->actor.yawTowardsPlayer;
        this->collider.dim.radius = 20;
        this->collider.dim.height = 60;
        EnBombers_StartTalking(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void EnBombers_StartTalking(EnBombers* this) {
    this->targetYaw = this->actor.yawTowardsPlayer;
    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_TALK_HANDS_ON_HIPS, 1.0f);
    this->action = 1;
    this->actionFunc = EnBombers_UpdateTalk;
}

void EnBombers_UpdateTalk(EnBombers* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 hasMoreText;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->targetYaw, 1, 0x1388, 0);

    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            this->headRotTargetZ = -0xFA0;
            break;

        case PLAYER_FORM_GORON:
        case PLAYER_FORM_ZORA:
            this->headRotTargetZ = -0x1770;
            break;

        default:
            break;
    }

    if ((this->talkState == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        hasMoreText = false;
        Message_CloseTextbox(play);
        this->talkState = TEXT_STATE_EVENT;

        if ((this->actor.textId == 0x73D) || (this->actor.textId == 0x73E) || (this->actor.textId == 0x73F)) {
            this->actor.textId = 0x740;
            this->talkState = TEXT_STATE_CHOICE;
            hasMoreText = true;
        } else if (this->actor.textId == 0x740) {
            if (play->msgCtx.choiceIndex == 0) {
                Audio_PlaySfx_MessageDecide();
                this->actor.textId = 0x742;
                hasMoreText = true;
            } else {
                Audio_PlaySfx_MessageCancel();
                this->actor.textId = 0x741;
                hasMoreText = true;
            }
        } else if (this->actor.textId == 0x742) {
            this->actor.textId = 0x737;
            hasMoreText = true;
        } else if (this->actor.textId == 0x737) {
            this->actor.textId = 0x743;
            hasMoreText = true;
        } else if (this->actor.textId == 0x74B) {
            this->actor.textId = 0x74C;
            this->talkState = TEXT_STATE_CHOICE;
            hasMoreText = true;
        } else if (this->actor.textId == 0x74C) {
            if (play->msgCtx.choiceIndex == 1) {
                Audio_PlaySfx_MessageCancel();
                this->actor.textId = 0x737;
                hasMoreText = true;
            } else {
                Audio_PlaySfx_MessageDecide();
                this->actor.textId = 0x74D;
                EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_SALUTE, 1.0f);
                hasMoreText = true;
            }
        } else if (this->actor.textId == 0x744) {
            s32 day = gSaveContext.save.day - 1;

            if (day == 2) {
                this->actor.textId = 0x746;
                hasMoreText = true;
            } else {
                this->actor.textId = 0x745;
                hasMoreText = true;
            }
        } else if ((this->actor.textId == 0x745) || (this->actor.textId == 0x746)) {
            this->actor.textId = 0x747;
            hasMoreText = true;
        } else if (this->actor.textId == 0x747) {
            this->actor.textId = 0x748;
            hasMoreText = true;
        } else if (this->actor.textId == 0x748) {
            switch (this->number) {
                case ENBOMBERS_NO_2:
                    SET_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_2);
                    break;

                case ENBOMBERS_NO_3:
                    SET_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_3);
                    break;

                case ENBOMBERS_NO_4:
                    SET_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_4);
                    break;

                case ENBOMBERS_NO_5:
                    SET_WEEKEVENTREG(WEEKEVENTREG_DEKU_TALKED_TO_BOMBER_5);
                    break;

                default:
                    break;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_DEKU_LEARNED_WHERE_BOMBER_JIM_IS);
        }

        switch (hasMoreText) {
            case false:
                this->headRotTargetZ = 0;
                EnBombers_SetupNpcBehavior(this);
                break;

            case true:
                Message_ContinueTextbox(play, this->actor.textId);
                break;

            default:
                break;
        }
    }
}

void EnBombers_SetupSitting(EnBombers* this) {
    if (this->animIndex != ENBOMBERS_ANIM_SIT) {
        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_SIT, 1.0f);
    }
    this->actor.textId = 0x72D;
    this->headRotTargetZ = 0x1F40;
    this->action = 2;
    this->actionFunc = EnBombers_Sitting;
}

void EnBombers_Sitting(EnBombers* this, PlayState* play) {
    Math_SmoothStepToS(&this->headRotZ, this->headRotTargetZ, 1, 0x3E8, 0);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->headRotTargetZ = 0;
        this->actionFunc = EnBombers_TalkWhileSitting;
    } else {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void EnBombers_TalkWhileSitting(EnBombers* this, PlayState* play) {
    Math_SmoothStepToS(&this->headRotZ, this->headRotTargetZ, 1, 0x3E8, 0);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        EnBombers_SetupSitting(this);
    }
}

void EnBombers_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombers* this = (EnBombers*)thisx;

    if (this->walkTimer != 0) {
        this->walkTimer--;
    }

    if (this->idleTimer != 0) {
        this->idleTimer--;
    }

    if (this->unk_2AC != 0) {
        this->unk_2AC--;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    EnBombers_PlaySfx(this);
    Actor_SetFocus(&this->actor, 20.0f);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->headRotX, this->headRotTargetX, 1, 0x1388, 0);
    Math_SmoothStepToS(&this->headRotZ, this->headRotTargetZ, 1, 0x1388, 0);

    if (this->blinkTimer == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
            this->blinkTimer = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    this->actor.cullingVolumeDistance = 500.0f;
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBombers_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBombers* this = (EnBombers*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_0F) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->headRotX;
        rot->z += this->headRotZ;
    }

    if (limbIndex == OBJECT_CS_LIMB_13) {
        *dList = NULL;
    }

    return false;
}

void EnBombers_Draw(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sNumberTextures[this->number]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(sBomberDisplayLists[this->number]));

    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBombers_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
