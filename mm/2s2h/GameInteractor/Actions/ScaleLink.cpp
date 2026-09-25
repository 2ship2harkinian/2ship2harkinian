#include "Actions.h"

static GIActions::Register scaleLinkAction({
    .id = GI_ACTION_SCALE_LINK,
    .name = "scaleLink",
    .displayName = "Scale Link",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .exclusionGroup = GI_EXCLUSION_PLAYER_SCALE,
    .schema =
        {
            // Above 1.0 makes Link bigger; past the ceiling he stops fitting through doorways.
            { .name = "scale", .type = GI_PARAM_FLOAT, .defaultValue = 0.4f, .min = 0.05f, .max = 3.0f },
        },
    .onTick =
        [](GIAction& action) {
            float scale = action.params.Float("scale");
            GIActions::PlayerScale::Set(scale, scale, scale);
        },
    .onEnd = [](GIAction&) { GIActions::PlayerScale::Reset(); },
});
