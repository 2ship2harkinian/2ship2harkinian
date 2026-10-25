#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.NoDropDespawning"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterNoDropDespawning() {
    // Stop dropped items from despawning after a set amount of time
    COND_VB_SHOULD(VB_UPDATE_ITEM00, CVAR, {
        EnItem00* itemActor = va_arg(args, EnItem00*);
        if (itemActor->unk14C <= 0) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterNoDropDespawning, { CVAR_NAME });
