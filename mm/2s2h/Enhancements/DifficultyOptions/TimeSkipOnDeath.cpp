#include <functional>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/window/gui/ConsoleWindow.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "libultraship/libultra/gbi.h"
#include "macros.h"
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
        if (!gPlayState || CVAR == 0) {
            return;
        }

        if (!timeSkipped) {
            if  (gPlayState->gameOverCtx.state == GAMEOVER_DEATH_FADE_OUT) {
                timeSkipped = true;

                // Keep track of the overall time passed in ticks. Yes, this includes the full 72+ hours in clock ticks
                s32 skip_length = CLOCK_TIME(CVAR, 0);
                s32 current_day = gSaveContext.save.day;
                u16 current_daytime = gSaveContext.save.time - CLOCK_TIME(6, 0);        // Subtract this 6-hour offset so this value could wrap at the dawn of the next day instead of midnight
                s32 current_time = CLOCK_TIME(current_day * 24, 0) + current_daytime;   // How much time has currently passed overall
                s32 current_daystart = current_time - (s32)current_daytime;             // The start of the day in overall ticks

                // The time after the time skip
                s32 new_time = current_time + skip_length;
                u16 new_daytime = new_time % CLOCK_TIME(24, 0);
                s32 new_daystart = new_time - (s32)new_daytime;
                s32 new_day = new_daystart / CLOCK_TIME(24, 0);

                // Check for the next half-day
                if (current_day != new_day || (current_daytime < CLOCK_TIME(12, 0) && new_daytime >= CLOCK_TIME(12, 0))) {
                    // Check for skip to 4th day (this logic should prevent playing the moon crash cutscene upon dying during the 4th day glitch)
                    if (current_day <= 3 && new_day > 3) {
                        // Trigger the moon crash cutscene
                        Interface_StartMoonCrash(gPlayState);
                    } else {
                        // Else just trigger the day card
                        SET_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
                        gSaveContext.respawnFlag = -63;
                    }
                }
                
                gSaveContext.save.time = new_daytime + CLOCK_TIME(6, 0);    // Re-add the 6-hour offset
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
