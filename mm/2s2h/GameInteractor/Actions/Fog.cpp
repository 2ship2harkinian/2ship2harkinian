#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// Below 950 the fullscreen fog filter is fully opaque -- a flat colour card, not fog.
#define FOG_NEAR_CLEAREST ENV_FOGNEAR_MAX // 996
#define FOG_NEAR_THICKEST 950

// Rolled once per activation, so the fog settles on a colour rather than strobing.
static u8 sColor[3] = { 128, 128, 128 };

// Written as a delta through adjLightSettings: Environment_UpdateLights rebuilds lightCtx from
// lightSettings + adjLightSettings after the queue ticks, so a direct write would be overwritten.
static GIActions::Register fogAction({
    .id = GI_ACTION_FOG,
    .name = "fog",
    .displayName = "Fog",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // 0 leaves the scene's own draw distance alone; 1 is as thick as fog can get.
            { .name = "intensity", .type = GI_PARAM_FLOAT, .defaultValue = 0.75f, .min = 0.0f, .max = 1.0f },
        },
    .onStart =
        [](GIAction& action) {
            sColor[0] = (u8)Rand_Next();
            sColor[1] = (u8)Rand_Next();
            sColor[2] = (u8)Rand_Next();
        },
    .onTick =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }
            EnvironmentContext* envCtx = &gPlayState->envCtx;

            s16 fogNear = FOG_NEAR_CLEAREST - (s16)((FOG_NEAR_CLEAREST - FOG_NEAR_THICKEST) *
                                                    action.params.Float("intensity"));

            envCtx->adjLightSettings.fogNear = fogNear - envCtx->lightSettings.fogNear;
            for (int i = 0; i < 3; i++) {
                envCtx->adjLightSettings.fogColor[i] = (s16)sColor[i] - envCtx->lightSettings.fogColor[i];
            }
        },
    .onEnd =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }
            EnvironmentContext* envCtx = &gPlayState->envCtx;

            // Zero is the identity for a delta.
            envCtx->adjLightSettings.fogNear = 0;
            for (int i = 0; i < 3; i++) {
                envCtx->adjLightSettings.fogColor[i] = 0;
            }
        },
});
