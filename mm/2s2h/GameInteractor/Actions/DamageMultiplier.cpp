#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

// File static because REGISTER_VB_SHOULD expands to a captureless lambda.
static float sMultiplier = 2.0f;
static HOOK_ID sDamageHook = 0;

static GIActions::Register damageMultiplierAction({
    .id = GI_ACTION_DAMAGE_MULTIPLIER,
    .name = "damageMultiplier",
    .displayName = "Damage Multiplier",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            { .name = "multiplier", .type = GI_PARAM_FLOAT, .defaultValue = 2.0f, .min = 1.0f, .max = 8.0f },
        },
    .onStart =
        [](GIAction& action) {
            sMultiplier = action.params.Float("multiplier");

            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sDamageHook);
            sDamageHook = REGISTER_VB_SHOULD(VB_MULTIPLY_INFLICTED_DMG, {
                // `should` is left alone: it carries Giant's Mask quarter-damage, which composes on top.
                s32* damage = va_arg(args, s32*);
                *damage = (s32)(*damage * sMultiplier);
            });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sDamageHook);
            sDamageHook = 0;
        },
});
