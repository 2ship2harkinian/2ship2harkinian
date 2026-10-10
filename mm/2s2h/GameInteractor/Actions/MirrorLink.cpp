#include "Actions.h"

#include <libultraship/bridge/consolevariablebridge.h>

// OPEN_DISPS/CLOSE_DISPS expand into this; without it they don't link from C++.
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static HOOK_ID sBeforeDrawHook = 0;
static HOOK_ID sAfterDrawHook = 0;

// A negative scale reverses triangle winding, so culling has to be inverted back for Link's draw.
// Toggled against Mirrored World, which may already have the whole scene inverted.
static void InvertPlayerCulling(bool duringPlayer) {
    if (gPlayState == NULL) {
        return;
    }

    bool worldMirrored = CVarGetInteger("gModes.MirroredWorld.State", 0);
    bool invert = duringPlayer ? !worldMirrored : worldMirrored;

    OPEN_DISPS(gPlayState->state.gfxCtx);

    if (invert) {
        gSPSetExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
        gSPSetExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
    } else {
        gSPClearExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
        gSPClearExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
    }

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

// Visual only -- collision comes from the player's cylinder, not actor.scale.
static GIActions::Register mirrorLinkAction({
    .id = GI_ACTION_MIRROR_LINK,
    .name = "mirrorLink",
    .displayName = "Mirror Link",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_PLAYER_SCALE,
    // The ShouldActorDraw/OnActorDraw pair brackets exactly the player's own display list.
    .onStart =
        [](GIAction&) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldActorDraw>(sBeforeDrawHook);
            sBeforeDrawHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorDraw>(
                ACTOR_PLAYER, [](Actor* actor, bool* should) { InvertPlayerCulling(true); });

            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorDraw>(sAfterDrawHook);
            sAfterDrawHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorDraw>(
                ACTOR_PLAYER, [](Actor* actor) { InvertPlayerCulling(false); });
        },
    .onTick = [](GIAction&) { GIActions::PlayerScale::Set(-1.0f, 1.0f, 1.0f); },
    .onEnd =
        [](GIAction&) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldActorDraw>(sBeforeDrawHook);
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorDraw>(sAfterDrawHook);
            sBeforeDrawHook = 0;
            sAfterDrawHook = 0;
            GIActions::PlayerScale::Reset();
        },
});
