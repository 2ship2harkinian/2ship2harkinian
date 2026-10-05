#include "kzTools.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64actor.h"
#include "functions.h"
#include "variables.h"
}

extern PlayState* gPlayState;

void KzToolsWindow::DrawElement() {
    UIWidgets::Checkbox("Enabled", "gKzViewer.Enabled");
};
