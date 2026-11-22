#include "ActorBehavior.h"
#include "CSMC.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
}

// Containers without custom models are drawn with their vanilla models, tinted by the type of item they hold.
// With CSMC off, or when holding junk, they all share one tint so they still stand out.
static Color_RGBA8 CSMC_GetTint(RandoItemType randoItemType) {
    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            return { 90, 140, 255, 255 }; // Blue
        case RITYPE_HEALTH:
            return { 255, 90, 110, 255 }; // Red
        case RITYPE_LESSER:
            return { 230, 140, 70, 255 }; // Orange
        case RITYPE_MAJOR:
            return { 255, 210, 60, 255 }; // Gold
        case RITYPE_MASK:
            return { 190, 110, 255, 255 }; // Purple
        case RITYPE_SKULLTULA_TOKEN:
            return { 120, 235, 120, 255 }; // Green
        case RITYPE_SMALL_KEY:
            return { 170, 225, 255, 255 }; // Light Blue
        case RITYPE_STRAY_FAIRY:
            return { 255, 150, 230, 255 }; // Pink
        default:
            return { 200, 240, 255, 255 }; // Pale Blue
    }
}

RandoItemType CSMC_GetTintType(RandoCheckId randoCheckId) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        return RITYPE_JUNK;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    return Rando::StaticData::Items[randoItemId].randoItemType;
}

void CSMC_BeginTint(Gfx** gfx, RandoItemType randoItemType) {
    Color_RGBA8 tint = CSMC_GetTint(randoItemType);
    gDPSetGrayscaleColor((*gfx)++, tint.r, tint.g, tint.b, tint.a);
    gSPGrayscale((*gfx)++, true);
}

void CSMC_EndTint(Gfx** gfx) {
    gSPGrayscale((*gfx)++, false);
}

void CSMC_DrawActorOpa(Actor* actor, PlayState* play, const char* dList, ActorFunc vanillaDraw) {
    if (!IS_MISSING_ASSET(dList)) {
        Gfx_DrawDListOpa(play, (Gfx*)dList);
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);
    CSMC_BeginTint(&POLY_OPA_DISP, CSMC_GetTintType(Rando::ActorBehavior::GetObjectRandoCheckId(actor)));
    vanillaDraw(actor, play);
    CSMC_EndTint(&POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx);
}

void CSMC_DrawDListOpa(PlayState* play, RandoCheckId randoCheckId, const char* dList, const char* vanillaDList) {
    if (!IS_MISSING_ASSET(dList)) {
        Gfx_DrawDListOpa(play, (Gfx*)dList);
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);
    CSMC_BeginTint(&POLY_OPA_DISP, CSMC_GetTintType(randoCheckId));
    Gfx_DrawDListOpa(play, (Gfx*)vanillaDList);
    CSMC_EndTint(&POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx);
}
