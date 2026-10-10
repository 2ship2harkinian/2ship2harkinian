#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static GIActions::Register transitionAction({
    .id = GI_ACTION_TRANSITION,
    .name = "transition",
    .displayName = "Teleport",
    .schema =
        {
            { .name = "entrance", .type = GI_PARAM_INT, .required = true, .min = 0.0f, .max = 65535.0f },
            { .name = "cutsceneIndex", .type = GI_PARAM_INT },
            { .name = "transitionTrigger", .type = GI_PARAM_INT, .defaultValue = (int32_t)TRANS_TRIGGER_START },
            { .name = "transitionType", .type = GI_PARAM_INT, .defaultValue = (int32_t)TRANS_TYPE_FADE_BLACK },
        },
    .onStart =
        [](GIAction& action) {
            gPlayState->nextEntrance = (u16)action.params.Int("entrance");
            gSaveContext.nextCutsceneIndex = (u16)action.params.Int("cutsceneIndex");
            gPlayState->transitionTrigger = (s8)action.params.Int("transitionTrigger");
            gPlayState->transitionType = (u8)action.params.Int("transitionType");
        },
});

GIAction GIActions::Transition(GIActions::TransitionArgs args) {
    const GIActions::Definition* definition = GIActions::Get(GI_ACTION_TRANSITION);

    // Every field is typed at the call site and every value fits its spec, so this can't fail.
    return definition
        ->Build({
            { "entrance", (int32_t)args.entrance },
            { "cutsceneIndex", (int32_t)args.cutsceneIndex },
            { "transitionTrigger", (int32_t)args.transitionTrigger },
            { "transitionType", (int32_t)args.transitionType },
        })
        .value_or(GIAction{});
}
