#pragma once
#ifdef __cplusplus

#include "ship/window/gui/GuiWindow.h"

// Live monitor for the microphone ocarina: input level, detected pitch, tracked
// notes and per-song match progress. Purely diagnostic — the ocarina input itself
// comes from MicOcarina_GetButtons.
class MicOcarinaWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override{};
    void UpdateElement() override{};
};

#endif
