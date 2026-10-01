#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/window/gui/ConsoleWindow.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "z64interface.h"
#include "z64save.h"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.TimeSkipOnDeath"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static bool timeSkipped = false;

void RegisterTimeSkipOnDeath() {
    COND_HOOK(OnGameStateUpdate, CVAR, []() {
        if (!gPlayState) {
            return;
        }

        if (!timeSkipped) {
            if  (gPlayState->gameOverCtx.state == GAMEOVER_REVIVE_FADE_OUT) {
                timeSkipped = true;

                u16 skip_length = CLOCK_TIME(CVAR, 0);
                u16 current_time = gSaveContext.save.time;
                s32 current_day = gSaveContext.save.day;

                u16 new_time = current_time + skip_length;
                s32 new_day = current_day;

                // Check for the next half-day
                if ((current_time < CLOCK_TIME(6, 0) && new_time >= CLOCK_TIME(6, 0)) ||
                    (current_time < CLOCK_TIME(18, 0) && new_time >= CLOCK_TIME(18, 0)) || 
                    CVAR >= 12) {

                    // Check for the next day
                    if ((current_time < CLOCK_TIME(6, 0) && new_time >= CLOCK_TIME(6, 0)) ||
                        CVAR >= 24) {
                        new_day += (1 + CVAR / 24);
                    }

                    // Check for skip to 4th day (this logic should prevent dying in the 4th day glitch to cause the moon crash)
                    if (current_day <= 3 && new_day > 3) {
                        // Trigger the moon crash cutscene
                        Interface_StartMoonCrash(gPlayState);
                    } else {
                        // Else just trigger the day card
                        SET_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
                        gSaveContext.respawnFlag = -63;
                    }
                }
                
                gSaveContext.save.time = new_time;
                gSaveContext.save.day = new_day;
            }
        } else {
            if (gPlayState->gameOverCtx.state == GAMEOVER_INACTIVE) {
                timeSkipped = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTimeSkipOnDeath, { CVAR_NAME });
