#include "MicOcarinaWindow.h"
#include "MicOcarina.h"
#include <cmath>
#include <cfloat>
#include <string>
#include <imgui.h>
#include <libultraship/bridge/consolevariablebridge.h>

void MicOcarinaWindow::DrawElement() {
    MicOcarinaDebug d;
    MicOcarina_GetDebug(&d);

    // ---- status ----
    ImGui::Text("Enhancement: %s", d.enabled ? "ON" : "off (enable in Enhancements > Items/Songs)");
    ImGui::Text("Capture device: %s", d.deviceOpen ? "open" : "not open (opens when the ocarina comes out)");
    ImGui::Text("Ocarina hook calls: %d", d.callCount);
    ImGui::Separator();

    // ---- live input ----
    if (ImGui::BeginTable("levels", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableNextColumn();
        ImGui::Text("Level");
        ImGui::SameLine();
        ImGui::ProgressBar(d.rms / 3000.0f, ImVec2(-FLT_MIN, 0), "");
        ImGui::TableNextColumn();
        ImGui::Text("Tonality");
        ImGui::SameLine();
        ImGui::ProgressBar(d.tonality, ImVec2(-FLT_MIN, 0), "");
        ImGui::EndTable();
    }
    if (d.rms < 300.0f && d.rms > 0.0f) {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "input is quiet — blow closer to the mic");
    }
    if (d.pitchHz > 0) {
        ImGui::Text("Pitch: %.1f Hz  (%+.2f semitones vs D4)", d.pitchHz, d.pitchSemis);
    } else {
        ImGui::TextDisabled("Pitch: (none)");
    }
    if (d.playingSong >= 0) {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Playing matched song %d, note %d", d.playingSong,
                           d.playingNote);
    } else if (d.inNote) {
        ImGui::Text("Current note: %+.2f semis (%d frames)", d.noteSemis, d.noteFrames);
    }
    ImGui::Separator();

    // ---- tracked notes ----
    ImGui::Text("Tracked notes (newest last):");
    if (d.eventCount == 0) {
        ImGui::TextDisabled("  (nothing yet)");
    }
    for (int i = 0; i < d.eventCount; i++) {
        ImGui::BulletText("%+.2f semis (%.0f Hz), %d frames", d.eventSemis[i], 293.66f * powf(2.0f, d.eventSemis[i] / 12.0f),
                          d.eventFrames[i]);
    }
    ImGui::Separator();

    // ---- song match progress ----
    ImGui::Text("Song matching (unlocked songs, trailing progress):");
    for (int i = 0; i < d.songCount; i++) {
        if (!d.songUnlocked[i]) {
            continue;
        }
        // songRun counts matched intervals; a full song has pitchCount-1 of them.
        const float frac = (d.songRun[i] > 0 && d.songPitchCount[i] > 1)
                               ? ((float)d.songRun[i] / (float)(d.songPitchCount[i] - 1))
                               : 0.0f;
        ImGui::PushID(i);
        ImGui::Text("%s", d.songNames[i] ? d.songNames[i] : "?");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::ProgressBar(frac, ImVec2(-FLT_MIN, 0), d.songRun[i] > 0 ? std::to_string(d.songRun[i]).c_str() : "");
        ImGui::PopID();
    }
}
