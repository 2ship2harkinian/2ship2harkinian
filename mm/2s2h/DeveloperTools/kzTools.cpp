#include "kzTools.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64.h"
#include "z64actor.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

void DrawKzMenu(){
    UIWidgets::BeginCardLayout({ .columnsPerRow = 3, .minColumnWidth = 420.0f });

    if (gPlayState) {
        Player* player = GET_PLAYER(gPlayState);

        UIWidgets::BeginCardLayout({ .columnsPerRow = 2, .minColumnWidth = 420.0f });
        
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        ImGui::InputScalar("Underwater Timer", ImGuiDataType_Float, &player.underwaterTimer);
        ImGui::InputScalar("Underwater Timer", ImGuiDataType_Float, &player->actor.depthInWater);
    }

} 

void KzToolsWindow::DrawElement() {
    DrawKzMenu();
};

void KzToolsWindow::InitElement() {
};
