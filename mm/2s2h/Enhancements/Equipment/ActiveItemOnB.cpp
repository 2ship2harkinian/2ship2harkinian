#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64interface.h"
#include "variables.h"

void Player_UseItem(PlayState* play, Player* player, ItemId item);
}

constexpr bool IsFirstPersonItem(ItemId item) {
    return item != ITEM_NONE && (item == ITEM_BOW || item == ITEM_BOW_FIRE || item == ITEM_BOW_ICE ||
                                 item == ITEM_BOW_LIGHT || item == ITEM_HOOKSHOT);
}

constexpr bool IsThirdPersonItem(ItemId item) {
    return item != ITEM_NONE && (item == ITEM_DEKU_STICK || item == ITEM_SWORD_GREAT_FAIRY);
}

constexpr bool IsItemInScope(ItemId item) {
    return item != ITEM_NONE && (IsFirstPersonItem(item) || IsThirdPersonItem(item));
}

constexpr bool PlayerHoldsItem(Player* player) {
    return (player->heldItemAction != PLAYER_IA_NONE && (ItemId)player->heldItemId != ITEM_NONE);
}

constexpr bool IsAiming(Player* player) {
    return (player->unk_AA5 == PLAYER_UNKAA5_3 ||
            ( // overshoulder
                player->unk_AA5 == PLAYER_UNKAA5_0 && (player->stateFlags1 & PLAYER_STATE1_PARALLEL) &&
                player->focusActor == NULL));
}

constexpr bool IsHoldingScoped(Player* player) {
    return (PlayerHoldsItem(player) && (IsThirdPersonItem((ItemId)player->heldItemId) ||
                                        (IsFirstPersonItem((ItemId)player->heldItemId) && IsAiming(player))));
}

// allows to temporary set used item to B button, so camera works correctly
static struct ButtonState {
    ItemId stored = ITEM_NONE;
    ItemId override = ITEM_NONE;
    bool frameOverridden = false;
} mBButtonState;

static void HandleGetItemOnButton(bool* should, EquipSlot slot, ItemId* pressedItem) {
    Player* player = GET_PLAYER(gPlayState);

    if (player->transformation != PLAYER_FORM_HUMAN) {
        return;
    }
    if (player->currentMask == PLAYER_MASK_BLAST || player->currentMask == PLAYER_MASK_BREMEN ||
        player->currentMask == PLAYER_MASK_KAMARO) {
        return;
    }

    ItemId heldItem = (ItemId)player->heldItemId;

    if (slot == EQUIP_SLOT_B) {
        if (IsHoldingScoped(player)) {
            // fixes camera
            if (IsFirstPersonItem(heldItem) && IsAiming(player)) {
                ItemId current = (ItemId)BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B);
                if (current != ITEM_NONE) {
                    if (!IsItemInScope(current)) {
                        mBButtonState.stored = current;
                    }
                    mBButtonState.frameOverridden = true;
                    mBButtonState.override = heldItem;
                    BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) = mBButtonState.override;
                }
            }

            // actually shoots
            *pressedItem = heldItem;
        }
    } else if (IsItemInScope(*pressedItem)) {
        if (IsHoldingScoped(player) && heldItem == *pressedItem) {
            // put away
            if (player->heldItemAction > PLAYER_IA_LAST_USED) {
                Player_UseItem(gPlayState, player, ITEM_NONE);
            }
            *pressedItem = ITEM_NONE;
        }
    }
}

// keeps EQUIP_SLOT_B integrity
void RestoreBButtonItem(Actor* actor) {
    if (!mBButtonState.frameOverridden) {
        return;
    }
    // assert(mBButtonState.stored != ITEM_NONE);
    auto* item = &BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B);

    if (*item != mBButtonState.override && !IsItemInScope((ItemId)*item)) {
        mBButtonState.stored = (ItemId)*item;
    } else {
        *item = mBButtonState.stored;
    }
    mBButtonState.frameOverridden = false;
    mBButtonState.override = ITEM_NONE;
}

void CleanupBButtonSlot() {
    if (mBButtonState.stored != ITEM_NONE && !CVarGetInteger("gEnhancements.Equipment.ActiveItemOnB", 0)) {
        RestoreBButtonItem(nullptr);
        mBButtonState.stored = ITEM_NONE;
    }
}

void RegisterActiveItemOnB() {
    CleanupBButtonSlot();
    COND_VB_SHOULD(VB_GET_ITEM_ON_BUTTON, CVarGetInteger("gEnhancements.Equipment.ActiveItemOnB", 0), {
        EquipSlot slot = (EquipSlot)va_arg(args, int);
        ItemId* item = va_arg(args, ItemId*);
        HandleGetItemOnButton(should, slot, item);
    });
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVarGetInteger("gEnhancements.Equipment.ActiveItemOnB", 0),
                 RestoreBButtonItem);
}

static RegisterShipInitFunc initFunc(RegisterActiveItemOnB, { "gEnhancements.Equipment.ActiveItemOnB" });
