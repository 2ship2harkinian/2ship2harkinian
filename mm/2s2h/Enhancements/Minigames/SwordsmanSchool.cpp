#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Kendo_Js/z_en_kendo_js.h"

void EnKendoJs_ExpertCourse(EnKendoJs* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.SwordsmanSchoolScore"
#define CVAR CVarGetInteger(CVAR_NAME, 30)

void RegisterSwordsmanSchool() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_KENDO_JS, CVAR != 30, [](Actor* actor, bool* should) {
        EnKendoJs* kendo = (EnKendoJs*)actor;

        if (kendo->actionFunc != EnKendoJs_ExpertCourse) {
            return;
        }

        // Finishes the game early, as soon as the player reaches the required score
        if (gSaveContext.minigameScore >= CVAR) {
            kendo->timer = 140;
            kendo->minigameRound.expertRound = 5;
        }

        // Each time player chops a log, check if they've reached the required score
        if (kendo->timer >= 140 && kendo->minigameRound.expertRound == 5 && gSaveContext.minigameScore >= CVAR) {
            gSaveContext.minigameScore = 30;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSwordsmanSchool, { CVAR_NAME });
