#include "Actions.h"

// Visual only -- collision comes from the player's cylinder, not actor.scale.
static GIActions::Register paperLinkAction({
    .id = GI_ACTION_PAPER_LINK,
    .name = "paperLink",
    .displayName = "Paper Link",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_PLAYER_SCALE,
    .onTick = [](GIAction& action) { GIActions::PlayerScale::Set(0.1f, 1.0f, 1.0f); },
    .onEnd = [](GIAction& action) { GIActions::PlayerScale::Reset(); },
});
