/*
 * File: z_bg_iknv_doukutu.c
 * Overlay: ovl_Bg_Iknv_Doukutu
 * Description: Sharp's Cave
 */

#include "z_bg_iknv_doukutu.h"
#include "objects/object_iknv_obj/object_iknv_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgIknvDoukutu_Init(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Destroy(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Update(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Draw(Actor* thisx, PlayState* play);

void BgIknvDoukutu_Cleanse(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_Cursed(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_FillSpring(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_FadeInWater(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_Dry(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_DoNothing(BgIknvDoukutu* this, PlayState* play);
void BgIknvDoukutu_DrawPurified(Actor* thisx, PlayState* play);
void BgIknvDoukutu_DrawStream(Actor* thisx, PlayState* play);
void BgIknvDoukutu_DrawSpringWater(Actor* thisx, PlayState* play);
void BgIknvDoukutu_DrawCleansing(Actor* thisx, PlayState* play);

ActorProfile Bg_Iknv_Doukutu_Profile = {
    /**/ ACTOR_BG_IKNV_DOUKUTU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKNV_OBJ,
    /**/ sizeof(BgIknvDoukutu),
    /**/ BgIknvDoukutu_Init,
    /**/ BgIknvDoukutu_Destroy,
    /**/ BgIknvDoukutu_Update,
    /**/ BgIknvDoukutu_Draw,
};

void BgIknvDoukutu_Init(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->actionFunc = BgIknvDoukutu_DoNothing;
    this->unk_15C = 0;

    switch (BGIKNVDOUKUTU_GET_TYPE(&this->dyna.actor)) {
        case BGIKNVDOUKUTU_INTERIOR:
            this->actionFunc = BgIknvDoukutu_Cursed;
            this->cueType = CS_CMD_ACTOR_CUE_516;
            this->opacity = 1.0f;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_IKANA_SPRING_RESTORED) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
                this->dyna.actor.draw = BgIknvDoukutu_DrawPurified;
                this->actionFunc = BgIknvDoukutu_DoNothing;
                play->envCtx.lightSettingOverride = 25;
            } else {
                play->envCtx.lightSettingOverride = 24;
            }
            break;

        case BGIKNVDOUKUTU_STREAM:
            Actor_SetScale(&this->dyna.actor, 1.0f);
            this->dyna.actor.draw = BgIknvDoukutu_DrawStream;
            this->cueType = CS_CMD_ACTOR_CUE_516;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&gIkanaStreamCol, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_IKANA_SPRING_RESTORED)) {
                Actor_Kill(&this->dyna.actor);
            }
            break;

        case BGIKNVDOUKUTU_SPRING_WATER:
            this->cueType = CS_CMD_ACTOR_CUE_516;
            this->dyna.actor.draw = BgIknvDoukutu_DrawSpringWater;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&gIkanaCaveSpringWaterCol, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_IKANA_SPRING_RESTORED)) {
                this->opacity = 1.0f;
                this->dyna.actor.world.pos.y += 68.0f;
            } else {
                this->actionFunc = BgIknvDoukutu_Dry;
                this->opacity = 0.0f;
            }
            break;

        default:
            Actor_Kill(&this->dyna.actor);
            break;
    }
}

void BgIknvDoukutu_Destroy(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    if ((BGIKNVDOUKUTU_GET_TYPE(&this->dyna.actor) == BGIKNVDOUKUTU_STREAM) ||
        (BGIKNVDOUKUTU_GET_TYPE(&this->dyna.actor) == BGIKNVDOUKUTU_SPRING_WATER)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgIknvDoukutu_Cleanse(BgIknvDoukutu* this, PlayState* play) {
    if (this->opacity >= 0.05f) {
        this->opacity -= 0.05f;
    } else {
        this->actionFunc = BgIknvDoukutu_DoNothing;
        this->dyna.actor.draw = BgIknvDoukutu_DrawPurified;
    }
    play->envCtx.lightSettingOverride = 25;
}

void BgIknvDoukutu_Cursed(BgIknvDoukutu* this, PlayState* play) {
    play->envCtx.lightSettingOverride = 24;
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 2)) {
        this->actionFunc = BgIknvDoukutu_Cleanse;
        this->dyna.actor.draw = BgIknvDoukutu_DrawCleansing;
    }
}

void BgIknvDoukutu_FillSpring(BgIknvDoukutu* this, PlayState* play) {
    f32 waterLevelMax = this->dyna.actor.home.pos.y + 68.0f;

    this->dyna.actor.world.pos.y += 1.7f;
    if (waterLevelMax < this->dyna.actor.world.pos.y) {
        this->dyna.actor.world.pos.y = waterLevelMax;
        this->actionFunc = BgIknvDoukutu_DoNothing;
    }
    Audio_PlaySfx_2(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
}

void BgIknvDoukutu_FadeInWater(BgIknvDoukutu* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 3)) {
        this->actionFunc = BgIknvDoukutu_FillSpring;
    }

    if (this->opacity < 0.95f) {
        this->opacity += 0.05f;
    }
}

void BgIknvDoukutu_Dry(BgIknvDoukutu* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 2)) {
        this->actionFunc = BgIknvDoukutu_FadeInWater;
    }
}

void BgIknvDoukutu_DoNothing(BgIknvDoukutu* this, PlayState* play) {
}

void BgIknvDoukutu_Update(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    this->actionFunc(this, play);
}

void BgIknvDoukutu_Draw(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gIkanaCaveInteriorTexAnim));
    Scene_SetRenderModeXlu(play, 0, 1);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPDisplayList(POLY_OPA_DISP++, gIkanaCaveCursedDL);
    gDPSetEnvColor(POLY_XLU_DISP++, 215, 42, 55, 120);
    gSPDisplayList(POLY_XLU_DISP++, gIkanaCaveFogDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgIknvDoukutu_DrawCleansing(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    GraphicsContext* gfxCtx;
    f32 opacity;

    BgIknvDoukutu_DrawPurified(&this->dyna.actor, play);

    gfxCtx = play->state.gfxCtx;
    OPEN_DISPS(gfxCtx);
    Vec3f translationVec;
    s16 cameraYaw = Math_Vec3f_Yaw(&GET_ACTIVE_CAM(play)->eye, &GET_ACTIVE_CAM(play)->at);
    s16 cameraPitch = -Math_Vec3f_Pitch(&GET_ACTIVE_CAM(play)->eye, &GET_ACTIVE_CAM(play)->at);

    translationVec.x = -100.0f * Math_SinS(cameraYaw) * Math_CosS(cameraPitch);
    translationVec.y = Math_SinS(cameraPitch) * -100.0f;
    translationVec.z = -100.0f * Math_CosS(cameraYaw) * Math_CosS(cameraPitch);
    Matrix_Translate(translationVec.x, translationVec.y, translationVec.z, MTXMODE_APPLY);
    opacity = this->opacity;
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gIkanaCaveInteriorTexAnim));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL72(POLY_XLU_DISP++);
    Scene_SetRenderModeXlu(play, 1, 2);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (s32)(255.0f * opacity));
    gSPDisplayList(POLY_XLU_DISP++, gIkanaCaveCursedDL);
    gDPSetEnvColor(POLY_XLU_DISP++, 215, 42, 55, (s32)(120.0f * opacity));
    gSPDisplayList(POLY_XLU_DISP++, gIkanaCaveFogDL);

    CLOSE_DISPS(gfxCtx);
}

void BgIknvDoukutu_DrawPurified(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Scene_SetRenderModeXlu(play, 0, 1);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPDisplayList(POLY_OPA_DISP++, gIkanaCavePurifiedDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgIknvDoukutu_DrawStream(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gIkanaStreamTexAnim));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gIkanaStreamDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgIknvDoukutu_DrawSpringWater(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    f32 opacity = this->opacity;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gIkanaCaveSpringWaterTexAnim));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (s32)(140.0f * opacity));

    gSPDisplayList(POLY_XLU_DISP++, gIkanaCaveSpringWaterDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
