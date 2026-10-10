#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static GIActions::Register rupeesAction({
    .id = GI_ACTION_RUPEES,
    .name = "rupees",
    .displayName = "Rupees",
    .valence = GI_VALENCE_NEUTRAL, // Covers both directions, so neither valence is honest.
    .schema =
        {
            // Negative takes rupees away; 999 is the largest wallet.
            { .name = "amount", .type = GI_PARAM_INT, .required = true, .min = -999, .max = 999 },
        },
    .onStart =
        [](GIAction& action) {
            // Rupees_ChangeBy nudges an accumulator that stops on its own at the wallet cap or zero.
            Rupees_ChangeBy((s16)action.params.Int("amount"));
        },
});
