#pragma once

#include <ship/window/gui/GuiWindow.h>

extern "C" {
#include "z64save.h"
}

class KzToolsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};