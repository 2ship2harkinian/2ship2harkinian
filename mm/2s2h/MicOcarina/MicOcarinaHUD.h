#pragma once
#ifdef __cplusplus

#include "ship/window/gui/GuiWindow.h"

// Small in-game overlay for the microphone ocarina: a mic icon with status
// text ("Waiting for audio" / "Receiving audio" / "Playing: <song>") and a
// rolling waveform of the mic input level. It appears automatically only
// while the ocarina is out and the Microphone Ocarina enhancement is on.
// The screen corner it sits in is chosen with the
// "gEnhancements.Playback.MicOcarinaHUDCorner" CVar (0-3).
class MicOcarinaHUD : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    // Replaces the CVar-gated base draw: this window manages its own
    // Begin/End so it can auto-show while the ocarina is open.
    void Draw() override;
    void DrawElement() override{};
    void InitElement() override{};
    void UpdateElement() override{};
};

#endif
