/*
 * File: z_en_kendo_js.c
 * Overlay: ovl_En_Kendo_Js
 * Description: Swordsman
 */

#include "z_en_kendo_js.h"
#include "overlays/actors/ovl_En_Maruta/z_en_maruta.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnKendoJs_Init(Actor* thisx, PlayState* play);
void EnKendoJs_Destroy(Actor* thisx, PlayState* play);
void EnKendoJs_Update(Actor* thisx, PlayState* play);
void EnKendoJs_Draw(Actor* thisx, PlayState* play);

void EnKendoJs_SetupAwaitingTalk(EnKendoJs* this);
void EnKendoJs_AwaitTalk(EnKendoJs* this, PlayState* play);
void EnKendoJs_SetupMessageStateHandler(EnKendoJs* this);
void EnKendoJs_HandleMessageState(EnKendoJs* this, PlayState* play);
void EnKendoJs_SetupNoviceCourse(EnKendoJs* this);
void EnKendoJs_MovePlayerToStartNoviceCourse(EnKendoJs* this, PlayState* play);
void EnKendoJs_StartNoviceCourse(EnKendoJs* this);
void EnKendoJs_NoviceCourse(EnKendoJs* this, PlayState* play);
void EnKendoJs_SetupExpertCourse(EnKendoJs* this);
void EnKendoJs_MovePlayerToStartExpertCourse(EnKendoJs* this, PlayState* play);
void EnKendoJs_ExpertCourse(EnKendoJs* this, PlayState* play);
void EnKendoJs_SetupPrize(EnKendoJs* this);
void EnKendoJs_AwardPrize(EnKendoJs* this, PlayState* play);
void EnKendoJs_AfterPrizeAccepted(EnKendoJs* this);
void EnKendoJs_AwaitFinalMessage(EnKendoJs* this, PlayState* play);
void EnKendoJs_SetExpertCourseCamera(EnKendoJs* this, PlayState* play);
s32 EnKendoJs_MovePlayerToPos(PlayState* play, Vec3f targetPos);
void EnKendoJs_SpawnLog(EnKendoJs* this, PlayState* play, s32 position);
void EnKendoJs_RetractLogs(EnKendoJs* this, PlayState* play);

ActorProfile En_Kendo_Js_Profile = {
    /**/ ACTOR_EN_KENDO_JS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_JS,
    /**/ sizeof(EnKendoJs),
    /**/ EnKendoJs_Init,
    /**/ EnKendoJs_Destroy,
    /**/ EnKendoJs_Update,
    /**/ EnKendoJs_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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
    { 18, 30, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnKendoJsAnimation {
    /* 0 */ ENKENDOJS_ANIM_SLAPPING_KNEE, // unused
    /* 1 */ ENKENDOJS_ANIM_DEFAULT,
    /* 2 */ ENKENDOJS_ANIM_FLAILING,
    /* 3 */ ENKENDOJS_ANIM_STOP_FLAILING,
    /* 4 */ ENKENDOJS_ANIM_COWERING,
    /* 5 */ ENKENDOJS_ANIM_MAX
} EnKendoJsAnimation;

static AnimationInfo sAnimationInfo[ENKENDOJS_ANIM_MAX] = {
    { &gSwordmasterSlappingKneeAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // unused
    { &gSwordmasterDefaultAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },
    { &gSwordmasterFlailingAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },
    { &gSwordmasterStopFlailingAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },
    { &gSwordmasterCoweringAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },
};

#define ENKENDOJS_START_MSG(this, play, textId)       \
    Message_StartTextbox(play, textId, &this->actor); \
    this->curTextId = textId;

s16 sNonHumanTextIds[][3] = {
    { 0x2731, 0x2732, 0x2733 },
    { 0x2734, 0x2735, 0x2736 },
    { 0x2737, 0x2738, 0x2739 },
};

s16 sHumanTextIds[][3] = {
    { 0x2710, 0x2712, 0x2714 },
    { 0x2711, 0x2713, 0x2715 },
};

s16 sNoviceCourseProgressTextIds[] = {
    0x271C, 0x271E, 0x2720, 0x2722, 0x2724, 0x2726, 0x2728,
};

s16 sNoviceCourseTryAgainTextIds[] = {
    0x271B, 0x271D, 0x271F, 0x2721, 0x2723, 0x2725, 0x2727,
};

typedef enum EnKendoJsCourseState {
    /* 0 */ ENKENDOJS_COURSE_PAUSED,
    /* 1 */ ENKENDOJS_COURSE_ACTIVE,
    /* 2 */ ENKENDOJS_COURSE_RETRY,
} EnKendoJsCourseState;

typedef enum EnKendoJsNoviceCourseLesson {
    /* 0 */ ENKENDOJS_NOVICE_SIDE_JUMP,
    /* 1 */ ENKENDOJS_NOVICE_ROLL,
    /* 2 */ ENKENDOJS_NOVICE_BACKFLIP,
    /* 3 */ ENKENDOJS_NOVICE_SWORD_ATTACK,
    /* 4 */ ENKENDOJS_NOVICE_SWORD_TARGET,
    /* 5 */ ENKENDOJS_NOVICE_SWORD_THRUST,
    /* 6 */ ENKENDOJS_NOVICE_JUMP_ATTACK,
    /* 7 */ ENKENDOJS_NOVICE_FINISHED,
} EnKendoJsNoviceCourseLesson;

typedef enum EnKendoJsNoviceCourseActionResult {
    /* 0 */ ENKENDOJS_RESULT_CORRECT,
    /* 1 */ ENKENDOJS_RESULT_INCORRECT,
    /* 2 */ ENKENDOJS_RESULT_PENDING,
} EnKendoJsNoviceCourseActionResult;

void EnKendoJs_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKendoJs* this = (EnKendoJs*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);

    SkelAnime_InitFlex(play, &this->skelAnime, &gSwordmasterSkel, &gSwordmasterDefaultAnim, this->jointTable,
                       this->morphTable, OBJECT_JS_LIMB_MAX);

    if ((CURRENT_DAY == 3) && ((CURRENT_TIME > CLOCK_TIME(23, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
        if (ENKENDOJS_GET_LOCATION(&this->actor) != ENKENDOJS_IN_BACK_ROOM) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, this->actor.home.pos.x, this->actor.home.pos.y,
                        this->actor.home.pos.z - 10.0f, this->actor.home.rot.x, this->actor.home.rot.y,
                        this->actor.home.rot.z, 0x10);
            Actor_Kill(&this->actor);
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_COWERING);
        }
    } else if (ENKENDOJS_GET_LOCATION(&this->actor) == ENKENDOJS_IN_BACK_ROOM) {
        Actor_Kill(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (ENKENDOJS_GET_LOCATION(&this->actor) != ENKENDOJS_IN_BACK_ROOM) {
        Path* path = &play->setupPathList[ENKENDOJS_GET_PATH_INDEX(&this->actor)];

        this->pathPoints = Lib_SegmentedToVirtual(path->points);
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 30.0f;
    this->actor.child = NULL;
    this->hasSpoken = false;
    this->numLogs = 0;
    this->isSlashingLog = false;
    this->courseState = ENKENDOJS_COURSE_PAUSED;
    EnKendoJs_SetupAwaitingTalk(this);
}

void EnKendoJs_Destroy(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_PLAYING_SWORDSMAN_MINIGAME);
}

void EnKendoJs_SetupAwaitingTalk(EnKendoJs* this) {
    this->actionFunc = EnKendoJs_AwaitTalk;
}

void EnKendoJs_AwaitTalk(EnKendoJs* this, PlayState* play) {
    s32 mainIndex;
    s32 dayIndex;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (CURRENT_DAY != 0) {
            dayIndex = CURRENT_DAY - 1;
        } else {
            dayIndex = 0;
        }

        if (ENKENDOJS_GET_LOCATION(&this->actor) == ENKENDOJS_IN_BACK_ROOM) {
            ENKENDOJS_START_MSG(this, play, 0x273C);
        } else if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_DEKU:
                    mainIndex = 0;
                    break;

                case PLAYER_FORM_GORON:
                    mainIndex = 1;
                    break;

                case PLAYER_FORM_ZORA:
                    mainIndex = 2;
                    break;

                default:
                    mainIndex = 0;
                    break;
            }

            ENKENDOJS_START_MSG(this, play, sNonHumanTextIds[mainIndex][dayIndex]);
        } else if ((Player_GetMask(play) != PLAYER_MASK_NONE) && (Player_GetMask(play) < PLAYER_MASK_GIANT)) {
            u16 maskMessage = Player_GetMask(play) + 0x273C;

            //! FAKE:
            if (1) {}

            ENKENDOJS_START_MSG(this, play, maskMessage);
        } else {
            if (!this->hasSpoken) {
                this->hasSpoken = true;
                mainIndex = 0;
            } else {
                mainIndex = 1;
            }
            ENKENDOJS_START_MSG(this, play, sHumanTextIds[mainIndex][dayIndex]);
        }

        EnKendoJs_SetupMessageStateHandler(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnKendoJs_HandleMessageChoices(EnKendoJs* this, PlayState* play) {
    if (Message_ShouldAdvance(play) && (this->curTextId == 0x2716)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    ENKENDOJS_START_MSG(this, play, 0x272C);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_FLAILING);
                } else if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.firstChoicePrice) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    ENKENDOJS_START_MSG(this, play, 0x2718);
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-play->msgCtx.firstChoicePrice);
                    ENKENDOJS_START_MSG(this, play, 0x2719);
                }
                break;

            case 1:
                if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    ENKENDOJS_START_MSG(this, play, 0x272C);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_FLAILING);
                } else if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.secondChoicePrice) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    ENKENDOJS_START_MSG(this, play, 0x2718);
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-play->msgCtx.secondChoicePrice);
                    ENKENDOJS_START_MSG(this, play, 0x273A);
                }
                break;

            case 2:
                Audio_PlaySfx_MessageCancel();
                ENKENDOJS_START_MSG(this, play, 0x2717);
                break;

            default:
                break;
        }
    }
}

void EnKendoJs_HandleMessageEvents(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Message_ShouldAdvance(play)) {
        return;
    }

    switch (this->curTextId) {
        case 0x2710:
        case 0x2711:
        case 0x2712:
        case 0x2713:
        case 0x2714:
        case 0x2715:
            ENKENDOJS_START_MSG(this, play, 0x2716);
            break;

        case 0x2719:
            Message_CloseTextbox(play);
            player->stateFlags1 |= PLAYER_STATE1_20;
            EnKendoJs_SetupNoviceCourse(this);
            break;

        case 0x271A:
            Message_CloseTextbox(play);
            EnKendoJs_StartNoviceCourse(this);
            break;

        case 0x273A:
            ENKENDOJS_START_MSG(this, play, 0x273B);
            break;

        case 0x273B:
            Message_CloseTextbox(play);
            Interface_InitMinigame(play);
            player->stateFlags1 |= PLAYER_STATE1_20;
            EnKendoJs_SetupExpertCourse(this);
            break;

        case 0x272D:
            Message_CloseTextbox(play);
            gSaveContext.minigameStatus = MINIGAME_STATUS_END;
            EnKendoJs_SetupPrize(this);
            EnKendoJs_AwardPrize(this, play);
            break;

        default:
            break;
    }
}

void EnKendoJs_SetupMessageStateHandler(EnKendoJs* this) {
    this->actionFunc = EnKendoJs_HandleMessageState;
}

void EnKendoJs_HandleMessageState(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            EnKendoJs_HandleMessageChoices(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnKendoJs_HandleMessageEvents(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if (this->curTextId == 0x272C) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_STOP_FLAILING);
                }

                if ((this->curTextId == 0x272E) || (this->curTextId == 0x272F) || (this->curTextId == 0x2730)) {
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                }

                player->stateFlags1 &= ~PLAYER_STATE1_20;
                EnKendoJs_SetupAwaitingTalk(this);
            }

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        default:
            break;
    }
}

s32 EnKendoJs_GetNoviceCourseActionResult(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->minigameRound.noviceRound) {
        case ENKENDOJS_NOVICE_SIDE_JUMP:
            if (Player_IsSideJumping(play)) {
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags3 & PLAYER_STATE3_8000000) || (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        case ENKENDOJS_NOVICE_ROLL:
            if ((player->stateFlags3 & PLAYER_STATE3_8000000)) {
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        case ENKENDOJS_NOVICE_BACKFLIP:
            if (Player_IsBackJumping(play)) {
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags3 & PLAYER_STATE3_8000000) || (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return ENKENDOJS_RESULT_INCORRECT;
            }
            this->isSlashingLog = false;
            break;

        case ENKENDOJS_NOVICE_SWORD_ATTACK:
            if ((this->isSlashingLog == true) && ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                                                  (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_COMBO_1H))) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((this->isSlashingLog == true) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        case ENKENDOJS_NOVICE_SWORD_TARGET:
            if ((this->isSlashingLog == true) && ((player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) ||
                                                  (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_COMBO_1H))) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((this->isSlashingLog == true) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        case ENKENDOJS_NOVICE_SWORD_THRUST:
            if ((this->isSlashingLog == true) && (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H)) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((this->isSlashingLog == true) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        case ENKENDOJS_NOVICE_JUMP_ATTACK:
            if ((this->isSlashingLog == true) && ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) ||
                                                  (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH))) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_CORRECT;
            }

            if ((this->isSlashingLog == true) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->isSlashingLog = false;
                return ENKENDOJS_RESULT_INCORRECT;
            }
            break;

        default:
            break;
    }
    return ENKENDOJS_RESULT_PENDING;
}

void EnKendoJs_NoviceCourseProgress(EnKendoJs* this, PlayState* play) {
    ENKENDOJS_START_MSG(this, play, sNoviceCourseProgressTextIds[this->minigameRound.noviceRound]);
    this->minigameRound.noviceRound++;
}

void EnKendoJs_NoviceCourseTryAgain(EnKendoJs* this, PlayState* play) {
    ENKENDOJS_START_MSG(this, play, sNoviceCourseTryAgainTextIds[this->minigameRound.noviceRound]);
}

s32 EnKendoJs_HasMoreAdviceToTryAgain(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->curTextId) {
        case 0x271D:
            if (Player_GetMeleeWeaponHeld(player) != PLAYER_MELEEWEAPON_NONE) {
                ENKENDOJS_START_MSG(this, play, 0x272A);
                return true;
            }
            break;

        case 0x2721:
            if (this->isPlayerLockedOn) {
                ENKENDOJS_START_MSG(this, play, 0x272B);
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

void EnKendoJs_SetupNoviceCourse(EnKendoJs* this) {
    this->actionFunc = EnKendoJs_MovePlayerToStartNoviceCourse;
}

void EnKendoJs_MovePlayerToStartNoviceCourse(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f startPos = this->actor.world.pos;

    startPos.z += 200.0f;

    if (EnKendoJs_MovePlayerToPos(play, startPos)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            EnKendoJs_SpawnLog(this, play, 0);
            ENKENDOJS_START_MSG(this, play, 0x271A);
            EnKendoJs_SetupMessageStateHandler(this);
        } else {
            Actor_OfferTalk(&this->actor, play, 800.0f);
        }
    }
}

void EnKendoJs_StartNoviceCourse(EnKendoJs* this) {
    SET_WEEKEVENTREG(WEEKEVENTREG_PLAYING_SWORDSMAN_MINIGAME);
    this->numLogs = 1;
    this->timer = 0;
    this->minigameRound.noviceRound = ENKENDOJS_NOVICE_SIDE_JUMP;
    this->courseState = ENKENDOJS_COURSE_ACTIVE;
    this->actionFunc = EnKendoJs_NoviceCourse;
}

void EnKendoJs_NoviceCourse(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->curTextId == 0x2729) {
            EnKendoJs_NoviceCourseTryAgain(this, play);
        } else if (!EnKendoJs_HasMoreAdviceToTryAgain(this, play)) {
            if (this->skelAnime.animation == &gSwordmasterFlailingAnim) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_STOP_FLAILING);
            }
            this->courseState = ENKENDOJS_COURSE_RETRY;
            Message_CloseTextbox(play);
            player->stateFlags1 &= ~PLAYER_STATE1_20;
        }
    } else if (this->courseState == ENKENDOJS_COURSE_RETRY) {
        this->courseState = ENKENDOJS_COURSE_ACTIVE;
    }

    if (this->courseState == ENKENDOJS_COURSE_ACTIVE) {
        switch (EnKendoJs_GetNoviceCourseActionResult(this, play)) {
            case ENKENDOJS_RESULT_CORRECT:
                this->courseState = ENKENDOJS_COURSE_PAUSED;
                Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
                player->stateFlags1 |= PLAYER_STATE1_20;
                EnKendoJs_NoviceCourseProgress(this, play);
                break;

            case ENKENDOJS_RESULT_INCORRECT:
                Actor_PlaySfx(&this->actor, NA_SE_SY_ERROR);
                this->courseState = ENKENDOJS_COURSE_PAUSED;
                player->stateFlags1 |= PLAYER_STATE1_20;
                ENKENDOJS_START_MSG(this, play, 0x2729);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_FLAILING);
                break;

            default:
                if (this->numLogs == 0) {
                    this->timer++;
                    if (this->timer == 30) {
                        this->timer = 0;
                        EnKendoJs_SpawnLog(this, play, 0);
                    }
                }
                break;
        }

        if ((this->skelAnime.animation == &gSwordmasterStopFlailingAnim) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_DEFAULT);
        }

        if (this->minigameRound.noviceRound == ENKENDOJS_NOVICE_FINISHED) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_PLAYING_SWORDSMAN_MINIGAME);
            EnKendoJs_SetupMessageStateHandler(this);
        }
    }
}

void EnKendoJs_SetupExpertCourse(EnKendoJs* this) {
    SET_WEEKEVENTREG(WEEKEVENTREG_PLAYING_SWORDSMAN_MINIGAME);
    this->timer = 120;
    this->minigameRound.expertRound = 0;
    this->courseState = ENKENDOJS_COURSE_ACTIVE;
    this->actionFunc = EnKendoJs_MovePlayerToStartExpertCourse;
}

void EnKendoJs_MovePlayerToStartExpertCourse(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f startPos = this->actor.world.pos;

    startPos.z += 300.0f;

    if (EnKendoJs_MovePlayerToPos(play, startPos)) {
        this->numLogs = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->actionFunc = EnKendoJs_ExpertCourse;
    }
}

void EnKendoJs_ExpertCourse(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer >= 140) {
        if (this->minigameRound.expertRound == 5) {
            if (gSaveContext.minigameScore == 30) {
                ENKENDOJS_START_MSG(this, play, 0x272D);
            } else {
                ENKENDOJS_START_MSG(this, play, 0x272E);
            }
            player->stateFlags1 |= PLAYER_STATE1_20;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_PLAYING_SWORDSMAN_MINIGAME);
            EnKendoJs_SetupMessageStateHandler(this);
            return;
        }

        Audio_PlaySfx(NA_SE_SY_FOUND);
        EnKendoJs_SpawnLog(this, play, ((u8)Rand_Next() % 3) + 1);
        EnKendoJs_SpawnLog(this, play, ((u8)Rand_Next() % 3) + 4);
        this->timer = 0;
        this->minigameRound.expertRound++;
    } else if (this->timer == 120) {
        EnKendoJs_RetractLogs(this, play);
        this->timer++;
    } else {
        this->timer++;
    }

    if (this->isSlashingLog == true) {
        if ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) ||
            (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {
            play->interfaceCtx.minigamePoints = 3;
            if (gSaveContext.minigameScore >= 27) {
                player->stateFlags1 |= PLAYER_STATE1_20;
            }
        } else if (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H) {
            play->interfaceCtx.minigamePoints = 2;
        } else {
            play->interfaceCtx.minigamePoints = 1;
        }
        Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
        this->isSlashingLog = false;
    }
    EnKendoJs_SetExpertCourseCamera(this, play);
}

void EnKendoJs_SetupPrize(EnKendoJs* this) {
    this->actionFunc = EnKendoJs_AwardPrize;
}

void EnKendoJs_AwardPrize(EnKendoJs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        EnKendoJs_AfterPrizeAccepted(this);
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 800.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 800.0f, 100.0f);
    }
}

void EnKendoJs_AfterPrizeAccepted(EnKendoJs* this) {
    this->actionFunc = EnKendoJs_AwaitFinalMessage;
}

void EnKendoJs_AwaitFinalMessage(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE);
            ENKENDOJS_START_MSG(this, play, 0x272F);
        } else {
            ENKENDOJS_START_MSG(this, play, 0x2730);
        }
        EnKendoJs_SetupMessageStateHandler(this);
        player->stateFlags1 &= ~PLAYER_STATE1_20;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void EnKendoJs_SetExpertCourseCamera(EnKendoJs* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        Camera_ChangeActorCsCamIndex(play->cameraPtrs[CAM_ID_MAIN],
                                     CutsceneManager_GetCutsceneEntry(this->actor.csId)->csCamId);
    }
}

void EnKendoJs_KeepPlayerInTrainingArea(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 zClamp = this->actor.world.pos.z + 70.0f;

    if ((ENKENDOJS_GET_LOCATION(&this->actor) != ENKENDOJS_IN_BACK_ROOM) && (player->actor.world.pos.z < zClamp)) {
        player->actor.world.pos.z = zClamp;
    }
}

s32 EnKendoJs_MovePlayerToPos(PlayState* play, Vec3f targetPos) {
    Player* player = GET_PLAYER(play);
    f32 distXZ;
    f32 controlStickMagnitude;
    s16 controlStickAngle;

    controlStickAngle = Math_Vec3f_Yaw(&player->actor.world.pos, &targetPos);
    distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &targetPos);

    if (distXZ < 20.0f) {
        controlStickMagnitude = 10.0f;
    } else if (distXZ < 40.0f) {
        controlStickMagnitude = 40.0f;
    } else {
        controlStickMagnitude = 80.0f;
    }

    play->actorCtx.isOverrideInputOn = true;
    Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, controlStickAngle);

    if (distXZ < 20.0f) {
        return true;
    }
    return false;
}

void EnKendoJs_UpdateCollider(EnKendoJs* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnKendoJs_SpawnLog(EnKendoJs* this, PlayState* play, s32 position) {
    f32 x = this->pathPoints[position].x;
    f32 y = this->pathPoints[position].y;
    f32 z = this->pathPoints[position].z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MARUTA, x, y, z, 0, 0, 0, 0);
    this->numLogs++;
}

void EnKendoJs_RetractLogs(EnKendoJs* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_MARUTA) {
            ((EnMaruta*)actor)->isRetracting = true;
        }
        actor = actor->next;
    }

    this->numLogs = 0;
}

void EnKendoJs_Update(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    EnKendoJs_UpdateCollider(this, play);
    EnKendoJs_KeepPlayerInTrainingArea(this, play);
}

s32 EnKendoJs_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    if (limbIndex == OBJECT_JS_LIMB_0C) {
        rot->y -= this->headRot.y;
    }
    return false;
}

void EnKendoJs_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnKendoJs_Draw(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnKendoJs_OverrideLimbDraw, EnKendoJs_PostLimbDraw, &this->actor);
}
