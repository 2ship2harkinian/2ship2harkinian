#include "Actions.h"

#include "2s2h_assets.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// Defined in Enhancements/Equipment/BombArrows.cpp, and reached the same way ItemUnequip.cpp and
// ArrowCycle.cpp reach it.
extern void SetBombArrowButton(s32 slot, bool state, bool isDpad);

// Form index 0 rather than CUR_FORM throughout: only EQUIP_SLOT_B is per-form. The D-pad half is
// cleared even with its equips off -- harmless, and it leaves nothing stale.
static GIActions::Register clearButtonsAction({
    .id = GI_ACTION_CLEAR_BUTTONS,
    .name = "clearButtons",
    .displayName = "Clear Buttons",
    .valence = GI_VALENCE_NEGATIVE,
    .onStart =
        [](GIAction& action) {
            for (s32 slot = EQUIP_SLOT_C_LEFT; slot <= EQUIP_SLOT_C_RIGHT; slot++) {
                BUTTON_ITEM_EQUIP(0, slot) = ITEM_NONE;
                C_SLOT_EQUIP(0, slot) = SLOT_NONE;
                Interface_LoadItemIconImpl(gPlayState, slot);
                SetBombArrowButton(slot, false, false);
            }

            for (s32 slot = EQUIP_SLOT_D_RIGHT; slot <= EQUIP_SLOT_D_UP; slot++) {
                DPAD_BUTTON_ITEM_EQUIP(0, slot) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, slot) = SLOT_NONE;
                // Interface_Dpad_LoadItemIconImpl's clear path writes over a C button's icon.
                gPlayState->interfaceCtx.iconItemSegment[DPAD_BUTTON(slot) + EQUIP_SLOT_MAX] = (char*)gEmptyTexture;
                SetBombArrowButton(slot, false, true);
            }

            Audio_PlaySfx(NA_SE_SY_DECIDE);
        },
});
