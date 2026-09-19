#include "Actions.h"

#include <libultraship/bridge/consolevariablebridge.h>

// The state cvar behind Mirrored World mode; read every frame, so flipping it takes effect immediately.
#define CVAR_MIRRORED_WORLD_STATE "gModes.MirroredWorld.State"

// Restored rather than cleared: a player running Mirrored World mode may already have it on.
static int32_t sPreviousState = GIActions::Setting::ABSENT;

// Reusing Mirrored World also gets inverted culling, the map, and flipped X controls for free.
static GIActions::Register mirrorCameraAction({
    .id = GI_ACTION_MIRROR_CAMERA,
    .name = "mirrorCamera",
    .displayName = "Mirror Camera",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .onStart =
        [](GIAction&) {
            sPreviousState = GIActions::Setting::Snapshot(CVAR_MIRRORED_WORLD_STATE);
            CVarSetInteger(CVAR_MIRRORED_WORLD_STATE, 1);
        },
    .onEnd = [](GIAction&) { GIActions::Setting::Restore(CVAR_MIRRORED_WORLD_STATE, sPreviousState); },
});
