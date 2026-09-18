#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
#include "regs.h"
}

// Restored rather than blanket-disabled: the game turns blur on itself for Goht and other set pieces.
static s32 sPreviousEnabled = 0;
static s32 sPreviousAlpha = 0;

static GIActions::Register motionBlurAction({
    .id = GI_ACTION_MOTION_BLUR,
    .name = "motionBlur",
    .displayName = "Motion Blur",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // How much of the previous frame is kept; the game's own uses sit between 60 and 150.
            { .name = "strength", .type = GI_PARAM_INT, .defaultValue = 180, .min = 1, .max = 255 },
        },
    .onStart =
        [](GIAction& action) {
            sPreviousEnabled = R_MOTION_BLUR_ENABLED;
            sPreviousAlpha = R_MOTION_BLUR_ALPHA;
        },
    // Re-applied every tick: scene loads reset these registers.
    .onTick = [](GIAction& action) { Play_EnableMotionBlur(action.params.Int("strength")); },
    .onEnd =
        [](GIAction& action) {
            R_MOTION_BLUR_ALPHA = sPreviousAlpha;
            R_MOTION_BLUR_ENABLED = sPreviousEnabled;
        },
});
