#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// The engine counts health in sixteenths of a heart; the param is in hearts.
#define HEALTH_PER_HEART 16.0f

static GIActions::Register healthAction({
    .id = GI_ACTION_HEALTH,
    .name = "health",
    .displayName = "Health",
    .valence = GI_VALENCE_NEUTRAL, // Covers both directions, so neither valence is honest.
    .schema =
        {
            // Negative takes hearts away. 20 is the most the game allows.
            { .name = "amount", .type = GI_PARAM_FLOAT, .required = true, .min = -20.0f, .max = 20.0f },
        },
    .onStart =
        [](GIAction& action) {
            // Health_ChangeBy applies the game's own rules: clamping, Double Defense, death at zero.
            Health_ChangeBy(gPlayState, (s16)(action.params.Float("amount") * HEALTH_PER_HEART));
        },
});
