#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static const uint32_t sShiftInterval = 20 * 4; // How long one colour takes to fade into the next.
// Not a param: full opacity paints the sky out into a flat colour card rather than tinting it.
static const u8 sFilterAlpha = 100;

static u8 sFrom[3] = { 255, 0, 255 };
static u8 sTo[3] = { 0, 255, 255 };

static void RollColor(u8 color[3]) {
    color[0] = (u8)Rand_Next();
    color[1] = (u8)Rand_Next();
    color[2] = (u8)Rand_Next();
}

// customSkyboxFilter/skyboxFilterColor are a filter the game can draw but never uses, so nothing
// fights over them. Environment_Init clears them per scene load, hence re-applying each tick.
static GIActions::Register randomSkyboxAction({
    .id = GI_ACTION_RANDOM_SKYBOX,
    .name = "randomSkybox",
    .displayName = "Random Skybox",
    .valence = GI_VALENCE_NEUTRAL,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .onStart =
        [](GIAction& action) {
            RollColor(sFrom);
            RollColor(sTo);
        },
    .onTick =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }

            uint32_t phase = action.elapsed % sShiftInterval;
            // elapsed 0 still has onStart's pair.
            if (phase == 0 && action.elapsed > 0) {
                sFrom[0] = sTo[0];
                sFrom[1] = sTo[1];
                sFrom[2] = sTo[2];
                RollColor(sTo);
            }

            float t = (float)phase / sShiftInterval;
            EnvironmentContext* envCtx = &gPlayState->envCtx;

            envCtx->customSkyboxFilter = true;
            for (int i = 0; i < 3; i++) {
                float from = sFrom[i];
                float to = sTo[i];
                envCtx->skyboxFilterColor[i] = (u8)(from + ((to - from) * t));
            }
            envCtx->skyboxFilterColor[3] = sFilterAlpha;
        },
    .onEnd =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }
            gPlayState->envCtx.customSkyboxFilter = false;
        },
});
