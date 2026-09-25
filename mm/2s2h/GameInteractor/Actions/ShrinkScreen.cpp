#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

// Drives gSaveContext.screenScale/screenScaleFlag, the fields behind the game's own end-of-day
// framebuffer shrink (1000 = full frame, lower = smaller).

// Frames spent easing in at the start and back out at the end.
#define SHRINK_RAMP_FRAMES 15

static GIActions::Register shrinkScreenAction({
    .id = GI_ACTION_SHRINK_SCREEN,
    .name = "shrinkScreen",
    .displayName = "Shrink Screen",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // The fraction of the screen the frame gives up; the 0.9 cap keeps it playable.
            { .name = "intensity", .type = GI_PARAM_FLOAT, .defaultValue = 0.4f, .min = 0.05f, .max = 0.9f },
        },
    .onTick =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }

            float target = 1000.0f - (1000.0f * action.params.Float("intensity"));

            // Ease in, hold, ease back out. duration is guarded before the subtraction so it can't
            // underflow.
            float progress;
            uint32_t elapsed = action.elapsed;
            uint32_t duration = action.duration;
            if (elapsed < SHRINK_RAMP_FRAMES) {
                progress = (float)elapsed / SHRINK_RAMP_FRAMES;
            } else if (duration > 2 * SHRINK_RAMP_FRAMES && elapsed > duration - SHRINK_RAMP_FRAMES) {
                progress = (float)(duration - elapsed) / SHRINK_RAMP_FRAMES;
            } else {
                progress = 1.0f;
            }

            gSaveContext.screenScale = 1000.0f - ((1000.0f - target) * progress);
            gSaveContext.screenScaleFlag = gSaveContext.screenScale < 1000.0f;
        },
    // No play-state guard: gSaveContext is a global, so the write is valid even after a save load.
    .onEnd =
        [](GIAction& action) {
            gSaveContext.screenScaleFlag = false;
            gSaveContext.screenScale = 1000.0f;
        },
});
