#ifndef ACTION_DEBUGGER_H
#define ACTION_DEBUGGER_H

#include <ship/window/gui/GuiWindow.h>

class ActionDebuggerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif // ACTION_DEBUGGER_H
