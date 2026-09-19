#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

static HOOK_ID sInputHook = 0;
static float sStrength = 40.0f;
// A binary angle, so the phase wraps at the end of a cycle for free.
static s16 sPhase = 0;
static const s16 sPhaseStep = 0x10000 / (20 * 3); // one full sway every 3 seconds

static s8 AddSway(s8 value, float sway) {
    float swayed = value + sway;

    // The total can leave the range an s8 stick can express.
    if (swayed > 127.0f) {
        return 127;
    }
    if (swayed < -127.0f) {
        return -127;
    }
    return (s8)swayed;
}

// Sways the stick from side to side. OnPassPlayerInputs fires on a *copy* of the controller state,
// so menus, shops and the ocarina are unaffected.
static GIActions::Register drunkLinkAction({
    .id = GI_ACTION_DRUNK_LINK,
    .name = "drunkLink",
    .displayName = "Drunk Link",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .schema =
        {
            // How far the sway pushes the stick, in stick units; 127 is a full deflection.
            { .name = "strength", .type = GI_PARAM_FLOAT, .defaultValue = 40.0f, .min = 5.0f, .max = 127.0f },
        },
    .onStart =
        [](GIAction& action) {
            sStrength = action.params.Float("strength");
            sPhase = 0;

            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPassPlayerInputs>(sInputHook);
            sInputHook =
                GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPassPlayerInputs>([](Input* input) {
                    sPhase += sPhaseStep;
                    float sway = Math_SinS(sPhase) * sStrength;

                    // `prev` is left alone: stamping this frame's phase onto last frame's stick
                    // would describe a sway that never happened.
                    input->cur.stick_x = AddSway(input->cur.stick_x, sway);
                    input->rel.stick_x = AddSway(input->rel.stick_x, sway);
                });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPassPlayerInputs>(sInputHook);
            sInputHook = 0;
        },
});
