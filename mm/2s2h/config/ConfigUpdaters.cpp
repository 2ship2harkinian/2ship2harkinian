#include "2s2h/config/ConfigUpdaters.h"

#include "2s2h/BenGui/Notification.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/Enhancements/Trackers/DisplayOverlay.h"

namespace Ben {

struct Migration {
    const char* from;
    const char* to;
};

static void ApplyMigrationActions(const Migration* migrations) {
    while (migrations->from != nullptr) {
        if (migrations->to != nullptr) {
            CVarCopy(migrations->from, migrations->to);
        }
        CVarClear(migrations->from);
        migrations++;
    }
}

static const Migration version1Migrations[] = {
    { "gFixes.FixAmmoCountEnvColor", "gFixes.FixButtonEnvColor" },
    { nullptr, nullptr },
};

static bool HasDisplayOverlayModeInConfig(Ship::Config* conf) {
    if (!conf->GetNestedJson().contains("CVars")) {
        return false;
    }

    const auto& cvars = conf->GetNestedJson()["CVars"];
    return cvars.contains("gDisplayOverlay") && cvars["gDisplayOverlay"].is_object() &&
           cvars["gDisplayOverlay"].contains("Mode");
}

static void MigrateDisplayOverlayTimerMode(Ship::Config* conf) {
    if (!HasDisplayOverlayModeInConfig(conf)) {
        return;
    }

    int oldVal = CVarGetInteger("gWindows.DisplayOverlay", 0);
    if (oldVal == TIMER_DISPLAY_RTA || oldVal == TIMER_DISPLAY_IGT) {
        CVarSetInteger(CVAR_DISPLAY_OVERLAY_MODE, oldVal);
        CVarSetInteger("gWindows.DisplayOverlay", 1);
    } else {
        CVarSetInteger(CVAR_DISPLAY_OVERLAY_MODE, TIMER_DISPLAY_NONE);
        CVarSetInteger("gWindows.DisplayOverlay", 0);
    }
}

static void MigrateWarpPoints(Ship::Config* conf) {
    if (conf->GetNestedJson().contains("CVars")) {
        if (CVarGetInteger("gDeveloperTools.WarpPoint.Saved", 0) != 0) {
            auto warpPoints = nlohmann::json::object();

            s32 entranceId = CVarGetInteger("gDeveloperTools.WarpPoint.Entrance", 0);
            s8 roomNum = CVarGetInteger("gDeveloperTools.WarpPoint.Room", 0);
            Vec3f pos = { CVarGetFloat("gDeveloperTools.WarpPoint.X", 0.0f),
                          CVarGetFloat("gDeveloperTools.WarpPoint.Y", 0.0f),
                          CVarGetFloat("gDeveloperTools.WarpPoint.Z", 0.0f) };
            s16 rotY = CVarGetFloat("gDeveloperTools.WarpPoint.Rotation", 0.0f);
            bool bootToPoint = CVarGetInteger("gDeveloperTools.WarpPoint.BootToWarpPoint", 0) > 0;
            warpPoints["Debug Warp Point"] = { { "entranceId", entranceId },
                                               { "roomNum", roomNum },
                                               { "pos", { { "x", pos.x }, { "y", pos.y }, { "z", pos.z } } },
                                               { "rotY", rotY },
                                               { "bootToPoint", bootToPoint } };

            Ship::Context::GetRawInstance()->GetConfig()->SetBlock("WarpPoints", warpPoints);
        }
    }
}

static void MigrateGreatFairySwordBButton(Ship::Config* conf) {
    if (!conf->GetNestedJson().contains("CVars")) {
        return;
    }
    if (CVarGetInteger("gEnhancements.Equipment.GreatFairySwordBButton", 0) != 0) {
        CVarSetInteger("gEnhancements.Equipment.ActiveItemOnB", 1);

        Notification::Emit({
            .prefix = "Settings updated:",
            .message = "Great Fairy Sword B-Button Attack is now part of Active Item on B.",
            .suffix = "It has been enabled for you.",
            .remainingTime = 10.0f,
        });
    }

    CVarClear("gEnhancements.Equipment.GreatFairySwordBButton");
}

ConfigVersion1Updater::ConfigVersion1Updater() : ConfigVersionUpdater(1) {
}

void ConfigVersion1Updater::Update(Ship::Config* conf) {
    ApplyMigrationActions(version1Migrations);
    MigrateDisplayOverlayTimerMode(conf);
    MigrateWarpPoints(conf);
}

ConfigVersion2Updater::ConfigVersion2Updater() : ConfigVersionUpdater(2) {
}

void ConfigVersion2Updater::Update(Ship::Config* conf) {
    MigrateGreatFairySwordBButton(conf);
}

} // namespace Ben
