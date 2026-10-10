#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
}

static HOOK_ID sInputHook = 0;

static s8 NegateStick(s8 value) {
    // Negating -128 would wrap straight back to -128.
    return value == -128 ? 127 : -value;
}

// OnPassPlayerInputs fires on a *copy* of the controller state, so menus, shops and the ocarina
// are unaffected.
static GIActions::Register reverseControlsAction({
    .id = GI_ACTION_REVERSE_CONTROLS,
    .name = "reverseControls",
    .displayName = "Reverse Controls",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    .onStart =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPassPlayerInputs>(sInputHook);
            sInputHook =
                GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPassPlayerInputs>([](Input* input) {
                    // `press` is left alone: its x/y hold a delta, not a stick position.
                    OSContPad* pads[] = { &input->cur, &input->prev, &input->rel };

                    for (OSContPad* pad : pads) {
                        pad->stick_x = NegateStick(pad->stick_x);
                        pad->stick_y = NegateStick(pad->stick_y);
                    }
                });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPassPlayerInputs>(sInputHook);
            sInputHook = 0;
        },
});
