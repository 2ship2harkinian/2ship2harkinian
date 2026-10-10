#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

// File statics because REGISTER_VB_SHOULD expands to a captureless lambda.
static float sMultiplier = 1.0f;
static HOOK_ID sWalkHook = 0;
static HOOK_ID sSwimHook = 0;

// Scales the speed *target* before vanilla's stepping; multiplying speedXZ itself would compound.
static GIActions::Register speedAction({
    .id = GI_ACTION_SPEED,
    .name = "speed",
    .displayName = "Speed",
    .valence = GI_VALENCE_NEUTRAL,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // Below 1.0 slows Link down, above speeds him up.
            { .name = "multiplier", .type = GI_PARAM_FLOAT, .required = true, .min = 0.1f, .max = 3.0f },
        },
    .onStart =
        [](GIAction& action) {
            sMultiplier = action.params.Float("multiplier");

            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sWalkHook);
            sWalkHook = REGISTER_VB_SHOULD(VB_SPEED_MODIFIER_WALK, {
                f32* speedTarget = va_arg(args, f32*);
                *speedTarget *= sMultiplier;
            });

            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sSwimHook);
            sSwimHook = REGISTER_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, {
                va_arg(args, f32*); // incrStep
                f32* maxSpeed = va_arg(args, f32*);
                va_arg(args, f32*); // speed
                f32* speedTarget = va_arg(args, f32*);

                *maxSpeed *= sMultiplier;
                *speedTarget *= sMultiplier;
            });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sWalkHook);
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sSwimHook);
            sWalkHook = 0;
            sSwimHook = 0;
        },
});
