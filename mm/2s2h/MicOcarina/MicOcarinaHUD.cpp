#include "MicOcarinaHUD.h"
#include "MicOcarina.h"
#include <cmath>
#include <cstdio>
#include <vector>
#include <imgui.h>
#include <ship/window/gui/IconsFontAwesome4.h>
#include <libultraship/bridge/consolevariablebridge.h>

namespace {
constexpr int kBars = (int)(sizeof(((MicOcarinaDebug*)0)->waveform) / sizeof(float)); // matches MicOcarina.h
constexpr float kGate = 300.0f;         // silence gate — matches kRmsGate in MicOcarina.cpp
constexpr float kFull = 3000.0f;        // mic level that fills a waveform bar
constexpr float kActiveTimeout = 0.25f; // seconds without hook activity = ocarina put away

// The game's five ocarina pitches in semitones above D4 — matches MicOcarina.cpp.
constexpr float kPitchSemis[5] = { 0, 3, 7, 9, 12 };
const char* const kNoteLabels[5] = { "D4 (A)", "F4 (C-down)", "A4 (C-right)", "B4 (C-left)", "D5 (C-up)" };
} // namespace

void MicOcarinaHUD::Draw() {
    MicOcarina_IdleCheck(); // release the mic when the ocarina is away

    MicOcarinaDebug d;
    MicOcarina_GetDebug(&d);

    // AudioOcarina_ReadControllerInput (and with it MicOcarina_GetButtons) only
    // runs while the ocarina is out and taking input, so callCount stops
    // advancing once the ocarina is put away. A short timeout smooths over the
    // gap between game ticks; the HUD renders faster than the game updates.
    static int sLastCount = 0;
    static double sLastActive = -1000.0;
    const bool countAdvanced = d.callCount != sLastCount;
    if (countAdvanced) {
        sLastCount = d.callCount;
        sLastActive = ImGui::GetTime();
    }
    if (!d.enabled || ImGui::GetTime() - sLastActive > kActiveTimeout) {
        return;
    }

    const bool matched = d.playingSong >= 0 && d.playingName != nullptr && d.playingName[0] != '\0';
    const bool pending = matched && d.matchPending == 1; // green beat before playback
    // Hysteresis so the label doesn't flicker when ambient noise sits at the
    // gate: switch to receiving well above the gate, back to waiting well below.
    static bool sReceiving = false;
    if (!sReceiving && d.rms >= kGate * 1.6f) {
        sReceiving = true;
    } else if (sReceiving && d.rms < kGate) {
        sReceiving = false;
    }
    const bool receiving = sReceiving;
    const bool live = receiving || matched;
    const ImVec4 waitingColor(0.72f, 0.75f, 0.80f, 1.0f);
    const ImVec4 receivingColor(0.40f, 0.95f, 0.55f, 1.0f);
    const ImVec4 matchedColor(1.0f, 0.82f, 0.35f, 1.0f);
    const ImVec4 accent =
        pending ? receivingColor : (matched ? matchedColor : (receiving ? receivingColor : waitingColor));

    // anchor: 0 top-left, 1 top-right, 2 bottom-left, 3 bottom-right, 4 bottom-center, 5 top-center, 6 center
    const int corner = CVarGetInteger("gEnhancements.Playback.MicOcarinaHUDCorner", 5);
    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const float margin = 16.0f;
    ImVec2 anchor = ImVec2(margin, margin);
    ImVec2 pivot(0.0f, 0.0f);
    switch (corner) {
        case 1:
            anchor = ImVec2(display.x - margin, margin);
            pivot = ImVec2(1.0f, 0.0f);
            break;
        case 2:
            anchor = ImVec2(margin, display.y - margin);
            pivot = ImVec2(0.0f, 1.0f);
            break;
        case 3:
            anchor = ImVec2(display.x - margin, display.y - margin);
            pivot = ImVec2(1.0f, 1.0f);
            break;
        case 5:
            anchor = ImVec2(display.x * 0.5f, margin * 2.0f);
            pivot = ImVec2(0.5f, 0.0f);
            break;
        case 6:
            anchor = ImVec2(display.x * 0.5f, display.y * 0.5f);
            pivot = ImVec2(0.5f, 0.5f);
            break;
        case 0:
            break;
        default:
            anchor = ImVec2(display.x * 0.5f, display.y - margin * 3.0f);
            pivot = ImVec2(0.5f, 1.0f);
            break;
    }
    ImGui::SetNextWindowPos(anchor, ImGuiCond_Always, pivot);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // transparent: we draw separate pills ourselves
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;
    if (ImGui::Begin("###MicOcarinaHUD", nullptr, flags)) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImU32 accentCol = ImGui::GetColorU32(accent);
        const ImU32 pillBg = ImGui::GetColorU32(ImVec4(0.06f, 0.07f, 0.09f, 0.88f));
        const float pulse = live ? 0.5f + 0.5f * sinf((float)ImGui::GetTime() * 6.0f) : 0.0f;
        const float gap = 8.0f;

        // text for the status pill
        char status[96];
        char sub[64];
        if (matched) {
            snprintf(status, sizeof(status), pending ? "Song recognized: %s" : "Playing: %s", d.playingName);
        } else {
            snprintf(status, sizeof(status), "%s", receiving ? "Receiving input" : "Waiting for input");
        }
        snprintf(sub, sizeof(sub), "%s",
                 matched ? (pending ? "Get ready..." : "Sending to the game")
                         : (receiving ? "Hearing you" : "Play or sing your ocarina"));
        const ImVec2 statusSz = ImGui::CalcTextSize(status);
        const ImVec2 subSz = ImGui::CalcTextSize(sub);
        const float textW = fmaxf(statusSz.x, subSz.x);

        const float badge = 44.0f;
        const float pillH = 44.0f;
        const float pillW = textW + 32.0f;
        const float waveW = badge + gap + pillW;

        const ImVec2 o = ImGui::GetCursorScreenPos();
        ImGui::Dummy(ImVec2(waveW, pillH));

        // piece 1: round mic badge, glowing while live
        const ImVec2 bC(o.x + badge * 0.5f, o.y + pillH * 0.5f);
        if (live) {
            dl->AddCircleFilled(bC, badge * 0.5f + 3.0f * pulse,
                                ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.12f + 0.12f * pulse)), 32);
        }
        dl->AddCircleFilled(bC, badge * 0.5f, pillBg, 32);
        dl->AddCircle(bC, badge * 0.5f - 1.0f, accentCol, 32, 1.5f);
        const ImVec2 iconSz = ImGui::CalcTextSize(ICON_FA_MICROPHONE);
        dl->AddText(ImVec2(bC.x - iconSz.x * 0.5f, bC.y - iconSz.y * 0.5f), accentCol, ICON_FA_MICROPHONE);

        // piece 2: status pill
        const ImVec2 pMin(o.x + badge + gap, o.y);
        dl->AddRectFilled(pMin, ImVec2(pMin.x + pillW, pMin.y + pillH), pillBg, pillH * 0.5f);
        dl->AddText(ImVec2(pMin.x + 16.0f, pMin.y + 6.0f), accentCol, status);
        dl->AddText(ImVec2(pMin.x + 16.0f, pMin.y + 6.0f + statusSz.y + 1.0f),
                    ImGui::GetColorU32(ImGuiCol_TextDisabled), sub);
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();

    // ---- ghostly piano roll over the middle of the screen ----
    // Time runs left->right (newest at the right edge); pitch runs bottom->top.
    // While a note sounds the trace sits on that pitch's lane, so playing a
    // melody draws steps up and down. The view auto-centres on what you play
    // (matching is key-agnostic, so absolute pitch doesn't matter).
    struct Sample {
        double t;
        float semis;
        bool on;
    };
    static std::vector<Sample> sTrail;
    static float sCenter = 0.0f;
    static float sSpan = 16.0f;
    const double now = ImGui::GetTime();
    const bool sounding = d.inNote && !matched && d.rms >= kGate;
    sTrail.push_back({ now, d.noteSemis, sounding });
    constexpr double kHistory = 6.0;
    while (!sTrail.empty() && now - sTrail.front().t > kHistory) {
        sTrail.erase(sTrail.begin());
    }

    float lo = 1e9f, hi = -1e9f;
    for (const Sample& sm : sTrail) {
        if (sm.on) {
            lo = fminf(lo, sm.semis);
            hi = fmaxf(hi, sm.semis);
        }
    }
    if (hi >= lo) {
        sCenter += ((lo + hi) * 0.5f - sCenter) * 0.08f;
        sSpan += (fmaxf(16.0f, hi - lo + 8.0f) - sSpan) * 0.08f;
    }

    ImDrawList* bg = ImGui::GetForegroundDrawList();
    const float rw = display.x * 0.62f;
    const float rh = display.y * 0.42f;
    const ImVec2 rc(display.x * 0.5f, display.y * 0.5f);
    const ImVec2 rMin(rc.x - rw * 0.5f, rc.y - rh * 0.5f);
    const ImVec2 rMax(rc.x + rw * 0.5f, rc.y + rh * 0.5f);
    const float master = live ? 1.0f : 0.55f; // fades back when nothing is playing
    auto yOf = [&](float semis) { return rc.y - (semis - sCenter) / sSpan * rh; };

    // note lines: one per semitone, very faint; the lane being played glows
    const int firstLane = (int)floorf(sCenter - sSpan * 0.5f);
    const int lastLane = (int)ceilf(sCenter + sSpan * 0.5f);
    const float curSemis = sounding ? roundf(d.noteSemis) : -1000.0f;
    for (int n = firstLane; n <= lastLane; n++) {
        const float y = yOf((float)n);
        if (y < rMin.y || y > rMax.y) {
            continue;
        }
        const float edge = 1.0f - fabsf(y - rc.y) / (rh * 0.5f); // fade toward top/bottom
        const bool active = (float)n == curSemis;
        const float a = (active ? 0.30f : 0.07f) * edge * master;
        bg->AddLine(ImVec2(rMin.x, y), ImVec2(rMax.x, y), ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, a)),
                    active ? 2.0f : 1.0f);
    }

    // the note trail: thick translucent horizontal strokes with a glow
    const ImVec4 tail(0.62f, 0.45f, 1.0f, 1.0f);
    for (size_t i = 1; i < sTrail.size(); i++) {
        const Sample& p0 = sTrail[i - 1];
        const Sample& p1 = sTrail[i];
        if (!p0.on || !p1.on) {
            continue;
        }
        const float age = (float)((now - p1.t) / kHistory); // 0 newest .. 1 oldest
        const float t = 1.0f - age;
        const float x0 = rMax.x - (float)((now - p0.t) / kHistory) * rw;
        const float x1 = rMax.x - age * rw;
        const ImVec4 c(tail.x + (accent.x - tail.x) * t, tail.y + (accent.y - tail.y) * t,
                       tail.z + (accent.z - tail.z) * t, 1.0f);
        const ImVec2 a(x0, yOf(p0.semis));
        const ImVec2 b(x1, yOf(p1.semis));
        bg->AddLine(a, b, ImGui::GetColorU32(ImVec4(c.x, c.y, c.z, 0.10f * t * master)), 16.0f);
        bg->AddLine(a, b, ImGui::GetColorU32(ImVec4(c.x, c.y, c.z, 0.38f * t * master)), 6.0f);
    }
    if (sounding) {
        bg->AddCircleFilled(ImVec2(rMax.x, yOf(d.noteSemis)), 7.0f,
                            ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.55f)));
    }
}
