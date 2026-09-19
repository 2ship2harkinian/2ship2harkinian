#include "Anchor.h"
#include <libultraship/libultraship.h>
#include "2s2h/BenGui/UIWidgets.hpp"
#include "ShipUtils.h"
#include "2s2h/BenGui/BenMenu.h"
#include "2s2h/BenGui/BenGui.hpp"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Network/Sail/Sail.h"
#include "2s2h/Rando/Spoiler/Spoiler.h"
#include "2s2h/Rando/MiscBehavior/MiscBehavior.h"

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
extern std::shared_ptr<AnchorRoomWindow> mAnchorRoomWindow;
} // namespace BenGui
using namespace UIWidgets;

static const char* pvpModes[3] = { "Off", "On", "On + Friendly Fire" };
static std::vector<const char*> teleportModes = { "None", "Team Only", "All" };
static std::vector<const char*> showLocationsModes = { "None", "Team Only", "All" };

void AnchorMainMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;

    std::string host = CVarGetString("gNetwork.Anchor.Host", "anchor.hm64.org");
    uint16_t port = CVarGetInteger("gNetwork.Anchor.Port", 43383);
    std::string anchorTeamId = CVarGetString("gNetwork.Anchor.TeamId", "default");
    std::string anchorRoomId = CVarGetString("gNetwork.Anchor.RoomId", "");
    std::string anchorName = CVarGetString("gNetwork.Anchor.Name", "");
    bool isFormValid = !IsStringEmpty(host) && port > 1024 && port < 65535 && !IsStringEmpty(anchorRoomId) &&
                       !IsStringEmpty(anchorName);

    ImGui::SeparatorText("Connection Settings");

    ImGui::BeginDisabled(anchor->isEnabled);
    ImGui::Text("Host & Port");
    if (UIWidgets::InputString("##Host", &host,
                               UIWidgets::InputOptions()
                                   .Size(ImGui::GetContentRegionAvail() -
                                         ImVec2((ImGui::GetFontSize() * 5 + ImGui::GetStyle().ItemSpacing.x), 0))
                                   .Color(THEME_COLOR))) {
        CVarSetString("gNetwork.Anchor.Host", host.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();
    UIWidgets::PushStyleInput(THEME_COLOR);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5);
    if (ImGui::InputScalar("##Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger("gNetwork.Anchor.Port", port);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    UIWidgets::PopStyleInput();

    ImGui::Text("Name");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##Name", &anchorName, UIWidgets::InputOptions().Color(THEME_COLOR))) {
        CVarSetString("gNetwork.Anchor.Name", anchorName.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Text("Room ID");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##RoomId", &anchorRoomId,
                               UIWidgets::InputOptions().IsSecret(anchor->isEnabled).Color(THEME_COLOR))) {
        CVarSetString("gNetwork.Anchor.RoomId", anchorRoomId.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Text("Team ID (Items Shared)");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##TeamId", &anchorTeamId, UIWidgets::InputOptions().Color(THEME_COLOR))) {
        CVarSetString("gNetwork.Anchor.TeamId", anchorTeamId.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Spacing();

    if (UIWidgets::Button("Restore Defaults", UIWidgets::ButtonOptions()
                                                  .Size(ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))
                                                  .Color(UIWidgets::Colors::Red))) {
        CVarSetString("gNetwork.Anchor.Host", "anchor.hm64.org");
        CVarSetInteger("gNetwork.Anchor.Port", 43383);
        CVarSetString("gNetwork.Anchor.TeamId", "default");
        CVarSetString("gNetwork.Anchor.RoomId", "");
        CVarSetString("gNetwork.Anchor.Name", "");
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();

    if (UIWidgets::Button("Global Room", UIWidgets::ButtonOptions()
                                             .Color(UIWidgets::Colors::Blue)
                                             .Tooltip("Always-online public room so you don't have to experience "
                                                      "Termina alone. PVP and syncing are disabled."))) {
        CVarSetString("gNetwork.Anchor.Host", "anchor.hm64.org");
        CVarSetInteger("gNetwork.Anchor.Port", 43383);
        CVarSetString("gNetwork.Anchor.TeamId", "default");
        CVarSetString("gNetwork.Anchor.RoomId", "2ship-global");
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = anchor->isEnabled ? "Disable" : "Enable";
    UIWidgets::PushStyleButton(anchor->isEnabled ? UIWidgets::ColorValues.at(UIWidgets::Colors::Red)
                                                 : UIWidgets::ColorValues.at(UIWidgets::Colors::Green));
    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
        if (anchor->isEnabled) {
            CVarClear("gNetwork.Anchor.Enabled");
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            anchor->Disable();
        } else {
            CVarSetInteger("gNetwork.Anchor.Enabled", 1);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            anchor->Enable();
        }
    }
    UIWidgets::PopStyleButton();
    ImGui::EndDisabled();
    ImGui::Spacing();

    if (!anchor->isEnabled) {
        return;
    }

    if (!anchor->isConnected) {
        ImGui::Text("Connecting...");
        return;
    }

    ImGui::SeparatorText("Current Room");
    ImGui::Text("%s Connected", ICON_FA_CHECK);

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Request Team State")) {
        anchor->SendPacket_RequestTeamState();
    }
    UIWidgets::Tooltip("Try this if you are missing items that your team members have collected");
    UIWidgets::PopStyleButton();

    ImGui::SameLine();

    UIWidgets::WindowButton("Toggle Anchor Room Window", "gWindows.AnchorRoom", BenGui::mAnchorRoomWindow);
    if (!BenGui::mAnchorRoomWindow->IsVisible()) {
        BenGui::mAnchorRoomWindow->DrawElement();
    }
}

void AnchorAdminMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;
    bool isGlobalRoom = (std::string("2ship-global") == CVarGetString("gNetwork.Anchor.RoomId", ""));

    if (!anchor->isEnabled || !anchor->isConnected || anchor->roomState.ownerClientId != anchor->ownClientId ||
        isGlobalRoom) {
        return;
    }

    ImGui::SeparatorText("Room Settings (Admin Only)");

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Clear All Team State")) {
        std::set<std::string> teams;
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            teams.insert(client.teamId);
        }
        for (auto& team : teams) {
            anchor->SendPacket_ClearTeamState(team);
        }
    }
    UIWidgets::PopStyleButton();

    if (UIWidgets::CVarCombobox("PvP Mode:", "gNetwork.Anchor.RoomSettings.PvpMode", pvpModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(2)
                                    .LabelPosition(UIWidgets::LabelPosition::Above)
                                    .Color(THEME_COLOR))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCombobox("Show Locations For:", "gNetwork.Anchor.RoomSettings.ShowLocationsMode",
                                showLocationsModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(1)
                                    .LabelPosition(UIWidgets::LabelPosition::Above)
                                    .Color(THEME_COLOR))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCombobox("Allow Teleporting To:", "gNetwork.Anchor.RoomSettings.TeleportMode", teleportModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(1)
                                    .LabelPosition(UIWidgets::LabelPosition::Above)
                                    .Color(THEME_COLOR))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCheckbox("Sync Items & Flags", "gNetwork.Anchor.RoomSettings.SyncItemsAndFlags",
                                UIWidgets::CheckboxOptions().DefaultValue(true).Color(THEME_COLOR))) {
        anchor->SendPacket_UpdateRoomState();
    }
}

void AnchorInstructionsMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;

    ImGui::TextWrapped(
        "Important Notes:\n- All players involved should start at the file select screen with Anchor disabled.\n- "
        "Archipelago and Anchor can be used together, but make sure you disable item syncing in Anchor!");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::SeparatorText("Co-op Usage Instructions");

    ImGui::TextWrapped("1. The host player should set up a room by entering a Room ID and enable Anchor. (If just one "
                       "team, leave the team as 'default')");

    ImGui::TextWrapped("2. Configure the room settings as desired (PvP, teleporting, etc).");

    ImGui::TextWrapped("3. If multiple teams, other team leaders should enter the same Room ID and their associated "
                       "Team IDs and enable Anchor.");

    ImGui::TextWrapped("4. Host/Team leaders should then create a randomizer seed like normal, and load into the game. "
                       "This can match across teams but doesn't have to.");

    ImGui::TextWrapped("4. Once the Host/Team leaders are in-game, other players can enter the same Room ID and "
                       "associated Team IDs and enable Anchor.");

    ImGui::TextWrapped("5. Other players should then each create a new randomizer file (the configuration does not "
                       "matter, it will be overridden by the team leader's settings), and load into the game.");

    ImGui::TextWrapped("6. All players should now be connected and able to see each other in-game, occasionally you "
                       "may need to leave and re-enter South clock town if a player does not appear.");
}

void RegisterAnchorMenu() {
    // Add Network Menu
    BenGui::mBenMenu->AddMenuEntry("Network", "gSettings.Menu.NetworkSidebarSection");

    BenGui::mBenMenu->AddSidebarEntry("Network", "Anchor", 2);
    WidgetPath path = { "Network", "Anchor", SECTION_COLUMN_1 };
    BenGui::mBenMenu->AddWidget(path, "AnchorMainMenu", WIDGET_CUSTOM).CustomFunction(AnchorMainMenu);
    path.column = SECTION_COLUMN_2;
    BenGui::mBenMenu->AddWidget(path, "AnchorAdminMenu", WIDGET_CUSTOM).CustomFunction(AnchorAdminMenu);
    BenGui::mBenMenu->AddWidget(path, "AnchorInstructionsMenu", WIDGET_CUSTOM).CustomFunction(AnchorInstructionsMenu);
}

static RegisterMenuInitFunc menuInitFunc(RegisterAnchorMenu);
