#pragma once

#include <fast/FastMouseStateManager.h>

class LocalMouseStateManager : public Fast::FastMouseStateManager {
    void ToggleMouseCaptureOverride() override;
    void UpdateMouseCapture() override;
};
