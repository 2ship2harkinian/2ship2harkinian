#include "2s2h/BenGui/UIWidgets.hpp"
#include "ShipUtils.h"
#include "2s2h/BenGui/BenMenu.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Network/Sail/Sail.h"

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
} // namespace BenGui
using namespace UIWidgets;

void RegisterNetworkMenu() {
    // Add Network Menu
    BenGui::mBenMenu->AddMenuEntry("Network", "gSettings.Menu.NetworkSidebarSection");
    WidgetPath path;

    // Sail
    path = { "Network", "Sail", SECTION_COLUMN_1 };
    BenGui::mBenMenu->AddSidebarEntry("Network", path.sidebarName, 3);
    BenGui::mBenMenu->AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(Sail::Instance->isEnabled);
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostSail", "gNetwork.Sail.Host",
                        InputOptions()
                            .PlaceholderText("127.0.0.1")
                            .DefaultValue("127.0.0.1")
                            .Size(ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 7, 0))
                            .LabelPosition(LabelPosition::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortSail", "gNetwork.Sail.Port",
                     InputOptions()
                         .PlaceholderText("43384")
                         .DefaultValue("43384")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPosition::None));
        ImGui::EndDisabled();
    });
    BenGui::mBenMenu->AddWidget(path, "Enable##Sail", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString("gNetwork.Sail.Host", "127.0.0.1");
            uint16_t port = CVarGetInteger("gNetwork.Sail.Port", 43384);
            info.options->disabled = !(!IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Sail::Instance->isEnabled) {
                info.name = "Disable##Sail";
            } else {
                info.name = "Enable##Sail";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (Sail::Instance->isEnabled) {
                CVarClear("gNetwork.Sail.Enabled");
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Disable();
            } else if (Sail::Instance->Enable()) {
                CVarSetInteger("gNetwork.Sail.Enabled", 1);
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            } else {
                Notification::Emit({ .message = "Couldn't resolve the Sail host" });
            }
        });
    BenGui::mBenMenu->AddWidget(path, "Connecting...", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !Sail::Instance->isEnabled;
        if (Sail::Instance->isConnected) {
            info.name = "Connected##Sail";
        } else {
            info.name = "Connecting...##Sail";
        }
    });
    path.column = SECTION_COLUMN_2;
    BenGui::mBenMenu->AddWidget(path,
                                "Sail is a networking protocol designed to facilitate remote "
                                "control of the 2Ship2Harkinian client. It is intended to "
                                "be utilized alongside a Sail server, for which we provide a "
                                "few straightforward implementations on our GitHub. The current "
                                "implementations available allow integration with Twitch chat "
                                "and SAMMI Bot, feel free to contribute your own!\n"
                                "\n"
                                "Click this button to copy the link to the Sail Github "
                                "page to your clipboard.",
                                WIDGET_TEXT);
    BenGui::mBenMenu->AddWidget(path, ICON_FA_CLIPBOARD "##Sail", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
            Notification::Emit({
                .message = "Copied to clipboard",
            });
        })
        .Options(ButtonOptions().Tooltip("https://github.com/HarbourMasters/sail"));
}

static RegisterMenuInitFunc initFunc(RegisterNetworkMenu);