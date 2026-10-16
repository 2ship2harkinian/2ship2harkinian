#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Equipment.ActiveItemOnB"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

extern "C" {
#include "z64interface.h"
#include "variables.h"

void Player_UseItem(PlayState* play, Player* player, ItemId item);
s32 Player_UpperAction_7(Player* thisx, PlayState* play);
s32 Player_UpperAction_8(Player* thisx, PlayState* play);

EquipSlot func_8082FDC4(void);
DpadEquipSlot func_Dpad_8082FDC4(void);
}

#define CLICKED_BUTTON ((s32)func_8082FDC4())
#define CLICKED_DPAD_BUTTON ((s32)func_Dpad_8082FDC4())

void Player_PutAway(Player* player) {
    if (player->heldItemAction > PLAYER_IA_LAST_USED) {
        Player_UseItem(gPlayState, player, ITEM_NONE);
    }
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
    // z-target check (including overshoulder, excluding target lock)
    bool aimingBox = (player->unk_AA5 == PLAYER_UNKAA5_3);
    bool transitionState = (player->unk_AA5 == PLAYER_UNKAA5_0);
    bool itemDrawnInFirstPerson = (player->upperActionFunc == Player_UpperAction_7);
    bool itemFire = (player->upperActionFunc == Player_UpperAction_8);
    bool targetLock = (player->focusActor != NULL);
    return ((aimingBox || (transitionState && (itemDrawnInFirstPerson || itemFire))) && !targetLock);
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
    char* iconOverride = NULL;
} mBButtonState;

static bool IsHeldItemClicked(ItemId heldItem) {
    s32 but = CLICKED_BUTTON;
    if (but > EQUIP_SLOT_NONE && but < EQUIP_SLOT_MAX && (ItemId)C_BTN_ITEM(but) == heldItem) {
        return true;
    }

    s32 dpad = CLICKED_DPAD_BUTTON;
    if (dpad > EQUIP_SLOT_D_NONE && dpad < EQUIP_SLOT_D_MAX && (ItemId)DPAD_BTN_ITEM(dpad) == heldItem) {
        return true;
    }

    return false;
}

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
            // forces it to wait for exact click, not button hold
            if (IsHeldItemClicked(heldItem)) {
                Player_PutAway(player);
            }
            *pressedItem = ITEM_NONE;
        }
    }
}

void UpdateBButtonView(Player* player) {
    if (IsHoldingScoped(player)) {
        char* icon = (char*)gItemIcons[(ItemId)player->heldItemId];
        if (mBButtonState.iconOverride != icon) {
            gPlayState->interfaceCtx.iconItemSegment[EQUIP_SLOT_B] = icon;
            mBButtonState.iconOverride = icon;
        }
    } else {
        if (mBButtonState.iconOverride != NULL) {
            Interface_LoadItemIcon(gPlayState, EQUIP_SLOT_B);
            mBButtonState.iconOverride = NULL;
        }
    }
}

// keeps EQUIP_SLOT_B integrity
void RestoreBButtonItem() {
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

void PlayerUpdate(Actor* actor) {
    Player* player = (Player*)actor;
    UpdateBButtonView(player);
    RestoreBButtonItem();
}

void CleanupBButtonSlot() {
    if (mBButtonState.stored != ITEM_NONE && !CVAR) {
        RestoreBButtonItem();
        mBButtonState.stored = ITEM_NONE;
    }

    if (mBButtonState.iconOverride != NULL && gPlayState != NULL) {
        Interface_LoadItemIcon(gPlayState, EQUIP_SLOT_B);
    }
    mBButtonState.iconOverride = NULL;
}

void RegisterActiveItemOnB() {
    CleanupBButtonSlot();
    COND_VB_SHOULD(VB_GET_ITEM_ON_BUTTON, CVAR, {
        EquipSlot slot = (EquipSlot)va_arg(args, int);
        ItemId* item = va_arg(args, ItemId*);
        HandleGetItemOnButton(should, slot, item);
    });
    COND_VB_SHOULD(VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        if ((ItemId)player->heldItemId == ITEM_HOOKSHOT && CLICKED_BUTTON == EQUIP_SLOT_B) {
            *should = false;
        }
    });
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, PlayerUpdate);
}

static RegisterShipInitFunc initFunc(RegisterActiveItemOnB, { CVAR_NAME });
