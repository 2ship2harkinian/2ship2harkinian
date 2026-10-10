#include "Anchor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

void AnchorRoomWindow::Draw() {
    if (!IsVisible() || !Anchor::Instance->isConnected) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, CVarGetFloat("gNotifications.BgOpacity", 0.5f)));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::Begin("Anchor Room", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    DrawElement();

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorRoomWindow::DrawElement() {
    bool isGlobalRoom = (std::string("2ship-global") == CVarGetString("gNetwork.Anchor.RoomId", ""));

    if (isGlobalRoom) {
        u32 activeClients = 0;
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            if (client.online) {
                activeClients++;
            }
        }
        ImGui::Text("Players Online: %d", activeClients);
        return;
    }

    // First build a list of teams
    std::set<std::string> teams;
    for (auto& [clientId, client] : Anchor::Instance->clients) {
        teams.insert(client.teamId);
    }

    for (auto& team : teams) {
        if (teams.size() > 1) {
            ImGui::SeparatorText(team.c_str());
        }
        bool isOwnTeam = team == CVarGetString("gNetwork.Anchor.TeamId", "default");
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            if (client.teamId != team) {
                continue;
            }

            ImGui::PushID(clientId);

            if (client.clientId == Anchor::Instance->roomState.ownerClientId) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", ICON_FA_GAVEL);
                ImGui::SameLine();
            }

            if (client.self) {
                ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "%s", CVarGetString("gNetwork.Anchor.Name", ""));
            } else if (!client.online) {
                ImGui::TextColored(ImVec4(1, 1, 1, 0.3f), "%s - offline", client.name.c_str());
                ImGui::PopID();
                continue;
            } else {
                ImGui::Text("%s", client.name.c_str());
            }

            if (Anchor::Instance->roomState.showLocationsMode == 2 ||
                (Anchor::Instance->roomState.showLocationsMode == 1 && isOwnTeam)) {
                if ((client.self ? Anchor::Instance->IsSaveLoaded() : client.isSaveLoaded)) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), "- %s",
                                       Ship_GetSceneName(client.self ? gPlayState->sceneId : client.sceneId));
                }
            }

            if (Anchor::Instance->CanTeleportTo(client.clientId)) {
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Button(ICON_FA_LOCATION_ARROW, ImVec2(20.0f, 20.0f))) {
                    Anchor::Instance->SendPacket_RequestTeleport(client.clientId);
                }
                ImGui::PopStyleVar();
            }

            if (client.clientVersion != Anchor::clientVersion) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Incompatible version! Will not work together!");
                    ImGui::Text("Yours: %s", Anchor::clientVersion.c_str());
                    ImGui::Text("Theirs: %s", client.clientVersion.c_str());
                    ImGui::EndTooltip();
                }
            }
            ImGui::PopID();
        }
    }
}
