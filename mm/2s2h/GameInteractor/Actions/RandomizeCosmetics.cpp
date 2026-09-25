#include "Actions.h"

// Not declared in CosmeticEditor.h, so there's nothing to include.
void CosmeticEditorRandomizeAllElements();

// Persistent rather than timed: it writes the same cvars the Cosmetics Editor does; there's no undo.
static GIActions::Register randomizeCosmeticsAction({
    .id = GI_ACTION_RANDOMIZE_COSMETICS,
    .name = "randomizeCosmetics",
    .displayName = "Randomize Cosmetics",
    .valence = GI_VALENCE_NEUTRAL,
    .onStart = [](GIAction& action) { CosmeticEditorRandomizeAllElements(); },
});
