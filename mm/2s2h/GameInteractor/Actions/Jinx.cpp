#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

// Instant rather than timed: the game owns the jinxTimer countdown, so a timed action would race it.
static GIActions::Register jinxAction({
    .id = GI_ACTION_JINX,
    .name = "jinx",
    .displayName = "Jinx",
    .valence = GI_VALENCE_NEGATIVE,
    .schema =
        {
            // 20 units per second; the ceiling is what a u16 holds.
            { .name = "length", .type = GI_PARAM_INT, .defaultValue = 20 * 60, .min = 1, .max = 65535 },
        },
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            Actor_PlaySfx(&GET_PLAYER(gPlayState)->actor, NA_SE_EN_BUBLE_BITE);
            gSaveContext.jinxTimer = static_cast<u16>(action.params.Int("length"));
        },
});
