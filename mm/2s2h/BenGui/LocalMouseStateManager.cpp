#include "LocalMouseStateManager.h"

#include <fast/Fast3dWindow.h>
#include "ship/Context.h"
#include "MouseCapture.h"

void LocalMouseStateManager::ToggleMouseCaptureOverride() {
    Mouse_ForceToggleCapture();
}

void LocalMouseStateManager::UpdateMouseCapture() {
    Mouse_UpdateCaptureByState();
    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    if (wnd->IsMouseCaptured()) {
        ResetCursorVisibilityTimer();
    }
}
