#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// PLAYER_STATE3_400000 rather than invincibilityTimer: the timer caps at 127 frames and flashes
// Link red; the flag is the same early-out with neither.
static GIActions::Register invincibilityAction({
    .id = GI_ACTION_INVINCIBILITY,
    .name = "invincibility",
    .displayName = "Invincibility",
    .valence = GI_VALENCE_POSITIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    // Re-applied every tick so it survives the player being rebuilt by a transition.
    .onTick =
        [](GIAction&) {
            if (Player* player = GIActions::PlayerOrNull()) {
                player->stateFlags3 |= PLAYER_STATE3_400000;
            }
        },
    .onEnd =
        [](GIAction&) {
            if (Player* player = GIActions::PlayerOrNull()) {
                player->stateFlags3 &= ~PLAYER_STATE3_400000;
            }
        },
});
