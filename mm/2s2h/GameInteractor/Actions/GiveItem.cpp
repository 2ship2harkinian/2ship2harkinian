#include "Actions.h"

#include <spdlog/spdlog.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64actor.h"
#include "z64interface.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

namespace {

// Only one give-item is ever in flight (the action blocks the queue), so a single slot is enough.
GIActions::GiveItemArgs inFlight = {};

void GiveItemAction(Actor* actor, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (inFlight.giveItem != nullptr) {
        inFlight.giveItem(actor, play);
    }

    if (inFlight.showGetItemCutscene && !(CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE)) {
        player->actor.freezeTimer = 30;
    }

    GameInteractor::Instance->FinishBlocking(GI_STATUS_APPLIED);
}

void GiveItemDestroy(Actor* actor, PlayState* play) {
    if (!(CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION)) {
        // Scene torn down before pickup; requeue rather than swallow the item.
        GameInteractor::Instance->RequeueBlocking();
    }
}

} // namespace

GIAction GIActions::GiveItem(GIActions::GiveItemArgs args) {
    return GIAction{
        .id = GI_ACTION_NONE,
        .blocking = true,
        .onStart =
            [args](GIAction& action) {
                Player* player = GET_PLAYER(gPlayState);

                inFlight = args;

                s16 flags = CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER;

                // If the player is climbing or in the air, deliver the item without a cutscene but
                // freeze the player
                if (!args.showGetItemCutscene ||
                    (player->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_2000 |
                                            PLAYER_STATE1_4000 | PLAYER_STATE1_40000 | PLAYER_STATE1_80000 |
                                            PLAYER_STATE1_100000 | PLAYER_STATE1_200000 | PLAYER_STATE1_8000000)) ||
                    (Player_GetExplosiveHeld(player) > PLAYER_EXPLOSIVE_NONE)) {

                    flags |= CustomItem::GIVE_OVERHEAD;
                } else {
                    flags |= CustomItem::GIVE_ITEM_CUTSCENE;
                }

                EnItem00* enItem00 =
                    CustomItem::Spawn(player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z,
                                      0, flags, args.param, GiveItemAction, args.drawItem);
                if (enItem00 == nullptr) {
                    // Nothing will ever call FinishBlocking, so stop blocking or the queue wedges shut.
                    SPDLOG_ERROR("[GameInteractor] Give-item couldn't spawn its item actor");
                    action.blocking = false;
                    return;
                }
                enItem00->actor.destroy = GiveItemDestroy;
            },
    };
}

// The remote-facing counterpart: no draw function, no pickup ceremony.
static GIActions::Register giveItemAction({
    .id = GI_ACTION_GIVE_ITEM,
    .name = "giveItem",
    .displayName = "Give Item",
    .valence = GI_VALENCE_POSITIVE,
    .schema =
        {
            // The item enum spans the whole byte short of ITEM_NONE (0xFF).
            { .name = "itemId", .type = GI_PARAM_INT, .required = true, .min = 0.0f, .max = 254.0f },
        },
    .onStart = [](GIAction& action) { Item_Give(gPlayState, (u8)action.params.Int("itemId")); },
});
