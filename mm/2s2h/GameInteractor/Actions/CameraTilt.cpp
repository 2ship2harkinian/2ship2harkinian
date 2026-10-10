#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static const uint32_t sRerollInterval = 20 * 2; // 2 seconds
static const s16 sTiltStep = 0x100;             // ~1.4 degrees per frame

static HOOK_ID sCameraHook = 0;
static s16 sTiltTarget = 0;
static s16 sTiltCurrent = 0;

static GIActions::Register cameraTiltAction({
    .id = GI_ACTION_CAMERA_TILT,
    .name = "cameraTilt",
    .displayName = "Camera Tilt",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_CAMERA_ROLL,
    .schema =
        {
            // Degrees either side of upright.
            { .name = "maxTilt", .type = GI_PARAM_FLOAT, .defaultValue = 25.0f, .min = 1.0f, .max = 180.0f },
        },
    .onStart =
        [](GIAction& action) {
            sTiltTarget = 0;
            sTiltCurrent = 0;

            GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterCameraUpdate>(sCameraHook);
            sCameraHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::AfterCameraUpdate>(
                [](Camera* camera) { GIActions::CameraRoll::Apply(camera, sTiltCurrent); });
        },
    .onTick =
        [](GIAction& action) {
            if (action.elapsed % sRerollInterval == 0) {
                sTiltTarget = CAM_DEG_TO_BINANG(Rand_CenteredFloat(action.params.Float("maxTilt") * 2.0f));
            }
            Math_StepToS(&sTiltCurrent, sTiltTarget, sTiltStep);
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterCameraUpdate>(sCameraHook);
            sCameraHook = 0;
        },
});
