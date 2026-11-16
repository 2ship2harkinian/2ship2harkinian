/*
 * File: z_obj_wturn.c
 * Overlay: ovl_Obj_Wturn
 * Description: Stone Tower Temple Inverter
 */

#include "z_obj_wturn.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ObjWturn_Init(Actor* thisx, PlayState* play);
void ObjWturn_Update(Actor* thisx, PlayState* play);

void ObjWturn_SetupIdle(ObjWturn* this);
void ObjWturn_Idle(ObjWturn* this, PlayState* play);
void ObjWturn_TriggerInversion(ObjWturn* this);
void ObjWturn_AwaitInversion(ObjWturn* this, PlayState* play);
void ObjWturn_SetupInversionCs(ObjWturn* this, PlayState* play);
void ObjWturn_InversionCs(ObjWturn* this, PlayState* play);
void ObjWturn_SetupFallingCs(ObjWturn* this, PlayState* play);
void ObjWturn_FallingCs(ObjWturn* this, PlayState* play);

ActorProfile Obj_Wturn_Profile = {
    /**/ ACTOR_OBJ_WTURN,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjWturn),
    /**/ ObjWturn_Init,
    /**/ Actor_Noop,
    /**/ ObjWturn_Update,
    /**/ NULL,
};

void ObjWturn_Init(Actor* thisx, PlayState* play) {
    ObjWturn* this = (ObjWturn*)thisx;

    ObjWturn_SetupIdle(this);
}

void ObjWturn_SetupIdle(ObjWturn* this) {
    this->actionFunc = ObjWturn_Idle;
}

void ObjWturn_Idle(ObjWturn* this, PlayState* play) {
    if ((play->msgCtx.ocarinaMode >= OCARINA_MODE_WARP_TO_GREAT_BAY_COAST) &&
        (play->msgCtx.ocarinaMode <= OCARINA_MODE_WARP_TO_ENTRANCE)) {
        Flags_UnsetSwitch(play, OBJWTURN_GET_SWITCH_FLAG(&this->actor));
        Actor_Kill(&this->actor);
        return;
    }

    if ((Flags_GetSwitch(play, OBJWTURN_GET_SWITCH_FLAG(&this->actor)) && (play->sceneId == SCENE_F40)) ||
        (!Flags_GetSwitch(play, OBJWTURN_GET_SWITCH_FLAG(&this->actor)) && (play->sceneId == SCENE_F41))) {
        ObjWturn_TriggerInversion(this);
    }
}

void ObjWturn_TriggerInversion(ObjWturn* this) {
    CutsceneManager_Queue(this->actor.csId);
    this->actionFunc = ObjWturn_AwaitInversion;
}

void ObjWturn_AwaitInversion(ObjWturn* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        ObjWturn_SetupInversionCs(this, play);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void ObjWturn_SetupInversionCs(ObjWturn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamEye;
    Vec3f subCamAt;

    CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
    Play_EnableMotionBlur(140);
    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
    subCamAt.x = player->actor.focus.pos.x;
    subCamAt.z = player->actor.focus.pos.z;
    subCamAt.y = player->actor.focus.pos.y;
    subCamEye.x = (Math_SinS(this->actor.shape.rot.y) * 150.0f) + subCamAt.x;
    subCamEye.z = (Math_CosS(this->actor.shape.rot.y) * 150.0f) + subCamAt.z;
    subCamEye.y = subCamAt.y + 4.0f;
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    this->actionFunc = ObjWturn_InversionCs;
}

void ObjWturn_InversionCs(ObjWturn* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->actor.shape.rot.z, -0x8000, 0x200)) {
        ObjWturn_SetupFallingCs(this, play);
    }
    Actor_PlaySfx_FlaggedCentered2(&this->actor, NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    Play_SetCameraRoll(play, this->subCamId, this->actor.shape.rot.z);
}

void ObjWturn_SetupFallingCs(ObjWturn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.world.pos.y += this->actor.playerHeightRel;
    player->actor.shape.shadowAlpha = 0;
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_84);
    Player_PlaySfx(player, NA_SE_VO_NAVY_ENEMY);
    this->fallingFrame = 0;
    Play_DisableMotionBlur();
    this->actionFunc = ObjWturn_FallingCs;
}

void ObjWturn_FallingCs(ObjWturn* this, PlayState* play) {
    static Vec3f sSubCamUp = { 0.0f, -1.0f, 0.0f };
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Player* player = GET_PLAYER(play);

    this->fallingFrame++;
    player->actor.world.pos.y = this->actor.world.pos.y + this->fallingFrame * 4.0f;
    Play_SetCameraAtEyeUp(play, this->subCamId, &player->actor.focus.pos, &subCam->eye, &sSubCamUp);
    if (this->fallingFrame == 1) {
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        gSaveContext.nextCutsceneIndex = 0;
        if (play->sceneId == SCENE_F40) {
            play->nextEntrance = ENTRANCE(STONE_TOWER_INVERTED, 0);
        } else {
            play->nextEntrance = ENTRANCE(STONE_TOWER, 1);
        }
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void ObjWturn_Update(Actor* thisx, PlayState* play) {
    ObjWturn* this = (ObjWturn*)thisx;

    this->actionFunc(this, play);
}
