#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

// Drops the player's entire wallet on the floor as collectible rupees.
static GIActions::Register emptyWalletAction({
    .id = GI_ACTION_EMPTY_WALLET,
    .name = "emptyWallet",
    .displayName = "Empty Wallet",
    .valence = GI_VALENCE_NEGATIVE,
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            int16_t currentRupees = gSaveContext.save.saveInfo.playerData.rupees;
            if (currentRupees == 0) {
                return;
            }

            Vec3f positional = GET_PLAYER(gPlayState)->actor.world.pos;
            positional.y = GET_PLAYER(gPlayState)->actor.world.pos.y + 100.0f;

            while (currentRupees > 0) {
                Item00Type rupee = ITEM00_RUPEE_GREEN;
                int16_t denomination = 1;
                if (currentRupees >= 20) {
                    rupee = ITEM00_RUPEE_RED;
                    denomination = 20;
                } else if (currentRupees >= 5) {
                    rupee = ITEM00_RUPEE_BLUE;
                    denomination = 5;
                }

                EnItem00* rupeeActor = (EnItem00*)Item_DropCollectible(gPlayState, &positional, rupee);
                if (rupeeActor == NULL) {
                    // Actor system is full; whatever didn't make it onto the floor stays in the wallet.
                    break;
                }
                rupeeActor->actor.speed = Rand_CenteredFloat(5.0f);
                rupeeActor->unk152 = 600; // Extending Time before Despawning

                Rupees_ChangeBy(-denomination);
                currentRupees -= denomination;
            }
        },
});
