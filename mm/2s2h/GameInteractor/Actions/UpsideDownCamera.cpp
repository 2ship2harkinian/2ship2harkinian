#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

static HOOK_ID sCameraHook = 0;

// Half a turn of roll, which lands the up vector exactly negated.
#define UPSIDE_DOWN_ROLL 0x8000

// A rotation, not a reflection, so there's no culling to invert; the HUD stays upright.
static GIActions::Register upsideDownCameraAction({
    .id = GI_ACTION_UPSIDE_DOWN_CAMERA,
    .name = "upsideDownCamera",
    .displayName = "Upside Down Camera",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_CAMERA_ROLL,
    .onStart =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterCameraUpdate>(sCameraHook);
            sCameraHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::AfterCameraUpdate>(
                [](Camera* camera) { GIActions::CameraRoll::Apply(camera, UPSIDE_DOWN_ROLL); });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterCameraUpdate>(sCameraHook);
            sCameraHook = 0;
        },
});
