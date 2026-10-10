#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// How long the wind holds a direction before picking a new one.
static const uint32_t sRerollInterval = 20 * 2; // 2 seconds
static s16 sWindYaw = 0;

// windSpeed/windAngleX/windAngleY are the fields Obj_Wind uses, so the player's own decay and
// stagger reactions come free.
static GIActions::Register randomWindAction({
    .id = GI_ACTION_RANDOM_WIND,
    .name = "randomWind",
    .displayName = "Random Wind",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // Scaled by form on the way in: Deku catches the most wind at 1.5x, Goron the least at 0.6x.
            { .name = "strength", .type = GI_PARAM_FLOAT, .defaultValue = 6.0f, .min = 1.0f, .max = 20.0f },
        },
    .canApply = GIActions::Gates::NotOnHorse,
    .onTick =
        [](GIAction& action) {
            Player* player = GIActions::PlayerOrNull();
            if (player == NULL) {
                return;
            }

            if (action.elapsed % sRerollInterval == 0) {
                sWindYaw = (s16)Rand_Next();
            }

            player->windSpeed = action.params.Float("strength");
            player->windAngleX = 0; // Pitch: level with the ground. 0x4000 would blow straight up.
            player->windAngleY = sWindYaw;
        },
    .onEnd =
        [](GIAction&) {
            if (Player* player = GIActions::PlayerOrNull()) {
                player->windSpeed = 0.0f;
            }
        },
});
