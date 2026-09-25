#include "Actions.h"

#include "2s2h_assets.h"

#include <libultraship/bridge/consolevariablebridge.h>
#include <utility>

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
}

// Defined in Enhancements/Equipment/BombArrows.cpp, and reached the same way ItemUnequip.cpp and
// ArrowCycle.cpp reach them.
extern bool IsBombArrowButton(s32 slot, bool isDpad);
extern void SetBombArrowButton(s32 slot, bool state, bool isDpad);

// With D-pad equips off, an item shuffled onto the D-pad would vanish from the player's buttons.
#define CVAR_DPAD_EQUIPS "gEnhancements.Dpad.DpadEquips"

struct ButtonSlot {
    s32 slot;
    bool isDpad;
};

static const ButtonSlot sSlots[] = {
    { EQUIP_SLOT_C_LEFT, false }, { EQUIP_SLOT_C_DOWN, false }, { EQUIP_SLOT_C_RIGHT, false },
    { EQUIP_SLOT_D_RIGHT, true }, { EQUIP_SLOT_D_LEFT, true },  { EQUIP_SLOT_D_DOWN, true },
    { EQUIP_SLOT_D_UP, true },
};
static const size_t sCButtonCount = 3;

struct Assignment {
    u8 item;
    u8 invSlot;
    bool bombArrow;
};

// Form index 0 rather than CUR_FORM: only EQUIP_SLOT_B is per-form.
static Assignment ReadSlot(const ButtonSlot& button) {
    if (button.isDpad) {
        return { DPAD_BUTTON_ITEM_EQUIP(0, button.slot), DPAD_SLOT_EQUIP(0, button.slot),
                 IsBombArrowButton(button.slot, true) };
    }
    return { BUTTON_ITEM_EQUIP(0, button.slot), C_SLOT_EQUIP(0, button.slot), IsBombArrowButton(button.slot, false) };
}

static void WriteSlot(const ButtonSlot& button, const Assignment& assignment) {
    if (button.isDpad) {
        DPAD_BUTTON_ITEM_EQUIP(0, button.slot) = assignment.item;
        DPAD_SLOT_EQUIP(0, button.slot) = assignment.invSlot;

        if (assignment.item < ARRAY_COUNT(gItemIcons)) {
            Interface_Dpad_LoadItemIconImpl(gPlayState, (u8)button.slot);
        } else {
            // That function's clear path writes over a C button's icon; only its set path can be trusted.
            gPlayState->interfaceCtx.iconItemSegment[DPAD_BUTTON(button.slot) + EQUIP_SLOT_MAX] = (char*)gEmptyTexture;
        }
    } else {
        BUTTON_ITEM_EQUIP(0, button.slot) = assignment.item;
        C_SLOT_EQUIP(0, button.slot) = assignment.invSlot;
        Interface_LoadItemIconImpl(gPlayState, (u8)button.slot);
    }

    SetBombArrowButton(button.slot, assignment.bombArrow, button.isDpad);
}

// Permutes whatever is on the buttons, C and D-pad together, so items cross between the two.
static void ShuffleButtons() {
    size_t count = CVarGetInteger(CVAR_DPAD_EQUIPS, 0) ? ARRAY_COUNT(sSlots) : sCButtonCount;
    Assignment assignments[ARRAY_COUNT(sSlots)];

    for (size_t i = 0; i < count; i++) {
        assignments[i] = ReadSlot(sSlots[i]);
    }

    // Fisher-Yates; an identity shuffle is intentionally possible.
    for (size_t i = count - 1; i > 0; i--) {
        std::swap(assignments[i], assignments[Rand_Next() % (i + 1)]);
    }

    for (size_t i = 0; i < count; i++) {
        WriteSlot(sSlots[i], assignments[i]);
    }
}

// The last shuffle deliberately sticks -- nothing is lost, and an onEnd restore would fight
// clearButtons run mid-roulette.
static GIActions::Register buttonRouletteAction({
    .id = GI_ACTION_BUTTON_ROULETTE,
    .name = "buttonRoulette",
    .displayName = "Button Roulette",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .onTick =
        [](GIAction& action) {
            if (gPlayState == NULL) {
                return;
            }
            // ~3s between shuffles; elapsed is 0 on the first tick, so the first lands immediately.
            if (action.elapsed % (20 * 3) == 0) {
                ShuffleButtons();
            }
        },
});
