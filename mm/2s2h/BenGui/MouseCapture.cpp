#include "MouseCapture.h"

#include "2s2h/Mouse.h"
#include "ship/Context.h"
#include "ShipInit.hpp"
#include "GameInteractor/GameInteractor.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include <libultraship/bridge/consolevariablebridge.h>

typedef struct MouseCaptureGameState {
    bool inKaleido;
    bool gameStarted;
    bool isCaptureForced;
    bool forcedCaptureValue;
} MouseCaptureGameState;

static MouseCaptureGameState gameState = {};

#ifdef __cplusplus
extern "C" {
#endif

void Mouse_SetForceCapture(bool isEnabled, bool value) {
    gameState.isCaptureForced = isEnabled;
    gameState.forcedCaptureValue = value;
    Mouse_UpdateCaptureByState();
}

bool InferCaptureFromState(MouseCaptureGameState state) {
    bool inBenMenu = Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetMenuOrMenubarVisible();

    if (state.isCaptureForced) {
        return state.forcedCaptureValue;
    } else if (inBenMenu || state.inKaleido) {
        return false;
    } else if (!state.gameStarted) {
        return false;
    } else {
        return true;
    }
}

void HandleForcing() {
    if (!gameState.isCaptureForced ||
        !Ship::Context::GetRawInstance()->GetWindow()->GetMouseStateManager()->ShouldAutoCaptureMouse()) {
        gameState.isCaptureForced = true;
        gameState.forcedCaptureValue = !Ship::Context::GetRawInstance()->GetWindow()->IsMouseCaptured();
        return;
    }

    // Detect if we are forcing the opposite of what the game state would normally dictate
    MouseCaptureGameState unforcedState = gameState;
    unforcedState.isCaptureForced = false;
    bool isOppositeForced = (gameState.forcedCaptureValue == InferCaptureFromState(unforcedState));

    if (isOppositeForced) {
        gameState.forcedCaptureValue = !gameState.forcedCaptureValue;
    } else {
        gameState.isCaptureForced = false;
    }
}

void Mouse_ForceToggleCapture() {
    HandleForcing();
    Mouse_UpdateCaptureByState();
}

void Mouse_UpdateCaptureByState() {
    bool capture = InferCaptureFromState(gameState);

    if (gameState.isCaptureForced ||
        Ship::Context::GetRawInstance()->GetWindow()->GetMouseStateManager()->ShouldAutoCaptureMouse() || !capture) {
        Ship::Context::GetRawInstance()->GetWindow()->SetMouseCapture(capture);
    }
}

void HandleKaleidoCapture(PauseContext* pauseCtx) {
    bool prev = gameState.inKaleido;
    switch (pauseCtx->state) {
        case PAUSE_STATE_MAIN: {
            gameState.inKaleido = true;
            break;
        }
        case PAUSE_STATE_UNPAUSE_SETUP: {
            gameState.inKaleido = false;
            break;
        }
    }
    if (prev != gameState.inKaleido) {
        Mouse_UpdateCaptureByState();
    }
}

void RegisterMouseCaptureHooks() {
    {
        static bool autoCapture = false;
        bool newAutoCapture =
            CVarGetInteger("gSettings.EnableMouse", 0) && CVarGetInteger("gSettings.AutoCaptureMouse", 1);
        if (autoCapture != newAutoCapture) {
            autoCapture = newAutoCapture;
            Ship::Context::GetRawInstance()->GetWindow()->SetAutoCaptureMouse(autoCapture);
        }
    }
    COND_HOOK(OnKaleidoUpdate, true, HandleKaleidoCapture);
    COND_HOOK(OnSaveLoad, true, [](s16 fileNum) {
        gameState.gameStarted = true;
        Mouse_UpdateCaptureByState();
    });
    COND_HOOK(OnConsoleLogoUpdate, true, []() {
        if (gameState.gameStarted) {
            gameState.gameStarted = false;
            gameState.inKaleido = false;
            Mouse_UpdateCaptureByState();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMouseCaptureHooks,
                                     { "gSettings.EnableMouse", "gSettings.AutoCaptureMouse" });

#ifdef __cplusplus
} // extern "C"
#endif
