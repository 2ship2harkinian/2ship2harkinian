#include "Actions.h"

#include "2s2h/BenGui/HudEditor.h"

#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "z64.h"
#include "variables.h"
}

// The HUD is laid out in a 320x240 box, and the button art is 32x32.
#define HUD_WIDTH 320
#define HUD_HEIGHT 240
#define B_BUTTON_SIZE 32

static float sX = 0.0f;
static float sY = 0.0f;
static float sVelX = 0.0f;
static float sVelY = 0.0f;

static int32_t sPreviousMode = GIActions::Setting::ABSENT;
static int32_t sPreviousX = GIActions::Setting::ABSENT;
static int32_t sPreviousY = GIActions::Setting::ABSENT;

// Clamped as well as reflected, so an overshoot can't leave the button stuck outside the box.
static void Bounce(float& pos, float& velocity, float limit) {
    pos += velocity;

    if (pos <= 0.0f) {
        pos = 0.0f;
        velocity = -velocity;
    } else if (pos >= limit) {
        pos = limit;
        velocity = -velocity;
    }
}

// Rides the Hud Editor cvars so the background, icon and ammo count move together; MOVABLE_43
// keeps it inside the 4:3 box at any window size.
static GIActions::Register bouncingBButtonAction({
    .id = GI_ACTION_BOUNCING_B_BUTTON,
    .name = "bouncingBButton",
    .displayName = "Bouncing B Button",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .onStart =
        [](GIAction& action) {
            const HudEditorElement& b = hudEditorElements[HUD_EDITOR_ELEMENT_B];

            sPreviousMode = GIActions::Setting::Snapshot(b.modeCvar);
            sPreviousX = GIActions::Setting::Snapshot(b.xCvar);
            sPreviousY = GIActions::Setting::Snapshot(b.yCvar);

            sX = b.defaultX;
            sY = b.defaultY;

            // Always diagonal, like the screensaver.
            float speed = 2.0f;
            sVelX = (Rand_Next() & 1) ? speed : -speed;
            sVelY = (Rand_Next() & 1) ? speed : -speed;
        },
    .onTick =
        [](GIAction& action) {
            const HudEditorElement& b = hudEditorElements[HUD_EDITOR_ELEMENT_B];

            Bounce(sX, sVelX, HUD_WIDTH - B_BUTTON_SIZE);
            Bounce(sY, sVelY, HUD_HEIGHT - B_BUTTON_SIZE);

            // Re-applied every tick so the Hud Editor can't quietly hand the button back mid-bounce.
            CVarSetInteger(b.modeCvar, HUD_EDITOR_ELEMENT_MODE_MOVABLE_43);
            CVarSetInteger(b.xCvar, (int32_t)sX);
            CVarSetInteger(b.yCvar, (int32_t)sY);
        },
    .onEnd =
        [](GIAction& action) {
            const HudEditorElement& b = hudEditorElements[HUD_EDITOR_ELEMENT_B];

            GIActions::Setting::Restore(b.modeCvar, sPreviousMode);
            GIActions::Setting::Restore(b.xCvar, sPreviousX);
            GIActions::Setting::Restore(b.yCvar, sPreviousY);
        },
});
