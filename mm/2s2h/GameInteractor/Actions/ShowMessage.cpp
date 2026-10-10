#include "Actions.h"

#include "2s2h/CustomMessage/CustomMessage.h"

static GIActions::Register showMessageAction({
    .id = GI_ACTION_SHOW_MESSAGE,
    .name = "showMessage",
    .displayName = "Show Message",
    .schema =
        {
            { .name = "text", .type = GI_PARAM_STRING, .required = true },
        },
    .onStart = [](GIAction& action) { CustomMessage::StartTextbox(action.params.String("text")); },
});
