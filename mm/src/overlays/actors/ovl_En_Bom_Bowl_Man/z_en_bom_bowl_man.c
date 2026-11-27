/*
 * File: z_en_bom_bowl_man.c
 * Overlay: ovl_En_Bom_Bowl_Man
 * Description: Line of Bombers when they teach you the password
 */

#include "z_en_bom_bowl_man.h"
#include "objects/object_cs/object_cs.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBomBowlMan_Init(Actor* thisx, PlayState* play);
void EnBomBowlMan_Destroy(Actor* thisx, PlayState* play);
void EnBomBowlMan_Update(Actor* thisx, PlayState* play);
void EnBomBowlMan_Draw(Actor* thisx, PlayState* play);

void func_809C4BC4(EnBomBowlMan* this, PlayState* play);
void func_809C4DA4(EnBomBowlMan* this, PlayState* play);
void func_809C51B4(EnBomBowlMan* this, PlayState* play);
void func_809C52B4(EnBomBowlMan* this);
void func_809C5310(EnBomBowlMan* this, PlayState* play);
void func_809C53A4(EnBomBowlMan* this);
void func_809C5408(EnBomBowlMan* this, PlayState* play);
void func_809C5524(EnBomBowlMan* this, PlayState* play);
void func_809C5598(EnBomBowlMan* this, PlayState* play);
void func_809C5738(EnBomBowlMan* this, PlayState* play);
void func_809C59A4(EnBomBowlMan* this, PlayState* play);
void func_809C59F0(EnBomBowlMan* this, PlayState* play);
void func_809C5AA4(EnBomBowlMan* this, PlayState* play);
void func_809C5B1C(EnBomBowlMan* this, PlayState* play);
void func_809C5BA0(EnBomBowlMan* this);
void func_809C5BF4(EnBomBowlMan* this, PlayState* play);

s32 D_809C6100 = 0;

s32 D_809C6104 = 0;

ActorProfile En_Bom_Bowl_Man_Profile = {
    /**/ ACTOR_EN_BOM_BOWL_MAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBomBowlMan),
    /**/ EnBomBowlMan_Init,
    /**/ EnBomBowlMan_Destroy,
    /**/ EnBomBowlMan_Update,
    /**/ EnBomBowlMan_Draw,
};

typedef enum {
    /*   -1 */ ENBOMBOWLMAN_ANIM_NONE = -1,
    /* 0x00 */ ENBOMBOWLMAN_ANIM_IDLE,
    /* 0x01 */ ENBOMBOWLMAN_ANIM_HANDS_ON_HIPS,
    /* 0x02 */ ENBOMBOWLMAN_ANIM_TALK_NORMAL,
    /* 0x03 */ ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS,
    /* 0x04 */ ENBOMBOWLMAN_ANIM_BLOWGUN,
    /* 0x05 */ ENBOMBOWLMAN_ANIM_WALK,
    /* 0x06 */ ENBOMBOWLMAN_ANIM_SURPRISE,
    /* 0x07 */ ENBOMBOWLMAN_ANIM_HIDE_UNDER_BOX,
    /* 0x08 */ ENBOMBOWLMAN_ANIM_DAZED,
    /* 0x09 */ ENBOMBOWLMAN_ANIM_SIT,
    /* 0x0A */ ENBOMBOWLMAN_ANIM_HOLD_CUCCO,
    /* 0x0B */ ENBOMBOWLMAN_ANIM_TURN_AROUND,
    /* 0x0C */ ENBOMBOWLMAN_ANIM_SHOW_NUMBER,
    /* 0x0D */ ENBOMBOWLMAN_ANIM_TALK_SHOWING_NUMBER,
    /* 0x0E */ ENBOMBOWLMAN_ANIM_TAUNT,
    /* 0x0F */ ENBOMBOWLMAN_ANIM_JUMP,
    /* 0x10 */ ENBOMBOWLMAN_ANIM_ARM_SWIPE,
    /* 0x11 */ ENBOMBOWLMAN_ANIM_SALUTE,
    /* 0x12 */ ENBOMBOWLMAN_ANIM_RUN,
    /* 0x13 */ ENBOMBOWLMAN_ANIM_LOOK_AROUND,
    /* 0x14 */ ENBOMBOWLMAN_ANIM_MAX
} EnBomBowlManAnimation;

static AnimationHeader* sAnimations[ENBOMBOWLMAN_ANIM_MAX] = {
    &gBomberIdleAnim,              // ENBOMBOWLMAN_ANIM_IDLE
    &gBomberHandsOnHipsAnim,       // ENBOMBOWLMAN_ANIM_HANDS_ON_HIPS
    &gBomberTalkNormalAnim,        // ENBOMBOWLMAN_ANIM_TALK_NORMAL
    &gBomberTalkHandsOnHipsAnim,   // ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS
    &gBomberBlowgunAnim,           // ENBOMBOWLMAN_ANIM_BLOWGUN
    &gBomberWalkAnim,              // ENBOMBOWLMAN_ANIM_WALK
    &gBomberSurpriseAnim,          // ENBOMBOWLMAN_ANIM_SURPRISE
    &gBomberHideUnderBoxAnim,      // ENBOMBOWLMAN_ANIM_HIDE_UNDER_BOX
    &gBomberDazedAnim,             // ENBOMBOWLMAN_ANIM_DAZED
    &gBomberSitAnim,               // ENBOMBOWLMAN_ANIM_SIT
    &gBomberHoldCuccoAnim,         // ENBOMBOWLMAN_ANIM_HOLD_CUCCO
    &gBomberTurnAroundAnim,        // ENBOMBOWLMAN_ANIM_TURN_AROUND
    &gBomberShowNumberAnim,        // ENBOMBOWLMAN_ANIM_SHOW_NUMBER
    &gBomberTalkShowingNumberAnim, // ENBOMBOWLMAN_ANIM_TALK_SHOWING_NUMBER
    &gBomberTauntAnim,             // ENBOMBOWLMAN_ANIM_TAUNT
    &gBomberJumpAnim,              // ENBOMBOWLMAN_ANIM_JUMP
    &gBomberArmSwipeAnim,          // ENBOMBOWLMAN_ANIM_ARM_SWIPE
    &gBomberSaluteAnim,            // ENBOMBOWLMAN_ANIM_SALUTE
    &gBomberRunAnim,               // ENBOMBOWLMAN_ANIM_RUN
    &gBomberLookAroundAnim,        // ENBOMBOWLMAN_ANIM_LOOK_AROUND
};

static u8 sAnimationModes[ENBOMBOWLMAN_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_IDLE
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_HANDS_ON_HIPS
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_TALK_NORMAL
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_BLOWGUN
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_WALK
    ANIMMODE_ONCE, // ENBOMBOWLMAN_ANIM_SURPRISE
    ANIMMODE_ONCE, // ENBOMBOWLMAN_ANIM_HIDE_UNDER_BOX
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_DAZED
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_SIT
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_HOLD_CUCCO
    ANIMMODE_ONCE, // ENBOMBOWLMAN_ANIM_TURN_AROUND
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_SHOW_NUMBER
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_TALK_SHOWING_NUMBER
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_TAUNT
    ANIMMODE_ONCE, // ENBOMBOWLMAN_ANIM_JUMP
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_ARM_SWIPE
    ANIMMODE_ONCE, // ENBOMBOWLMAN_ANIM_SALUTE
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_RUN
    ANIMMODE_LOOP, // ENBOMBOWLMAN_ANIM_LOOK_AROUND
};

u16 D_809C618C[] = { 0x710, 0x711, 0x715, 0x716, 0x717, 0x718 };

u16 D_809C6198[] = { 0x74F, 0x750, 0x751, 0x752 };

Vec3f D_809C61A0[] = {
    { -730.0f, 200.0f, -2350.0f }, { -690.0f, 200.0f, -2350.0f }, { -650.0f, 200.0f, -2350.0f },
    { -610.0f, 200.0f, -2350.0f }, { -570.0f, 200.0f, -2350.0f },
};

void EnBomBowlMan_Init(Actor* thisx, PlayState* play) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gBomberSkel, &gBomberIdleAnim, this->jointTable, this->morphTable,
                       OBJECT_CS_LIMB_MAX);
    this->unk_2F6 = ENBOMBOWLMAN_GET_F0(&this->actor);
    this->unk_2F4 = ENBOMBOWLMAN_GET_F(&this->actor);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_SetScale(&this->actor, 0.01f);

    if (this->unk_2F6 == ENBOMBOWLMAN_F0_0) {
        if (this->unk_2F4 == 0) {
            func_809C4BC4(this, play);
        } else {
            this->unk_2F4--;
            func_809C5BA0(this);
        }
        return;
    }

    this->pathIndex = ENBOMBOWLMAN_GET_PATH_INDEX(&this->actor);
    this->path = SubS_GetPathByIndex(play, this->pathIndex, ENBOMBOWLMAN_PATH_INDEX_NONE);
    this->unk_2C8 = 80.0f;

    // 2S2H There are two VB's here, one so we can override the entrance requirement, another to override the bombers
    // notebook requirement
    if (GameInteractor_Should(VB_SETUP_EAST_CLOCK_TOWN_BOM_BOWL_MAN,
                              (gSaveContext.save.entrance == ENTRANCE(EAST_CLOCK_TOWN, 2)) &&
                                  GameInteractor_Should(VB_BE_ELIGIBLE_FOR_BOMBERS_NOTEBOOK,
                                                        CHECK_WEEKEVENTREG(WEEKEVENTREG_ENTERED_BOMBERS_CODE) &&
                                                            !CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK),
                                                        this),
                              this)) {
        this->csId = this->actor.csId;
        if (this->csId == 0) {
            Actor_Kill(&this->actor);
        }
        func_809C52B4(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnBomBowlMan_Destroy(Actor* thisx, PlayState* play) {
}

void EnBomBowlMan_ChangeAnim(EnBomBowlMan* this, s32 animIndex, f32 playSpeed) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

void func_809C49CC(EnBomBowlMan* this) {
    if ((this->animIndex == ENBOMBOWLMAN_ANIM_WALK) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMBOWLMAN_ANIM_TURN_AROUND) &&
        (Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 8.0f) ||
         Animation_OnFrame(&this->skelAnime, 12.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMBOWLMAN_ANIM_RUN) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 2.0f) ||
         Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMBOWLMAN_ANIM_JUMP) && Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }

    if ((this->animIndex == ENBOMBOWLMAN_ANIM_SURPRISE) && Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }
}

void func_809C4B50(EnBomBowlMan* this) {
    this->unk_2B8 = 0;
    this->unk_2BC = 0;
    this->unk_2BA = 0;
    this->unk_29E = 0;
    this->unk_290 = 0;
}

void func_809C4B6C(EnBomBowlMan* this) {
    if ((this->pathIndex != PATH_INDEX_NONE) && (this->path != NULL)) {
        if (!SubS_CopyPointFromPath(this->path, this->unk_298, &this->unk_2A0)) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_809C4BC4(EnBomBowlMan* this, PlayState* play) {
    s32 pad;
    s32 i;
    Vec3f sp7C;
    EnBomBowlMan* bomBowlMan;
    s8 code;

    func_809C4B50(this);
    EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_IDLE, 1.0f);

    for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.bomberCode); i++) {
        Math_Vec3f_Copy(&sp7C, &this->actor.home.pos);
        code = gSaveContext.save.saveInfo.bomberCode[i];
        if (code == 1) {
            Math_Vec3f_Copy(&this->actor.world.pos, &D_809C61A0[i]);
            this->unk_2DC[code - 1] = this;
        } else {
            Math_Vec3f_Copy(&sp7C, &D_809C61A0[i]);
            bomBowlMan = (EnBomBowlMan*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_BOM_BOWL_MAN,
                                                           sp7C.x, sp7C.y, sp7C.z, 0, this->actor.world.rot.y, 0, code);
            if (bomBowlMan != NULL) {
                if (i == 2) {
                    this->unk_2D8 = bomBowlMan;
                }
                this->unk_2DC[code - 1] = bomBowlMan;
            }
        }
    }

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[2]);
    } else if (!CutsceneManager_IsNext(this->csIdList[2])) {
        CutsceneManager_Queue(this->csIdList[2]);
    }

    EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS, 1.0f);
    this->unk_2D4 = this->actor.yawTowardsPlayer;
    this->unk_290 = this->actor.yawTowardsPlayer;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_73_10);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_85_02);
    this->unk_29C = 0;
    this->actionFunc = func_809C4DA4;
}

void func_809C4DA4(EnBomBowlMan* this, PlayState* play) {
    Math_SmoothStepToS(&this->unk_290, this->unk_2D4, 1, 0x1388, 0);

    if (this->unk_2B8 == 0) {
        Player* player = GET_PLAYER(play);

        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[2]);
            return;
        }

        if (!CutsceneManager_IsNext(this->csIdList[2])) {
            CutsceneManager_Queue(this->csIdList[2]);
            return;
        }

        CutsceneManager_StartWithPlayerCs(this->csIdList[2], &this->actor);
        this->unk_2B8 = 1;
        this->unk_2C0 = 0;
        this->unk_2D4 = this->actor.yawTowardsPlayer;
        this->unk_290 = this->actor.yawTowardsPlayer;
        if (player->transformation == PLAYER_FORM_HUMAN) {
            Message_StartTextbox(play, D_809C618C[0], &this->actor);
        } else {
            Message_StartTextbox(play, D_809C6198[0], &this->actor);
        }
    }

    if ((this->unk_2BC == 0) && (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Player* player = GET_PLAYER(play);
        s32 pad;
        s32 sp28 = false;

        Message_CloseTextbox(play);

        switch (this->unk_2C0) {
            case 0:
                this->unk_2C0 = 1;
                D_809C6104 = 1;
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIdList[2])),
                                      &this->unk_2D8->actor);
                this->unk_2D4 = 0;
                this->unk_2BC = 10;
                EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_SALUTE, 1.0f);
                break;

            case 1:
                D_809C6104 = 0;
                EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS, 1.0f);
                this->unk_2D4 = this->actor.yawTowardsPlayer;
                this->unk_2C0 = 2;
                if ((player->transformation == PLAYER_FORM_HUMAN) && CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
                    this->unk_2C0 = 4;
                }
                break;

            case 2:
                if (player->transformation == PLAYER_FORM_HUMAN) {
                    if (GameInteractor_Should(VB_BOM_BOWL_MAN_GIVE_ITEM, true, this)) {
                        this->unk_2B8 = 2;
                        CutsceneManager_Stop(this->csIdList[2]);
                        func_809C59A4(this, play);
                        sp28 = true;
                    }
                } else {
                    this->unk_2C0 = 3;
                    play->msgCtx.msgLength = 0;
                    EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_HANDS_ON_HIPS, 1.0f);
                    D_809C6100 = 1;
                    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
                        CutsceneManager_Queue(this->csIdList[3]);
                    } else if (!CutsceneManager_IsNext(this->csIdList[3])) {
                        CutsceneManager_Queue(this->csIdList[3]);
                    }
                    CutsceneManager_Stop(this->csIdList[2]);
                    this->actionFunc = func_809C5B1C;
                    sp28 = true;
                }
                break;

            case 3:
                if (player->transformation == PLAYER_FORM_HUMAN) {
                    this->unk_2C0 = 4;
                } else {
                    this->actionFunc = func_809C51B4;
                    return;
                }
                break;

            case 4:
                if (this->unk_2B8 != 2) {
                    CutsceneManager_Stop(this->csIdList[2]);
                }
                play->msgCtx.msgLength = 0;
                EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_HANDS_ON_HIPS, 1.0f);
                D_809C6100 = 1;
                this->actionFunc = func_809C5B1C;
                sp28 = true;
                break;

            case 5:
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE);
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK);
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_BOMBERS);
                this->actionFunc = func_809C51B4;
                sp28 = true;
                break;

            default:
                break;
        }

        if (!sp28) {
            if (player->transformation == PLAYER_FORM_HUMAN) {
                Message_ContinueTextbox(play, D_809C618C[this->unk_2C0]);
            } else {
                Message_ContinueTextbox(play, D_809C6198[this->unk_2C0]);
            }
        }
    }
}

void func_809C51B4(EnBomBowlMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((play->msgCtx.bombersNotebookEventQueueCount == 0) &&
        ((play->msgCtx.msgMode == MSGMODE_NONE) || (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE))) {
        play->nextEntrance = Entrance_CreateFromSpawn(6);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_86;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_75_40);
        if (player->transformation == PLAYER_FORM_HUMAN) {
            SET_WEEKEVENTREG(WEEKEVENTREG_84_80);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_77_02);
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_73_20);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_85_01);
        }
    }
}

void func_809C52B4(EnBomBowlMan* this) {
    this->actor.draw = NULL;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.world.pos.x = 1340.0f;
    this->actor.world.pos.z = -1795.0f;
    this->unk_29C = 3;
    this->actionFunc = func_809C5310;
}

void func_809C5310(EnBomBowlMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.x < 1510.0f) {
        if (player->transformation != PLAYER_FORM_DEKU) {
            if (this->actor.xzDistToPlayer < this->unk_2C8) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
                func_809C53A4(this);
            }
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void func_809C53A4(EnBomBowlMan* this) {
    this->actor.draw = EnBomBowlMan_Draw;
    this->actor.world.pos.x = 1360.0f;
    this->actor.world.pos.z = -1870.0f;
    EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_RUN, 1.0f);
    this->unk_29C = 3;
    this->actionFunc = func_809C5408;
}

void func_809C5408(EnBomBowlMan* this, PlayState* play) {
    s32 pad[2];
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos), 1,
                       0x1388, 0);
    Math_SmoothStepToS(&player->actor.shape.rot.y, Math_Vec3f_Yaw(&player->actor.world.pos, &this->actor.world.pos), 1,
                       0x1388, 0);
    player->actor.world.rot.y = player->actor.shape.rot.y;
    Math_ApproachF(&this->actor.world.pos.x, 1350.0f, 0.3f, 2.0f);
    Math_ApproachF(&this->actor.world.pos.z, -1800.0f, 0.3f, 2.0f);
    if (sqrtf(SQ(this->actor.world.pos.x - 1350.0f) + SQ(this->actor.world.pos.z + 1800.0f)) < 4.0f) {
        func_809C5524(this, play);
    }
}

void func_809C5524(EnBomBowlMan* this, PlayState* play) {
    this->actor.textId = 0x730;
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
        this->actor.textId = 0x72F;
    }
    EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_TALK_HANDS_ON_HIPS, 1.0f);
    Message_StartTextbox(play, this->actor.textId, &this->actor);
    this->actionFunc = func_809C5598;
}

void func_809C5598(EnBomBowlMan* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if ((this->actor.textId == 0x72F) || (this->actor.textId == 0x730)) {
            this->actor.textId = 0x731;
        } else if (this->actor.textId == 0x731) {
            this->actor.textId = 0x732;
        } else if (this->actor.textId == 0x732) {
            this->actor.textId = 0x733;
        } else if (this->actor.textId == 0x733) {
            this->actor.textId = 0x734;
        } else if (this->actor.textId == 0x734) {
            this->actor.textId = 0x715;
        } else if (this->actor.textId == 0x715) {
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
            EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_SALUTE, 1.0f);
            func_809C59A4(this, play);
            return;
        } else if (this->actor.textId == 0x716) {
            this->actor.textId = 0x735;
        } else if (this->actor.textId == 0x735) {
            this->unk_2C2 = 0;
            EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_SALUTE, 1.0f);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_BOMBERS);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->actionFunc = func_809C5738;
            return;
        }
        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void func_809C5738(EnBomBowlMan* this, PlayState* play) {
    s32 pad[3];
    s16 yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_2A0);

    if (this->unk_2C2 == 0) {
        if ((play->msgCtx.bombersNotebookEventQueueCount == 0) &&
            ((play->msgCtx.msgMode == MSGMODE_NONE) || (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE))) {
            this->unk_2C2 = 1;
            func_809C4B6C(this);
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
                CutsceneManager_Queue(this->csId);
            } else if (!CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_Queue(this->csId);
            } else {
                CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
                this->unk_2C2 = 2;
                EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_RUN, 1.0f);
            }
        }
    } else if (this->unk_2C2 == 1) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csId);
        } else if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Queue(this->csId);
        } else {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_2C2 = 2;
            EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_RUN, 1.0f);
        }
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 1, 0x7D0, 0xA);
        Math_ApproachF(&this->actor.world.pos.x, this->unk_2A0.x, 0.5f, 6.0f);
        Math_ApproachF(&this->actor.world.pos.z, this->unk_2A0.z, 0.5f, 6.0f);
        if ((sqrtf(SQ(this->actor.world.pos.x - this->unk_2A0.x) + SQ(this->actor.world.pos.z - this->unk_2A0.z)) <
             4.0f) &&
            (this->path != NULL)) {
            this->unk_298++;
            if (this->unk_298 >= this->path->count) {
                SET_WEEKEVENTREG(WEEKEVENTREG_84_80);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_04);
                CutsceneManager_Stop(this->csId);
                Actor_Kill(&this->actor);
                return;
            }

            func_809C4B6C(this);
        }
    }
}

void func_809C59A4(EnBomBowlMan* this, PlayState* play) {
    Actor_OfferGetItem(&this->actor, play, GI_BOMBERS_NOTEBOOK, 300.0f, 300.0f);
    this->unk_29C = 1;
    this->actionFunc = func_809C59F0;
}

void func_809C59F0(EnBomBowlMan* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (this->unk_2F6 == ENBOMBOWLMAN_F0_0) {
            this->unk_2C0 = 3;
            this->actor.textId = D_809C618C[this->unk_2C0];
        } else {
            this->actor.textId = 0x716;
        }
        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);
        this->actionFunc = func_809C5AA4;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_BOMBERS_NOTEBOOK, 300.0f, 300.0f);
    }
}

void func_809C5AA4(EnBomBowlMan* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->unk_2F6 == ENBOMBOWLMAN_F0_0) {
            this->actionFunc = func_809C4DA4;
        } else {
            this->actionFunc = func_809C5598;
        }
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);
    }
}

void func_809C5B1C(EnBomBowlMan* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[3]);
    } else if (!CutsceneManager_IsNext(this->csIdList[3])) {
        CutsceneManager_Queue(this->csIdList[3]);
    } else {
        CutsceneManager_StartWithPlayerCs(this->csIdList[3], &this->actor);
        func_809C5BA0(this);
    }
}

void func_809C5BA0(EnBomBowlMan* this) {
    if (this->unk_2F4 != 0) {
        EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_IDLE, 1.0f);
    } else {
        this->animIndex = ENBOMBOWLMAN_ANIM_IDLE;
    }
    this->unk_29C = 2;
    this->actionFunc = func_809C5BF4;
}

void func_809C5BF4(EnBomBowlMan* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Camera* subCam;

    if ((D_809C6104 != 0) && (this->animIndex != ENBOMBOWLMAN_ANIM_JUMP)) {
        EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_JUMP, 1.0f);
    }

    if ((this->animIndex == ENBOMBOWLMAN_ANIM_JUMP) && (curFrame >= this->animEndFrame)) {
        EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_IDLE, 1.0f);
    }

    if (D_809C6100 != 0) {
        if ((this->animIndex == ENBOMBOWLMAN_ANIM_IDLE) && (D_809C6100 == (this->unk_2F4 + 1))) {
            EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_TURN_AROUND, 1.0f);
        }

        if ((this->animIndex == ENBOMBOWLMAN_ANIM_TURN_AROUND) && (curFrame >= this->animEndFrame)) {
            EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_SHOW_NUMBER, 1.0f);
            D_809C6100++;
        }

        if (this->unk_2F4 == 0) {
            subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIdList[3]));

            if (D_809C6100 > 5) {
                Player* player = GET_PLAYER(play);

                Message_CloseTextbox(play);
                Camera_SetTargetActor(subCam, &this->unk_2D8->actor);
                EnBomBowlMan_ChangeAnim(this, ENBOMBOWLMAN_ANIM_TALK_SHOWING_NUMBER, 1.0f);
                D_809C6100 = 0;
                if (player->transformation == PLAYER_FORM_HUMAN) {
                    this->unk_2C0 = 5;
                    Message_StartTextbox(play, D_809C618C[this->unk_2C0], &this->actor);
                } else {
                    this->unk_2C0 = 3;
                    Message_StartTextbox(play, D_809C6198[this->unk_2C0], &this->actor);
                }
                this->unk_29C = 0;
                this->actionFunc = func_809C4DA4;
            } else {
                s32 idx = D_809C6100 - 1;
                Camera_SetTargetActor(subCam, &this->unk_2DC[idx]->actor);
            }
        }
    }
}

void EnBomBowlMan_Update(Actor* thisx, PlayState* play) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    if (this->unk_2BA != 0) {
        this->unk_2BA--;
    }

    if (this->unk_2BC != 0) {
        this->unk_2BC--;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    func_809C49CC(this);
    Actor_SetFocus(&this->actor, 20.0f);

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->unk_28A, this->unk_290, 1, 0x1388, 0);
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0x3E8, 0);

    if (this->unk_2F2 == 0) {
        this->unk_2F0++;
        if (this->unk_2F0 >= 3) {
            this->unk_2F0 = 0;
            this->unk_2F2 = (s32)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

s32 EnBomBowlMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_0F) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->unk_28A;
    }

    if ((limbIndex == OBJECT_CS_LIMB_13) && (this->unk_2F6 == ENBOMBOWLMAN_F0_2)) {
        *dList = NULL;
    }

    if ((limbIndex == OBJECT_CS_LIMB_14) && (this->unk_2F6 == ENBOMBOWLMAN_F0_0)) {
        *dList = NULL;
    }
    return false;
}

#include "overlays/ovl_En_Bom_Bowl_Man/ovl_En_Bom_Bowl_Man.h"

TexturePtr D_809C6200[] = {
    gBomberRedBandanaTex, gBomberBlueBandanaTex, gBomberBlueBandanaTex, gBomberBlueBandanaTex, gBomberBlueBandanaTex,
};

TexturePtr D_809C6214[] = {
    gBomberEyeOpenTex,
    gBomberEyeHalfTex,
    gBomberEyeClosedTex,
};

TexturePtr D_809C6220[] = {
    gBomberNumber1Tex, gBomberNumber2Tex, gBomberNumber3Tex, gBomberNumber4Tex, gBomberNumber5Tex,
};

void EnBomBowlMan_Draw(Actor* thisx, PlayState* play) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_809C6214[this->unk_2F0]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_809C6220[this->unk_2F4]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(D_809C6200[this->unk_2F4]));

    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBomBowlMan_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
