#include "Actions.h"

// Squashes Link vertically and spreads him to match, so he reads as a pancake.
static GIActions::Register squishLinkAction({
    .id = GI_ACTION_SQUISH_LINK,
    .name = "squishLink",
    .displayName = "Squish Link",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_PLAYER_SCALE,
    .onTick =
        [](GIAction& action) {
            float intensity = 0.3f;
            float spread = 2.0f - intensity;
            GIActions::PlayerScale::Set(spread, intensity, spread);
        },
    .onEnd = [](GIAction& action) { GIActions::PlayerScale::Reset(); },
});
