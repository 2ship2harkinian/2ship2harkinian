#include "ActionDebugger.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/GameInteractor/Actions/Actions.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include <libultraship/bridge/consolevariablebridge.h>

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
#include "variables.h"
}

namespace {

const ImVec4& Color(UIWidgets::Colors color) {
    return UIWidgets::ColorValues.at(color);
}

// Wrapped, because these are long enough to clip.
void Hint(const char* text) {
    ImGui::PushStyleColor(ImGuiCol_Text, Color(UIWidgets::Colors::Gray));
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();
}

struct ActionEditor {
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, std::string> strings;
    int duration = 0;
    std::string lastError;
    bool seeded = false;
};

std::unordered_map<int, ActionEditor> editors;

bool queueAsSession = false;
int expiresAfter = 0;

UIWidgets::Colors ValenceColor(GIValence valence) {
    switch (valence) {
        case GI_VALENCE_POSITIVE:
            return UIWidgets::Colors::Green;
        case GI_VALENCE_NEGATIVE:
            return UIWidgets::Colors::Red;
        case GI_VALENCE_NEUTRAL:
            break;
    }
    return UIWidgets::Colors::LightBlue;
}

const char* ActionName(GIActionId id) {
    if (id == GI_ACTION_NONE) {
        return "(anonymous)";
    }
    const GIActions::Definition* definition = GIActions::Get(id);
    return definition != nullptr ? definition->name : "(unknown)";
}

template <typename T> T DefaultOr(const GIParamSpec& spec, T fallback) {
    if (spec.defaultValue.has_value()) {
        if (const T* value = std::get_if<T>(&*spec.defaultValue)) {
            return *value;
        }
    }
    return fallback;
}

void SeedEditor(ActionEditor& editor, const GIActions::Definition& definition) {
    for (const auto& spec : definition.schema) {
        switch (spec.type) {
            case GI_PARAM_BOOL:
                editor.bools[spec.name] = DefaultOr<bool>(spec, false);
                break;
            case GI_PARAM_INT:
                editor.ints[spec.name] = DefaultOr<int32_t>(spec, 0);
                break;
            case GI_PARAM_FLOAT:
                editor.floats[spec.name] = DefaultOr<float>(spec, 0.0f);
                break;
            case GI_PARAM_STRING:
                editor.strings[spec.name] = DefaultOr<std::string>(spec, "");
                break;
        }
    }
    editor.duration = (int)definition.defaultDuration;
    editor.seeded = true;
}

void DrawProgress(const GIAction& action) {
    float progress = action.duration > 0 ? (float)action.elapsed / (float)action.duration : 0.0f;
    char overlay[32];
    snprintf(overlay, sizeof(overlay), "%u / %u", action.elapsed, action.duration);
    ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), overlay);
}

void DrawParamEditors(ActionEditor& editor, const GIActions::Definition& definition) {
    for (const auto& spec : definition.schema) {
        std::string id = "##" + std::string(definition.name) + spec.name;

        if (spec.type == GI_PARAM_BOOL) {
            UIWidgets::Checkbox((std::string(spec.name) + id).c_str(), &editor.bools[spec.name],
                                { .color = UIWidgets::Colors::Gray });
            continue;
        }

        ImGui::Text("%s", spec.name);
        if (spec.required) {
            ImGui::SameLine(0.0f, 2.0f);
            ImGui::TextColored(Color(UIWidgets::Colors::Red), "*");
        }
        if (spec.min.has_value() || spec.max.has_value()) {
            ImGui::SameLine();
            if (spec.min.has_value() && spec.max.has_value()) {
                ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(%g .. %g)", *spec.min, *spec.max);
            } else if (spec.min.has_value()) {
                ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(>= %g)", *spec.min);
            } else {
                ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(<= %g)", *spec.max);
            }
        }

        switch (spec.type) {
            case GI_PARAM_INT:
                UIWidgets::PushStyleInput(UIWidgets::Colors::Gray);
                ImGui::InputInt(id.c_str(), &editor.ints[spec.name]);
                UIWidgets::PopStyleInput();
                break;
            case GI_PARAM_FLOAT:
                UIWidgets::PushStyleInput(UIWidgets::Colors::Gray);
                ImGui::InputFloat(id.c_str(), &editor.floats[spec.name]);
                UIWidgets::PopStyleInput();
                break;
            case GI_PARAM_STRING:
                UIWidgets::InputString(id.c_str(), &editor.strings[spec.name],
                                       { .labelPosition = UIWidgets::LabelPosition::None });
                break;
            case GI_PARAM_BOOL:
                break;
        }
    }
}

void FireAction(ActionEditor& editor, const GIActions::Definition& definition) {
    GIParams params;
    for (const auto& spec : definition.schema) {
        switch (spec.type) {
            case GI_PARAM_BOOL:
                params.Set(spec.name, editor.bools[spec.name]);
                break;
            case GI_PARAM_INT:
                params.Set(spec.name, (int32_t)editor.ints[spec.name]);
                break;
            case GI_PARAM_FLOAT:
                params.Set(spec.name, editor.floats[spec.name]);
                break;
            case GI_PARAM_STRING:
                params.Set(spec.name, editor.strings[spec.name]);
                break;
        }
    }

    std::string error;
    auto action = definition.Build(std::move(params), &error);
    if (!action.has_value()) {
        editor.lastError = error;
        return;
    }
    editor.lastError.clear();

    if (editor.duration > 0) {
        action->duration = (uint32_t)editor.duration;
    }
    action->lifetime = queueAsSession ? GI_LIFETIME_SESSION : GI_LIFETIME_SAVE;
    action->expiresAfter = (uint32_t)std::max(expiresAfter, 0);

    GameInteractor::Instance->Queue(std::move(*action));
}

const std::vector<const GIActions::Definition*>& SortedActions() {
    static std::vector<const GIActions::Definition*> sorted = [] {
        std::vector<const GIActions::Definition*> all;
        all.reserve(GIActions::All().size());
        for (const auto& definition : GIActions::All()) {
            all.push_back(&definition);
        }
        std::sort(all.begin(), all.end(), [](const GIActions::Definition* a, const GIActions::Definition* b) {
            return strcmp(a->displayName, b->displayName) < 0;
        });
        return all;
    }();
    return sorted;
}

void DrawStatusBar() {
    ImGui::Text("Gate");
    ImGui::SameLine();
    if (GameInteractor::Instance->CanProcessActions() == GI_AVAILABILITY_READY) {
        ImGui::TextColored(Color(UIWidgets::Colors::Green), "READY");
    } else {
        ImGui::TextColored(Color(UIWidgets::Colors::Yellow), "WAITING");
        UIWidgets::Tooltip("A message, a cutscene, or death is up. Requests will wait rather than be dropped.");
    }

    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::DarkGray), "|");
    ImGui::SameLine();
    ImGui::Text("Blocking");
    ImGui::SameLine();
    const GIAction* blocking = GameInteractor::Instance->BlockingAction();
    if (blocking != nullptr) {
        ImGui::TextColored(Color(UIWidgets::Colors::Yellow), "%s", ActionName(blocking->id));
    } else {
        ImGui::TextColored(Color(UIWidgets::Colors::Gray), "none");
    }

    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::DarkGray), "|");
    ImGui::SameLine();
    ImGui::Text("Pending");
    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::Gray), "%zu", GameInteractor::Instance->PendingActions().size());
    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::DarkGray), "|");
    ImGui::SameLine();
    ImGui::Text("Active");
    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::Gray), "%zu", GameInteractor::Instance->ActiveActions().size());
}

void DrawActionCard(const GIActions::Definition& definition) {
    ActionEditor& editor = editors[definition.id];
    if (!editor.seeded) {
        SeedEditor(editor, definition);
    }

    UIWidgets::BeginCard(definition.name);

    ImGui::Text("%s", definition.displayName);
    UIWidgets::Tooltip(definition.name);
    ImGui::SameLine(0.0f, 4.0f);
    ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(%s)", definition.IsTimed() ? "timed" : "instant");

    const GIAction* active = GameInteractor::Instance->FindActive(definition.id);
    if (active != nullptr) {
        DrawProgress(*active);
    }

    if (!definition.schema.empty() || definition.IsTimed()) {
        UIWidgets::Separator(true, true);
        DrawParamEditors(editor, definition);

        if (definition.IsTimed()) {
            ImGui::Text("duration");
            ImGui::SameLine();
            ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(frames, default %u)", definition.defaultDuration);
            UIWidgets::PushStyleInput(UIWidgets::Colors::Gray);
            ImGui::InputInt(("##duration" + std::string(definition.name)).c_str(), &editor.duration);
            UIWidgets::PopStyleInput();
        }
    }

    UIWidgets::Spacer(2.0f);

    std::string fireLabel = "Fire##" + std::string(definition.name);
    if (definition.IsTimed()) {
        float half = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
        if (UIWidgets::Button(fireLabel.c_str(),
                              { .size = ImVec2(half, 0), .color = ValenceColor(definition.valence) })) {
            FireAction(editor, definition);
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(active == nullptr);
        if (UIWidgets::Button(("Cancel##" + std::string(definition.name)).c_str(),
                              { .size = ImVec2(half, 0), .color = UIWidgets::Colors::Gray })) {
            GameInteractor::Instance->CancelAction(definition.id);
        }
        ImGui::EndDisabled();
    } else if (UIWidgets::Button(fireLabel.c_str(),
                                 { .size = UIWidgets::Sizes::Fill, .color = ValenceColor(definition.valence) })) {
        FireAction(editor, definition);
    }

    if (!editor.lastError.empty()) {
        ImGui::TextColored(Color(UIWidgets::Colors::Red), "%s", editor.lastError.c_str());
    }

    UIWidgets::EndCard();
}

void DrawActionsTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("actionsTab", ImVec2(0, 0), true);

    UIWidgets::Checkbox("Queue as session lifetime", &queueAsSession, { .color = UIWidgets::Colors::Gray });
    UIWidgets::Tooltip("Session requests survive loading a different save, the way Sail's and CrowdControl's do. "
                       "Save-scoped ones are dropped on save load.");

    ImGui::Text("Expires after");
    ImGui::SameLine();
    ImGui::TextColored(Color(UIWidgets::Colors::Gray), "(frames, 0 = wait forever)");
    UIWidgets::PushStyleInput(UIWidgets::Colors::Gray);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::InputInt("##expiresAfter", &expiresAfter);
    UIWidgets::PopStyleInput();
    UIWidgets::Tooltip("How long an action waits for the gate to open before giving up and reporting EXPIRED. "
                       "Set this, then fire something during a cutscene.");

    UIWidgets::Separator();

    UIWidgets::BeginCardLayout({ .columnsPerRow = 2, .minColumnWidth = 220.0f });
    for (const GIActions::Definition* definition : SortedActions()) {
        DrawActionCard(*definition);
    }
    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void DrawQueueTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("queueTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout({ .columnsPerRow = 1 });

    UIWidgets::BeginCard("pendingCard");
    const auto& pending = GameInteractor::Instance->PendingActions();
    ImGui::Text("Pending (%zu)", pending.size());
    Hint("Queued, not started. An action that isn't ready keeps its place while ready ones go ahead.");
    if (pending.empty()) {
        UIWidgets::Spacer(2.0f);
        ImGui::TextColored(Color(UIWidgets::Colors::Gray), "Nothing queued.");
    } else if (ImGui::BeginTable("##pending", 4,
                                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("Waiting");
        ImGui::TableSetupColumn("Lifetime");
        ImGui::TableSetupColumn("Expires");
        ImGui::TableHeadersRow();
        for (const auto& action : pending) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", ActionName(action.id));
            ImGui::TableNextColumn();
            if (action.framesWaiting > 0) {
                ImGui::TextColored(Color(UIWidgets::Colors::Yellow), "%u frames", action.framesWaiting);
            } else {
                ImGui::TextColored(Color(UIWidgets::Colors::Gray), "-");
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", action.lifetime == GI_LIFETIME_SESSION ? "session" : "save");
            ImGui::TableNextColumn();
            if (action.expiresAfter > 0) {
                ImGui::Text("%u", action.expiresAfter);
            } else {
                ImGui::TextColored(Color(UIWidgets::Colors::Gray), "never");
            }
        }
        ImGui::EndTable();
    }
    UIWidgets::EndCard();

    UIWidgets::BeginCard("activeCard");
    const auto& active = GameInteractor::Instance->ActiveActions();
    ImGui::Text("Active (%zu)", active.size());
    Hint("Timed actions ticking now. These don't block the queue.");

    std::optional<GIActionId> toCancel = std::nullopt;
    if (active.empty()) {
        UIWidgets::Spacer(2.0f);
        ImGui::TextColored(Color(UIWidgets::Colors::Gray), "Nothing running.");
    } else if (ImGui::BeginTable("##active", 3,
                                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("Progress");
        ImGui::TableSetupColumn("##cancel", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (const auto& action : active) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", ActionName(action.id));
            ImGui::TableNextColumn();
            DrawProgress(action);
            ImGui::TableNextColumn();
            ImGui::PushID((int)action.id);
            if (UIWidgets::Button("Cancel", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                toCancel = action.id;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    UIWidgets::EndCard();

    UIWidgets::EndCardLayout();

    if (toCancel.has_value()) {
        GameInteractor::Instance->CancelAction(*toCancel);
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

} // namespace

void ActionDebuggerWindow::DrawElement() {
    if (gPlayState == NULL) {
        ImGui::TextColored(Color(UIWidgets::Colors::Yellow),
                           "Not in game -- actions can only be queued during gameplay.");
        return;
    }

    DrawStatusBar();

    UIWidgets::PushStyleTabs(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
    if (ImGui::BeginTabBar("ActionDebuggerTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Actions")) {
            DrawActionsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Queue")) {
            DrawQueueTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();
}
