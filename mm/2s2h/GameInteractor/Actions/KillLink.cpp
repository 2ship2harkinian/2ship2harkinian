#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

static GIActions::Register killLinkAction({
    .id = GI_ACTION_KILL_LINK,
    .name = "killLink",
    .displayName = "Kill Link",
    .valence = GI_VALENCE_NEGATIVE,
    .onStart =
        [](GIAction& action) {
            // Zeroing health rather than dealing damage, which invincibility would swallow.
            gSaveContext.save.saveInfo.playerData.health = 0;
        },
});
