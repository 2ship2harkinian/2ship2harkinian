#include "Actions.h"

#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static HOOK_ID sHudRestoreHook = 0;

// Message_DisplaySceneTitleCard hides the HUD but never restores it -- En_Test4 does that itself.
static void RestoreHudWhenCardEnds() {
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameStateUpdate>(sHudRestoreHook);
    sHudRestoreHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateUpdate>([]() {
        // A transition tears the message down without ever reaching MSGMODE_NONE.
        if (gPlayState != NULL && gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
            return;
        }

        // Interface_SetHudVisibility ignores a no-op change, so the idle assignment is what makes it take.
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameStateUpdate>(sHudRestoreHook);
        sHudRestoreHook = 0;
    });
}

// The card is addressed by textId, so CUSTOM_MESSAGE_ID routes it through CustomMessage. The id
// must sit outside 0x1BB2..0x1BB6, the Dawn/Night cards' taller layout.
static GIActions::Register showTitleCardAction({
    .id = GI_ACTION_SHOW_TITLE_CARD,
    .name = "showTitleCard",
    .displayName = "Show Title Card",
    .schema =
        {
            { .name = "text", .type = GI_PARAM_STRING, .required = true },
        },
    .onStart =
        [](GIAction& action) {
            CustomMessage::SetActiveCustomMessage(action.params.String("text"));
            Message_DisplaySceneTitleCard(gPlayState, CUSTOM_MESSAGE_ID);
            RestoreHudWhenCardEnds();
        },
});
