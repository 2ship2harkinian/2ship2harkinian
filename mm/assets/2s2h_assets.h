#pragma once

#include "align_asset_macro.h"

// This file is manually made
// When new assets are added to the 2ship.otr file
// We need to add the aligned version of the resource names here and use in code
// On Mac, not using aligned resource names was causing crashes in release builds

// Custom assets that were removed and have not been replaced yet. Code using them must provide a fallback.
#define dgMissingAsset ""
#define IS_MISSING_ASSET(asset) ((asset)[0] == '\0')
#define ASSET_OR(asset, fallback) (IS_MISSING_ASSET(asset) ? (fallback) : (asset))

// textures
#define dgDPad "__OTR__textures/parameter_static/gDPad"
static const ALIGN_ASSET(2) char gDPadTex[] = dgDPad;

#define dgArrowUp "__OTR__textures/parameter_static/gArrowUp"
static const ALIGN_ASSET(2) char gArrowUpTex[] = dgArrowUp;

#define dgArrowDown "__OTR__textures/parameter_static/gArrowDown"
static const ALIGN_ASSET(2) char gArrowDownTex[] = dgArrowDown;

#define dgTriforcePiece "__OTR__textures/parameter_static/gTriforcePiece"
static const ALIGN_ASSET(2) char gTriforcePieceTex[] = dgTriforcePiece;

#define dgFlippers "__OTR__textures/parameter_static/gFlippers"
static const ALIGN_ASSET(2) char gFlippersTex[] = dgFlippers;

#define dgThreeDayClockHour13Tex "__OTR__textures/parameter_static/gThreeDayClockHour13Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour13Tex[] = dgThreeDayClockHour13Tex;

#define dgThreeDayClockHour14Tex "__OTR__textures/parameter_static/gThreeDayClockHour14Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour14Tex[] = dgThreeDayClockHour14Tex;

#define dgThreeDayClockHour15Tex "__OTR__textures/parameter_static/gThreeDayClockHour15Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour15Tex[] = dgThreeDayClockHour15Tex;

#define dgThreeDayClockHour16Tex "__OTR__textures/parameter_static/gThreeDayClockHour16Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour16Tex[] = dgThreeDayClockHour16Tex;

#define dgThreeDayClockHour17Tex "__OTR__textures/parameter_static/gThreeDayClockHour17Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour17Tex[] = dgThreeDayClockHour17Tex;

#define dgThreeDayClockHour18Tex "__OTR__textures/parameter_static/gThreeDayClockHour18Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour18Tex[] = dgThreeDayClockHour18Tex;

#define dgThreeDayClockHour19Tex "__OTR__textures/parameter_static/gThreeDayClockHour19Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour19Tex[] = dgThreeDayClockHour19Tex;

#define dgThreeDayClockHour20Tex "__OTR__textures/parameter_static/gThreeDayClockHour20Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour20Tex[] = dgThreeDayClockHour20Tex;

#define dgThreeDayClockHour21Tex "__OTR__textures/parameter_static/gThreeDayClockHour21Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour21Tex[] = dgThreeDayClockHour21Tex;

#define dgThreeDayClockHour22Tex "__OTR__textures/parameter_static/gThreeDayClockHour22Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour22Tex[] = dgThreeDayClockHour22Tex;

#define dgThreeDayClockHour23Tex "__OTR__textures/parameter_static/gThreeDayClockHour23Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour23Tex[] = dgThreeDayClockHour23Tex;

#define dgThreeDayClockHour24Tex "__OTR__textures/parameter_static/gThreeDayClockHour24Tex"
static const ALIGN_ASSET(2) char gThreeDayClockHour24Tex[] = dgThreeDayClockHour24Tex;

#define dgEmptyTexture "__OTR__textures/virtual/gEmptyTexture"
static const ALIGN_ASSET(2) char gEmptyTexture[] = dgEmptyTexture;

#define dgThreeDayClock3DSEdgeTex "__OTR__textures/parameter_static/gThreeDayClock3DSEdgeTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSEdgeTex[] = dgThreeDayClock3DSEdgeTex;

#define dgThreeDayClock3DSMiddleTex "__OTR__textures/parameter_static/gThreeDayClock3DSMiddleTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSMiddleTex[] = dgThreeDayClock3DSMiddleTex;

#define dgThreeDayClock3DSFillTex "__OTR__textures/parameter_static/gThreeDayClock3DSFillTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFillTex[] = dgThreeDayClock3DSFillTex;

#define dgThreeDayClock3DSArrowTex "__OTR__textures/parameter_static/gThreeDayClock3DSArrowTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSArrowTex[] = dgThreeDayClock3DSArrowTex;

#define dgThreeDayClock3DSTimeBackdropTex "__OTR__textures/parameter_static/gThreeDayClock3DSTimeBackdropTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSTimeBackdropTex[] = dgThreeDayClock3DSTimeBackdropTex;

#define dgThreeDayClock3DSSlowTimeTex "__OTR__textures/parameter_static/gThreeDayClock3DSSlowTimeTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSSlowTimeTex[] = dgThreeDayClock3DSSlowTimeTex;

#define dgThreeDayClock3DSFinalHoursMoonTex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursMoonTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursMoonTex[] = dgThreeDayClock3DSFinalHoursMoonTex;

#define dgThreeDayClock3DSFinalHoursDigit0Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit0Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit0Tex[] = dgThreeDayClock3DSFinalHoursDigit0Tex;

#define dgThreeDayClock3DSFinalHoursDigit1Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit1Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit1Tex[] = dgThreeDayClock3DSFinalHoursDigit1Tex;

#define dgThreeDayClock3DSFinalHoursDigit2Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit2Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit2Tex[] = dgThreeDayClock3DSFinalHoursDigit2Tex;

#define dgThreeDayClock3DSFinalHoursDigit3Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit3Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit3Tex[] = dgThreeDayClock3DSFinalHoursDigit3Tex;

#define dgThreeDayClock3DSFinalHoursDigit4Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit4Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit4Tex[] = dgThreeDayClock3DSFinalHoursDigit4Tex;

#define dgThreeDayClock3DSFinalHoursDigit5Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit5Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit5Tex[] = dgThreeDayClock3DSFinalHoursDigit5Tex;

#define dgThreeDayClock3DSFinalHoursDigit6Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit6Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit6Tex[] = dgThreeDayClock3DSFinalHoursDigit6Tex;

#define dgThreeDayClock3DSFinalHoursDigit7Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit7Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit7Tex[] = dgThreeDayClock3DSFinalHoursDigit7Tex;

#define dgThreeDayClock3DSFinalHoursDigit8Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit8Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit8Tex[] = dgThreeDayClock3DSFinalHoursDigit8Tex;

#define dgThreeDayClock3DSFinalHoursDigit9Tex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursDigit9Tex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursDigit9Tex[] = dgThreeDayClock3DSFinalHoursDigit9Tex;

#define dgThreeDayClock3DSFinalHoursColonTex "__OTR__textures/parameter_static/gThreeDayClock3DSFinalHoursColonTex"
static const ALIGN_ASSET(2) char gThreeDayClock3DSFinalHoursColonTex[] = dgThreeDayClock3DSFinalHoursColonTex;

#define dgPauseSavePromptENGTex "__OTR__textures/icon_item_static/gPauseSavePromptENGTex"
static const ALIGN_ASSET(2) char gPauseSavePromptENGTex[] = dgPauseSavePromptENGTex;

#define dgPauseYesENGTex "__OTR__textures/icon_item_static/gPauseYesENGTex"
static const ALIGN_ASSET(2) char gPauseYesENGTex[] = dgPauseYesENGTex;

#define dgPauseNoENGTex "__OTR__textures/icon_item_static/gPauseNoENGTex"
static const ALIGN_ASSET(2) char gPauseNoENGTex[] = dgPauseNoENGTex;

#define dgPauseSaveConfirmationENGTex "__OTR__textures/icon_item_static/gPauseSaveConfirmationENGTex"
static const ALIGN_ASSET(2) char gPauseSaveConfirmationENGTex[] = dgPauseSaveConfirmationENGTex;

#define dgContinuePlayingENGTex "__OTR__textures/icon_item_static/gContinuePlayingENGTex"
static const ALIGN_ASSET(2) char gContinuePlayingENGTex[] = dgContinuePlayingENGTex;

#define dgFileSelFourthDayTex "__OTR__misc/title_static/gFileSelFourthDayTex"
static const ALIGN_ASSET(2) char gFileSelFourthDayTex[] = dgFileSelFourthDayTex;

#define dgFileSelCheatingDayTex "__OTR__misc/title_static/gFileSelCheatingDayTex"
static const ALIGN_ASSET(2) char gFileSelCheatingDayTex[] = dgFileSelCheatingDayTex;

#define dgFileSelRandIconTex "__OTR__misc/title_static/gFileSelRandIconTex"
static const ALIGN_ASSET(2) char gFileSelRandIconTex[] = dgFileSelRandIconTex;

// File select new file setup labels, generated by tools/label_gen/genlabel.py
#define dgFileSelNewFileTex "__OTR__misc/title_static/gFileSelNewFileTex"
static const ALIGN_ASSET(2) char gFileSelNewFileTex[] = dgFileSelNewFileTex;

#define dgFileSelModeHeaderTex "__OTR__misc/title_static/gFileSelModeHeaderTex"
static const ALIGN_ASSET(2) char gFileSelModeHeaderTex[] = dgFileSelModeHeaderTex;

#define dgFileSelSeedHeaderTex "__OTR__misc/title_static/gFileSelSeedHeaderTex"
static const ALIGN_ASSET(2) char gFileSelSeedHeaderTex[] = dgFileSelSeedHeaderTex;

#define dgFileSelPresetHeaderTex "__OTR__misc/title_static/gFileSelPresetHeaderTex"
static const ALIGN_ASSET(2) char gFileSelPresetHeaderTex[] = dgFileSelPresetHeaderTex;

#define dgFileSelGenerateNewTex "__OTR__misc/title_static/gFileSelGenerateNewTex"
static const ALIGN_ASSET(2) char gFileSelGenerateNewTex[] = dgFileSelGenerateNewTex;

#define dgFileSelVanillaTex "__OTR__misc/title_static/gFileSelVanillaTex"
static const ALIGN_ASSET(2) char gFileSelVanillaTex[] = dgFileSelVanillaTex;

#define dgFileSelRandomizerTex "__OTR__misc/title_static/gFileSelRandomizerTex"
static const ALIGN_ASSET(2) char gFileSelRandomizerTex[] = dgFileSelRandomizerTex;

#define dgFileSelArrowLeftTex "__OTR__misc/title_static/gFileSelArrowLeftTex"
static const ALIGN_ASSET(2) char gFileSelArrowLeftTex[] = dgFileSelArrowLeftTex;

#define dgFileSelArrowRightTex "__OTR__misc/title_static/gFileSelArrowRightTex"
static const ALIGN_ASSET(2) char gFileSelArrowRightTex[] = dgFileSelArrowRightTex;

#define dgBoxChestCornerHealthTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerHealthTex[] = dgBoxChestCornerHealthTex;

#define dgBoxChestCornerLesserTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerLesserTex[] = dgBoxChestCornerLesserTex;

#define dgBoxChestCornerMajorTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerMajorTex[] = dgBoxChestCornerMajorTex;

#define dgBoxChestCornerMaskTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerMaskTex[] = dgBoxChestCornerMaskTex;

#define dgBoxChestCornerSkullTokenTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerSkullTokenTex[] = dgBoxChestCornerSkullTokenTex;

#define dgBoxChestCornerSmallKeyTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerSmallKeyTex[] = dgBoxChestCornerSmallKeyTex;

#define dgBoxChestCornerStrayFairyTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestCornerStrayFairyTex[] = dgBoxChestCornerStrayFairyTex;

#define dgBoxChestLockHealthTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockHealthTex[] = dgBoxChestLockHealthTex;

#define dgBoxChestLockLesserTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockLesserTex[] = dgBoxChestLockLesserTex;

#define dgBoxChestLockMajorTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockMajorTex[] = dgBoxChestLockMajorTex;

#define dgBoxChestLockMaskTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockMaskTex[] = dgBoxChestLockMaskTex;

#define dgBoxChestLockSkullTokenTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockSkullTokenTex[] = dgBoxChestLockSkullTokenTex;

#define dgBoxChestLockSmallKeyTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockSmallKeyTex[] = dgBoxChestLockSmallKeyTex;

#define dgBoxChestLockStrayFairyTex dgMissingAsset
static const ALIGN_ASSET(2) char gBoxChestLockStrayFairyTex[] = dgBoxChestLockStrayFairyTex;

#define dgPotBossKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotBossKeyDL[] = dgPotBossKeyDL;

#define dgPotFairyDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotFairyDL[] = dgPotFairyDL;

#define dgPotHeartDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotHeartDL[] = dgPotHeartDL;

#define dgPotMajorDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotMajorDL[] = dgPotMajorDL;

#define dgPotMaskDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotMaskDL[] = dgPotMaskDL;

#define dgPotMinorDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotMinorDL[] = dgPotMinorDL;

#define dgPotRandomDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotRandomDL[] = dgPotRandomDL;

#define dgPotSmallKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotSmallKeyDL[] = dgPotSmallKeyDL;

#define dgPotStandardDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotStandardDL[] = dgPotStandardDL;

#define dgPotTokenDL dgMissingAsset
static const ALIGN_ASSET(2) char gPotTokenDL[] = dgPotTokenDL;

#define dgLargeMajorCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeMajorCrateDL[] = dgLargeMajorCrateDL;

#define dgLargeMaskCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeMaskCrateDL[] = dgLargeMaskCrateDL;

#define dgLargeMinorCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeMinorCrateDL[] = dgLargeMinorCrateDL;

#define dgLargeRandoCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeRandoCrateDL[] = dgLargeRandoCrateDL;

#define dgLargeSmallKeyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeSmallKeyCrateDL[] = dgLargeSmallKeyCrateDL;

#define dgLargeTokenCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeTokenCrateDL[] = dgLargeTokenCrateDL;

#define dgLargeBossKeyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeBossKeyCrateDL[] = dgLargeBossKeyCrateDL;

#define dgLargeFairyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeFairyCrateDL[] = dgLargeFairyCrateDL;

#define dgLargeHeartCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeHeartCrateDL[] = dgLargeHeartCrateDL;

#define dgLargeJunkCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gLargeJunkCrateDL[] = dgLargeJunkCrateDL;

#define dgSmallMajorCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallMajorCrateDL[] = dgSmallMajorCrateDL;

#define dgSmallMaskCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallMaskCrateDL[] = dgSmallMaskCrateDL;

#define dgSmallMinorCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallMinorCrateDL[] = dgSmallMinorCrateDL;

#define dgSmallRandoCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallRandoCrateDL[] = dgSmallRandoCrateDL;

#define dgSmallSmallKeyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallSmallKeyCrateDL[] = dgSmallSmallKeyCrateDL;

#define dgSmallTokenCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallTokenCrateDL[] = dgSmallTokenCrateDL;

#define dgSmallBossKeyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallBossKeyCrateDL[] = dgSmallBossKeyCrateDL;

#define dgSmallFairyCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallFairyCrateDL[] = dgSmallFairyCrateDL;

#define dgSmallHeartCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallHeartCrateDL[] = dgSmallHeartCrateDL;

#define dgSmallJunkCrateDL dgMissingAsset
static const ALIGN_ASSET(2) char gSmallJunkCrateDL[] = dgSmallJunkCrateDL;

#define dgBarrelMajorDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelMajorDL[] = dgBarrelMajorDL;

#define dgBarrelMaskDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelMaskDL[] = dgBarrelMaskDL;

#define dgBarrelMinorDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelMinorDL[] = dgBarrelMinorDL;

#define dgBarrelRandoDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelRandoDL[] = dgBarrelRandoDL;

#define dgBarrelSmallKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelSmallKeyDL[] = dgBarrelSmallKeyDL;

#define dgBarrelTokenDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelTokenDL[] = dgBarrelTokenDL;

#define dgBarrelBossKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelBossKeyDL[] = dgBarrelBossKeyDL;

#define dgBarrelFairyDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelFairyDL[] = dgBarrelFairyDL;

#define dgBarrelHeartDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelHeartDL[] = dgBarrelHeartDL;

#define dgBarrelJunkDL dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelJunkDL[] = dgBarrelJunkDL;

#define dgRandoBushDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushDL[] = dgRandoBushDL;

#define dgRandoBushXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushXluDL[] = dgRandoBushXluDL;

#define dgRandoBushMinorDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMinorDL[] = dgRandoBushMinorDL;

#define dgRandoBushMinorXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMinorXluDL[] = dgRandoBushMinorXluDL;

#define dgRandoBushMajorDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMajorDL[] = dgRandoBushMajorDL;

#define dgRandoBushMajorXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMajorXluDL[] = dgRandoBushMajorXluDL;

#define dgRandoBushSmallKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushSmallKeyDL[] = dgRandoBushSmallKeyDL;

#define dgRandoBushSmallKeyXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushSmallKeyXluDL[] = dgRandoBushSmallKeyXluDL;

#define dgRandoBushBossKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushBossKeyDL[] = dgRandoBushBossKeyDL;

#define dgRandoBushBossKeyXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushBossKeyXluDL[] = dgRandoBushBossKeyXluDL;

#define dgRandoBushTokenDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushTokenDL[] = dgRandoBushTokenDL;

#define dgRandoBushTokenXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushTokenXluDL[] = dgRandoBushTokenXluDL;

#define dgRandoBushMaskDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMaskDL[] = dgRandoBushMaskDL;

#define dgRandoBushMaskXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushMaskXluDL[] = dgRandoBushMaskXluDL;

#define dgRandoBushFairyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushFairyDL[] = dgRandoBushFairyDL;

#define dgRandoBushFairyXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushFairyXluDL[] = dgRandoBushFairyXluDL;

#define dgRandoBushHeartDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushHeartDL[] = dgRandoBushHeartDL;

#define dgRandoBushHeartXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushHeartXluDL[] = dgRandoBushHeartXluDL;

#define dgRandoBushJunkDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushJunkDL[] = dgRandoBushJunkDL;

#define dgRandoBushJunkXluDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoBushJunkXluDL[] = dgRandoBushJunkXluDL;

#define dgRandoCuttableGrassRandomDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassRandomDL[] = dgRandoCuttableGrassRandomDL;

#define dgRandoCuttableGrassMinorDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassMinorDL[] = dgRandoCuttableGrassMinorDL;

#define dgRandoCuttableGrassMajorDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassMajorDL[] = dgRandoCuttableGrassMajorDL;

#define dgRandoCuttableGrassSmallKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassSmallKeyDL[] = dgRandoCuttableGrassSmallKeyDL;

#define dgRandoCuttableGrassBossKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassBossKeyDL[] = dgRandoCuttableGrassBossKeyDL;

#define dgRandoCuttableGrassTokenDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassTokenDL[] = dgRandoCuttableGrassTokenDL;

#define dgRandoCuttableGrassMaskDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassMaskDL[] = dgRandoCuttableGrassMaskDL;

#define dgRandoCuttableGrassFairyDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassFairyDL[] = dgRandoCuttableGrassFairyDL;

#define dgRandoCuttableGrassHeartDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassHeartDL[] = dgRandoCuttableGrassHeartDL;

#define dgRandoCuttableGrassJunkDL dgMissingAsset
static const ALIGN_ASSET(2) char gRandoCuttableGrassJunkDL[] = dgRandoCuttableGrassJunkDL;

#define dgChestTrackerIcon dgMissingAsset
static const ALIGN_ASSET(2) char gChestTrackerIcon[] = dgChestTrackerIcon;

#define dgPotTrackerIcon dgMissingAsset
static const ALIGN_ASSET(2) char gPotTrackerIcon[] = dgPotTrackerIcon;

#define dgCrateTrackerIcon dgMissingAsset
static const ALIGN_ASSET(2) char gCrateTrackerIcon[] = dgCrateTrackerIcon;

#define dgBarrelTrackerIcon dgMissingAsset
static const ALIGN_ASSET(2) char gBarrelTrackerIcon[] = dgBarrelTrackerIcon;

#define dgGiFlippersDL "__OTR__objects/object_ability_swim/gGiFlippersDL"
static const ALIGN_ASSET(2) char gGiFlippersDL[] = dgGiFlippersDL;

#define dgTriforcePiece0DL "__OTR__objects/object_triforce_piece_0/gTriforcePiece0DL"
static const ALIGN_ASSET(2) char gTriforcePiece0DL[] = dgTriforcePiece0DL;

#define dgTriforcePiece1DL "__OTR__objects/object_triforce_piece_1/gTriforcePiece1DL"
static const ALIGN_ASSET(2) char gTriforcePiece1DL[] = dgTriforcePiece1DL;

#define dgTriforcePiece2DL "__OTR__objects/object_triforce_piece_2/gTriforcePiece2DL"
static const ALIGN_ASSET(2) char gTriforcePiece2DL[] = dgTriforcePiece2DL;

#define dgTriforcePieceCompletedDL "__OTR__objects/object_triforce_completed/gTriforcePieceCompletedDL"
static const ALIGN_ASSET(2) char gTriforcePieceCompletedDL[] = dgTriforcePieceCompletedDL;

#define dgTrapDL "__OTR__objects/object_trap/gTrapDL"
static const ALIGN_ASSET(2) char gTrapDL[] = dgTrapDL;

#define dgSkeletonKeyDL dgMissingAsset
static const ALIGN_ASSET(2) char gSkeletonKeyDL[] = dgSkeletonKeyDL;

#define dgOcarinaAButtonDL "__OTR__objects/object_ocarina_a_button/gOcarinaAButtonDL"
static const ALIGN_ASSET(2) char gOcarinaAButtonDL[] = dgOcarinaAButtonDL;

#define dgOcarinaCDownButtonDL "__OTR__objects/object_ocarina_c_down_button/gOcarinaCDownButtonDL"
static const ALIGN_ASSET(2) char gOcarinaCDownButtonDL[] = dgOcarinaCDownButtonDL;

#define dgOcarinaCLeftButtonDL "__OTR__objects/object_ocarina_c_left_button/gOcarinaCLeftButtonDL"
static const ALIGN_ASSET(2) char gOcarinaCLeftButtonDL[] = dgOcarinaCLeftButtonDL;

#define dgOcarinaCRightButtonDL "__OTR__objects/object_ocarina_c_right_button/gOcarinaCRightButtonDL"
static const ALIGN_ASSET(2) char gOcarinaCRightButtonDL[] = dgOcarinaCRightButtonDL;

#define dgOcarinaCUpButtonDL "__OTR__objects/object_ocarina_c_up_button/gOcarinaCUpButtonDL"
static const ALIGN_ASSET(2) char gOcarinaCUpButtonDL[] = dgOcarinaCUpButtonDL;

#define dgGiDungeonSmallKeyDL "__OTR__objects/object_gi_key/gGiDungeonSmallKeyDL"
static const ALIGN_ASSET(2) char gGiDungeonSmallKeyDL[] = dgGiDungeonSmallKeyDL;

#define dgGiDungeonBossKeyDL "__OTR__objects/object_gi_key/gGiDungeonBossKeyDL"
static const ALIGN_ASSET(2) char gGiDungeonBossKeyDL[] = dgGiDungeonBossKeyDL;

#define dgGiWoodfallKeyEmblemDL "__OTR__objects/object_gi_key/gGiWoodfallKeyEmblemDL"
static const ALIGN_ASSET(2) char gGiWoodfallKeyEmblemDL[] = dgGiWoodfallKeyEmblemDL;

#define dgGiSnowheadKeyEmblemDL "__OTR__objects/object_gi_key/gGiSnowheadKeyEmblemDL"
static const ALIGN_ASSET(2) char gGiSnowheadKeyEmblemDL[] = dgGiSnowheadKeyEmblemDL;

#define dgGiGreatBayKeyEmblemDL "__OTR__objects/object_gi_key/gGiGreatBayKeyEmblemDL"
static const ALIGN_ASSET(2) char gGiGreatBayKeyEmblemDL[] = dgGiGreatBayKeyEmblemDL;

#define dgGiStoneTowerKeyEmblemDL "__OTR__objects/object_gi_key/gGiStoneTowerKeyEmblemDL"
static const ALIGN_ASSET(2) char gGiStoneTowerKeyEmblemDL[] = dgGiStoneTowerKeyEmblemDL;