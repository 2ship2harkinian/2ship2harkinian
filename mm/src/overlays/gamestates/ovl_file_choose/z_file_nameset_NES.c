/*
 * File: z_file_nameset_NES.c
 * Overlay: ovl_file_choose
 * Description: Entering name on a new file, selecting options from the options menu
 */

#include "prevent_bss_reordering.h"
#include "z_file_select.h"
#include "z64rumble.h"
#include "misc/title_static/title_static.h"
#include "overlays/ovl_file_choose/ovl_file_choose.h"
#include "BenPort.h"

#define DECLARE_PAL_TITLE_TEXTURE(name) static const ALIGN_ASSET(2) char name[] = "__OTR__misc/title_static/" #name

DECLARE_PAL_TITLE_TEXTURE(gFileSelOptionsGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelOptionsFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelOptionsESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelSoundGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelSoundFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelSoundESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelTargetingGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelTargetingFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelTargetingESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelCheckBrightnessGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelCheckBrightnessFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelCheckBrightnessESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelStereoGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelStereoFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelStereoESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelMonoGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelMonoFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelMonoESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelHeadsetGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelHeadsetFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelHeadsetESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelSwitchGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelSwitchFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelSwitchESPTex);

DECLARE_PAL_TITLE_TEXTURE(gFileSelHoldGERTex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelHoldFRATex);
DECLARE_PAL_TITLE_TEXTURE(gFileSelHoldESPTex);

#undef DECLARE_PAL_TITLE_TEXTURE

// PAL options menu vertices
static const ALIGN_ASSET(2) char sOptionsMenuHeadersPALVtx[] =
    "__OTR__overlays/ovl_file_choose/gOptionsMenuHeadersPALVtx";

static const ALIGN_ASSET(2) char sOptionsMenuSettingsPALVtx[] =
    "__OTR__overlays/ovl_file_choose/gOptionsMenuSettingsPALVtx";

static const ALIGN_ASSET(2) char sOptionsBrightnessPALVtx[] =
    "__OTR__overlays/ovl_file_choose/gOptionsBrightnessPALVtx";

static const ALIGN_ASSET(2) char sOptionsDividersPALVtx[] = "__OTR__overlays/ovl_file_choose/gOptionsDividersPALVtx";

static const ALIGN_ASSET(2) char sOptionsLanguageSettingsPALVtx[] =
    "__OTR__overlays/ovl_file_choose/gOptionsLanguageSettingsPALVtx";

// PAL language textures
static const ALIGN_ASSET(2) char sFileSelLanguageSmallFRATex[] = "__OTR__misc/title_static/gFileSelLanguageSmallFRATex";

static const ALIGN_ASSET(2) char sFileSelLanguageSmallGERTex[] = "__OTR__misc/title_static/gFileSelLanguageSmallGERTex";

static const ALIGN_ASSET(2) char sFileSelLanguageSmallENGTex[] = "__OTR__misc/title_static/gFileSelLanguageSmallENGTex";

static const ALIGN_ASSET(2) char sFileSelLanguageSmallESPTex[] = "__OTR__misc/title_static/gFileSelLanguageSmallESPTex";

static const ALIGN_ASSET(2) char sFileSelLanguageFRATex[] = "__OTR__misc/title_static/gFileSelLanguageFRATex";

static const ALIGN_ASSET(2) char sFileSelLanguageGERTex[] = "__OTR__misc/title_static/gFileSelLanguageGERTex";

static const ALIGN_ASSET(2) char sFileSelLanguageENGTex[] = "__OTR__misc/title_static/gFileSelLanguageENGTex";

static const ALIGN_ASSET(2) char sFileSelLanguageESPTex[] = "__OTR__misc/title_static/gFileSelLanguageESPTex";

static const TexturePtr sPalLanguageChoiceTextures[] = {
    sFileSelLanguageSmallENGTex,
    sFileSelLanguageSmallGERTex,
    sFileSelLanguageSmallFRATex,
    sFileSelLanguageSmallESPTex,
};

typedef struct {
    TexturePtr options;
    TexturePtr sound;
    TexturePtr targeting;
    TexturePtr language;
    TexturePtr checkBrightness;

    TexturePtr stereo;
    TexturePtr mono;
    TexturePtr headset;
    TexturePtr surround;
    TexturePtr switchTarget;
    TexturePtr holdTarget;
} PalOptionsTextures;

static const PalOptionsTextures sPalOptionsTextures[4] = {
    // English
    {
        gFileSelOptionsENGTex,
        gFileSelSoundENGTex,
        gFileSelTargetingENGTex,
        sFileSelLanguageENGTex,
        gFileSelCheckBrightnessENGTex,

        gFileSelStereoENGTex,
        gFileSelMonoENGTex,
        gFileSelHeadsetENGTex,
        gFileSelSurroundENGTex,
        gFileSelSwitchENGTex,
        gFileSelHoldENGTex,
    },

    // Deutsch
    {
        gFileSelOptionsGERTex,
        gFileSelSoundGERTex,
        gFileSelTargetingGERTex,
        sFileSelLanguageGERTex,
        gFileSelCheckBrightnessGERTex,

        gFileSelStereoGERTex,
        gFileSelMonoGERTex,
        gFileSelHeadsetGERTex,
        gFileSelSurroundENGTex,
        gFileSelSwitchGERTex,
        gFileSelHoldGERTex,
    },

    // Français
    {
        gFileSelOptionsFRATex,
        gFileSelSoundFRATex,
        gFileSelTargetingFRATex,
        sFileSelLanguageFRATex,
        gFileSelCheckBrightnessFRATex,

        gFileSelStereoFRATex,
        gFileSelMonoFRATex,
        gFileSelHeadsetFRATex,
        gFileSelSurroundENGTex,
        gFileSelSwitchFRATex,
        gFileSelHoldFRATex,
    },

    // Español
    {
        gFileSelOptionsESPTex,
        gFileSelSoundESPTex,
        gFileSelTargetingESPTex,
        sFileSelLanguageESPTex,
        gFileSelCheckBrightnessESPTex,

        gFileSelStereoESPTex,
        gFileSelMonoESPTex,
        gFileSelHeadsetESPTex,
        gFileSelSurroundENGTex,
        gFileSelSwitchESPTex,
        gFileSelHoldESPTex,
    },
};

static u8 FileSelect_GetPalLanguageIndex(void) {
    if ((gSaveContext.options.language < LANGUAGE_ENG) || (gSaveContext.options.language > LANGUAGE_SPA)) {
        return 0;
    }

    return gSaveContext.options.language - LANGUAGE_ENG;
}

static void FileSelect_ChangeLanguage(s32 direction) {
    if (direction < 0) {
        if (gSaveContext.options.language <= LANGUAGE_ENG) {
            gSaveContext.options.language = LANGUAGE_SPA;
        } else {
            gSaveContext.options.language--;
        }
    } else {
        if (gSaveContext.options.language >= LANGUAGE_SPA) {
            gSaveContext.options.language = LANGUAGE_ENG;
        } else {
            gSaveContext.options.language++;
        }
    }

    gSaveContext.options.languageSetting = gSaveContext.options.language - LANGUAGE_ENG;
}

#include "2s2h/BenGui/CosmeticEditor.h"

void FileSelect_DrawTexQuadI4(GraphicsContext* gfxCtx, TexturePtr texture, s16 point) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, point, point + 2, point + 3, point + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void FileSelect_DrawMultiTexQuadI4(GraphicsContext* gfxCtx, TexturePtr texture1, TexturePtr texture2, s16 point) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gDPLoadMultiBlock_4b(POLY_OPA_DISP++, texture2, 0x0080, 1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, point, point + 2, point + 3, point + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

s16 D_80814280[] = {
    2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 0, 1, 1, 2, 1, 1, 4, 2, 2, 2, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 2, 2, 2, 2, 2, 3, 2, 2, 4, 3, 2, 4, 1, 2, 2, 1, 1, 2, 2, 3, 2, 2, 0, 2, 2, 2, 0, 3, 1, 0,
};

// 2S2H [Port] - Add halfword from following array to avoid OOB access
s16 D_80814304[] = {
    1, 2, 0, 1, 1, 2, 1, 1, 4, 2, 2, 2, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 2,
    2, 4, 3, 2, 4, 1, 2, 2, 1, 1, 2, 2, 3, 2, 2, 0, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 0, 0
};

// 2S2H [Port] - Add halfword from following array to avoid OOB access
s16 D_80814384[] = {
    0, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, -94
};

s16 D_80814404[] = {
    -94, -96, -48, 0, 32, 64,
};

s16 D_80814410[] = {
    56, 44, 44, 28, 28, 44,
};

s16 D_8081441C[] = {
    72, -48, -48, -48, -48, -48,
};

void FileSelect_SetKeyboardVtx(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t3;
    s16 phi_s1;
    s16 phi_t1;
    s16 phi_s2;

    this->keyboardVtx = GRAPH_ALLOC(this->state.gfxCtx, sizeof(Vtx) * 4 * 5 * 13);

    phi_s1 = 0x26;

    for (phi_t2 = 0, phi_s2 = 0, phi_t3 = 0; phi_s2 < 5; phi_s2++) {
        phi_t0 = -0x60;

        for (phi_t1 = 0; phi_t1 < 13; phi_t1++, phi_t3 += 4, phi_t2++) {
            //! @bug D_80814304 is accessed out of bounds when drawing the empty space character (value of 64). Under
            //! normal circumstances it reads a halfword from D_80814384.
            // 2S2H [Port] - increase D_80814304 size
            this->keyboardVtx[phi_t3].v.ob[0] = this->keyboardVtx[phi_t3 + 2].v.ob[0] = D_80814304[phi_t2] + phi_t0;

            this->keyboardVtx[phi_t3 + 1].v.ob[0] = this->keyboardVtx[phi_t3 + 3].v.ob[0] =
                D_80814304[phi_t2] + phi_t0 + 12;

            this->keyboardVtx[phi_t3].v.ob[1] = this->keyboardVtx[phi_t3 + 1].v.ob[1] = phi_s1;

            this->keyboardVtx[phi_t3 + 2].v.ob[1] = this->keyboardVtx[phi_t3 + 3].v.ob[1] = phi_s1 - 12;

            this->keyboardVtx[phi_t3].v.ob[2] = this->keyboardVtx[phi_t3 + 1].v.ob[2] =
                this->keyboardVtx[phi_t3 + 2].v.ob[2] = this->keyboardVtx[phi_t3 + 3].v.ob[2] = 0;

            this->keyboardVtx[phi_t3].v.flag = this->keyboardVtx[phi_t3 + 1].v.flag =
                this->keyboardVtx[phi_t3 + 2].v.flag = this->keyboardVtx[phi_t3 + 3].v.flag = 0;

            this->keyboardVtx[phi_t3].v.tc[0] = this->keyboardVtx[phi_t3].v.tc[1] =
                this->keyboardVtx[phi_t3 + 1].v.tc[1] = this->keyboardVtx[phi_t3 + 2].v.tc[0] = 0;

            this->keyboardVtx[phi_t3 + 1].v.tc[0] = this->keyboardVtx[phi_t3 + 2].v.tc[1] =
                this->keyboardVtx[phi_t3 + 3].v.tc[0] = this->keyboardVtx[phi_t3 + 3].v.tc[1] = 16 << 5;

            this->keyboardVtx[phi_t3].v.cn[0] = this->keyboardVtx[phi_t3 + 1].v.cn[0] =
                this->keyboardVtx[phi_t3 + 2].v.cn[0] = this->keyboardVtx[phi_t3 + 3].v.cn[0] =
                    this->keyboardVtx[phi_t3].v.cn[1] = this->keyboardVtx[phi_t3 + 1].v.cn[1] =
                        this->keyboardVtx[phi_t3 + 2].v.cn[1] = this->keyboardVtx[phi_t3 + 3].v.cn[1] =
                            this->keyboardVtx[phi_t3].v.cn[2] = this->keyboardVtx[phi_t3 + 1].v.cn[2] =
                                this->keyboardVtx[phi_t3 + 2].v.cn[2] = this->keyboardVtx[phi_t3 + 3].v.cn[2] =
                                    this->keyboardVtx[phi_t3].v.cn[3] = this->keyboardVtx[phi_t3 + 1].v.cn[3] =
                                        this->keyboardVtx[phi_t3 + 2].v.cn[3] = this->keyboardVtx[phi_t3 + 3].v.cn[3] =
                                            255;

            phi_t0 += 0x10;
        }

        phi_s1 -= 0x10;
    }

    this->keyboard2Vtx = GRAPH_ALLOC(this->state.gfxCtx, sizeof(Vtx) * 24);

    for (phi_t1 = 0, phi_t3 = 0; phi_t3 < 6; phi_t3++, phi_t1 += 4) {

        this->keyboard2Vtx[phi_t1 + 0].v.ob[0] = this->keyboard2Vtx[phi_t1 + 2].v.ob[0] = D_80814404[phi_t3] + 1;

        this->keyboard2Vtx[phi_t1 + 1].v.ob[0] = this->keyboard2Vtx[phi_t1 + 3].v.ob[0] =
            this->keyboard2Vtx[phi_t1 + 0].v.ob[0] + D_80814410[phi_t3] - 2;

        this->keyboard2Vtx[phi_t1 + 0].v.ob[1] = this->keyboard2Vtx[phi_t1 + 1].v.ob[1] = D_8081441C[phi_t3] - 1;

        this->keyboard2Vtx[phi_t1 + 2].v.ob[1] = this->keyboard2Vtx[phi_t1 + 3].v.ob[1] =
            this->keyboard2Vtx[phi_t1 + 0].v.ob[1] - 0xE;

        this->keyboard2Vtx[phi_t1 + 0].v.ob[2] = this->keyboard2Vtx[phi_t1 + 1].v.ob[2] =
            this->keyboard2Vtx[phi_t1 + 2].v.ob[2] = this->keyboard2Vtx[phi_t1 + 3].v.ob[2] = 0;

        this->keyboard2Vtx[phi_t1 + 0].v.flag = this->keyboard2Vtx[phi_t1 + 1].v.flag =
            this->keyboard2Vtx[phi_t1 + 2].v.flag = this->keyboard2Vtx[phi_t1 + 3].v.flag = 0;

        this->keyboard2Vtx[phi_t1 + 0].v.tc[0] = this->keyboard2Vtx[phi_t1 + 0].v.tc[1] =
            this->keyboard2Vtx[phi_t1 + 1].v.tc[1] = this->keyboard2Vtx[phi_t1 + 2].v.tc[0] = 0;

        this->keyboard2Vtx[phi_t1 + 1].v.tc[0] = this->keyboard2Vtx[phi_t1 + 3].v.tc[0] = D_80814410[phi_t3] << 5;

        this->keyboard2Vtx[phi_t1 + 2].v.tc[1] = this->keyboard2Vtx[phi_t1 + 3].v.tc[1] = 16 << 5;

        this->keyboard2Vtx[phi_t1 + 0].v.cn[0] = this->keyboard2Vtx[phi_t1 + 1].v.cn[0] =
            this->keyboard2Vtx[phi_t1 + 2].v.cn[0] = this->keyboard2Vtx[phi_t1 + 3].v.cn[0] =
                this->keyboard2Vtx[phi_t1 + 0].v.cn[1] = this->keyboard2Vtx[phi_t1 + 1].v.cn[1] =
                    this->keyboard2Vtx[phi_t1 + 2].v.cn[1] = this->keyboard2Vtx[phi_t1 + 3].v.cn[1] =
                        this->keyboard2Vtx[phi_t1 + 0].v.cn[2] = this->keyboard2Vtx[phi_t1 + 1].v.cn[2] =
                            this->keyboard2Vtx[phi_t1 + 2].v.cn[2] = this->keyboard2Vtx[phi_t1 + 3].v.cn[2] =
                                this->keyboard2Vtx[phi_t1 + 0].v.cn[3] = this->keyboard2Vtx[phi_t1 + 1].v.cn[3] =
                                    this->keyboard2Vtx[phi_t1 + 2].v.cn[3] = this->keyboard2Vtx[phi_t1 + 3].v.cn[3] =
                                        255;
    }
}

TexturePtr sBackspaceEndTextures[] = {
    gFileSelBackspaceButtonTex,
    gFileSelENDButtonENGTex,
};

u16 sBackspaceEndWidths[] = { 28, 44 };

s16 D_80814434[] = {
    -30, -16, -6, 4, 14, 24, 34, 44, 54, -16, -16, 0,
};

s16 D_8081444C[] = {
    72, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 0,
};

/**
 * Set vertices used by all elements of the name entry screen that are NOT the keyboard.
 * This includes the cursor highlight, the name entry plate and characters, and the buttons.
 */
void FileSelect_SetNameEntryVtx(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    u8 temp;
    s16 var_s0;
    s16 var_t1;
    s16 sp108;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, this->keyboard2Vtx, 24, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameENGTex, G_IM_FMT_IA, G_IM_SIZ_8b, 56, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);

    for (var_t1 = 0, var_s0 = 0x10; var_t1 < 2; var_t1++, var_s0 += 4) {
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                255, COSMETIC_ID("Menus.FilePlates"));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sBackspaceEndTextures[var_t1], G_IM_FMT_IA, G_IM_SIZ_16b,
                            sBackspaceEndWidths[var_t1], 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, var_s0, var_s0 + 2, var_s0 + 3, var_s0 + 1, 0);
    }

    this->nameEntryVtx = GRAPH_ALLOC(this->state.gfxCtx, 44 * sizeof(Vtx));

    for (var_s0 = 0, var_t1 = 0; var_t1 < 44; var_t1 += 4, var_s0++) {
        if ((var_s0 > 0) && (var_s0 < 9)) {
            temp = this->fileNames[this->buttonIndex][var_s0 - 1];

            this->nameEntryVtx[var_t1 + 0].v.ob[0] = this->nameEntryVtx[var_t1 + 2].v.ob[0] =
                D_80814434[var_s0] + this->nameEntryBoxPosX + D_80814280[temp];

            this->nameEntryVtx[var_t1 + 1].v.ob[0] = this->nameEntryVtx[var_t1 + 3].v.ob[0] =
                this->nameEntryVtx[var_t1 + 0].v.ob[0] + 10;
        } else {
            this->nameEntryVtx[var_t1 + 0].v.ob[0] = this->nameEntryVtx[var_t1 + 2].v.ob[0] =
                D_80814434[var_s0] + this->nameEntryBoxPosX;

            this->nameEntryVtx[var_t1 + 1].v.ob[0] = this->nameEntryVtx[var_t1 + 3].v.ob[0] =
                this->nameEntryVtx[var_t1 + 0].v.ob[0] + 10;
        }

        this->nameEntryVtx[var_t1 + 0].v.ob[1] = this->nameEntryVtx[var_t1 + 1].v.ob[1] = D_8081444C[var_s0];

        this->nameEntryVtx[var_t1 + 2].v.ob[1] = this->nameEntryVtx[var_t1 + 3].v.ob[1] =
            this->nameEntryVtx[var_t1 + 0].v.ob[1] - 10;

        this->nameEntryVtx[var_t1 + 0].v.ob[2] = this->nameEntryVtx[var_t1 + 1].v.ob[2] =
            this->nameEntryVtx[var_t1 + 2].v.ob[2] = this->nameEntryVtx[var_t1 + 3].v.ob[2] = 0;

        this->nameEntryVtx[var_t1 + 0].v.flag = this->nameEntryVtx[var_t1 + 1].v.flag =
            this->nameEntryVtx[var_t1 + 2].v.flag = this->nameEntryVtx[var_t1 + 3].v.flag = 0;

        this->nameEntryVtx[var_t1 + 0].v.tc[0] = this->nameEntryVtx[var_t1 + 0].v.tc[1] =
            this->nameEntryVtx[var_t1 + 1].v.tc[1] = this->nameEntryVtx[var_t1 + 2].v.tc[0] = 0;

        this->nameEntryVtx[var_t1 + 1].v.tc[0] = this->nameEntryVtx[var_t1 + 2].v.tc[1] =
            this->nameEntryVtx[var_t1 + 3].v.tc[0] = this->nameEntryVtx[var_t1 + 3].v.tc[1] = 16 << 5;

        this->nameEntryVtx[var_t1 + 0].v.cn[0] = this->nameEntryVtx[var_t1 + 1].v.cn[0] =
            this->nameEntryVtx[var_t1 + 2].v.cn[0] = this->nameEntryVtx[var_t1 + 3].v.cn[0] =
                this->nameEntryVtx[var_t1 + 0].v.cn[1] = this->nameEntryVtx[var_t1 + 1].v.cn[1] =
                    this->nameEntryVtx[var_t1 + 2].v.cn[1] = this->nameEntryVtx[var_t1 + 3].v.cn[1] =
                        this->nameEntryVtx[var_t1 + 0].v.cn[2] = this->nameEntryVtx[var_t1 + 1].v.cn[2] =
                            this->nameEntryVtx[var_t1 + 2].v.cn[2] = this->nameEntryVtx[var_t1 + 3].v.cn[2] =
                                this->nameEntryVtx[var_t1 + 0].v.cn[3] = this->nameEntryVtx[var_t1 + 1].v.cn[3] =
                                    this->nameEntryVtx[var_t1 + 2].v.cn[3] = this->nameEntryVtx[var_t1 + 3].v.cn[3] =
                                        255;
    }

    this->nameEntryVtx[1].v.ob[0] = this->nameEntryVtx[3].v.ob[0] = this->nameEntryVtx[0].v.ob[0] + 0x6C;
    this->nameEntryVtx[2].v.ob[1] = this->nameEntryVtx[3].v.ob[1] = this->nameEntryVtx[0].v.ob[1] - 0x10;
    this->nameEntryVtx[1].v.tc[0] = this->nameEntryVtx[3].v.tc[0] = 108 << 5;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->nameEntryBoxAlpha, COSMETIC_ID("Menus.FilePlates"));
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx, 4, 0);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelFileNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx + 4, 32, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->nameEntryBoxAlpha);

    for (sp108 = 0, var_s0 = 0; var_s0 < 0x20; var_s0 += 4, sp108++) {
        FileSelect_DrawTexQuadI4(
            this->state.gfxCtx, font->fontBuf + this->fileNames[this->buttonIndex][sp108] * FONT_CHAR_TEX_SIZE, var_s0);
    }

    this->nameEntryVtx[37].v.tc[0] = this->nameEntryVtx[38].v.tc[1] = this->nameEntryVtx[39].v.tc[0] =
        this->nameEntryVtx[39].v.tc[1] = this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[42].v.tc[1] =
            this->nameEntryVtx[43].v.tc[0] = this->nameEntryVtx[43].v.tc[1] = 24 << 5;

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[43].v.tc[0] = 56 << 5;
    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[43].v.tc[0] = 40 << 5;
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_DrawKeyboard(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    s16 i;
    s16 tmp;
    s16 vtx;

    OPEN_DISPS(this->state.gfxCtx);

    Gfx_SetupDL42_Opa(this->state.gfxCtx);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, COMBINED, 0,
                      0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, this->charBgAlpha, 255, 255, 255, 255);

    for (i = 0, vtx = 0; vtx < 0x100; vtx += 32) {
        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

        for (tmp = 0; tmp < 32; i++, tmp += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + D_808141F0[i] * FONT_CHAR_TEX_SIZE, tmp);
        }
    }

    gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
    FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + D_808141F0[i] * FONT_CHAR_TEX_SIZE, 0);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_DrawNameEntry(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Font* font = &this->font;
    Input* input = CONTROLLER1(&this->state);
    s16 i;
    s16 tmp;
    u16 time;
    s16 validName;

    OPEN_DISPS(this->state.gfxCtx);

    FileSelect_SetKeyboardVtx(&this->state);
    FileSelect_SetNameEntryVtx(&this->state);
    FileSelect_PulsateCursor(&this->state);

    tmp = (this->newFileNameCharCount * 4) + 4;
    this->nameEntryVtx[36].v.ob[0] = this->nameEntryVtx[38].v.ob[0] = this->nameEntryVtx[tmp].v.ob[0] - 6;
    this->nameEntryVtx[37].v.ob[0] = this->nameEntryVtx[39].v.ob[0] = this->nameEntryVtx[36].v.ob[0] + 24;
    this->nameEntryVtx[36].v.ob[1] = this->nameEntryVtx[37].v.ob[1] = this->nameEntryVtx[tmp].v.ob[1] + 7;
    this->nameEntryVtx[38].v.ob[1] = this->nameEntryVtx[39].v.ob[1] = this->nameEntryVtx[36].v.ob[1] - 24;

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboard2Vtx[(this->kbdX + 1) * 4].v.ob[0] - 4;
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 52;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            this->keyboard2Vtx[(this->kbdX + 1) * 4].v.ob[1] + 4;
    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboard2Vtx[(this->kbdX + 1) * 4].v.ob[0] - 4;
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 40;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            this->keyboard2Vtx[(this->kbdX + 1) * 4].v.ob[1] + 4;
    } else {
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboardVtx[this->charIndex * 4].v.ob[0] - D_80814304[this->charIndex] - 6;
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 24;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            this->keyboardVtx[this->charIndex * 4].v.ob[1] + 6;
    }

    this->nameEntryVtx[42].v.ob[1] = this->nameEntryVtx[43].v.ob[1] = this->nameEntryVtx[40].v.ob[1] - 24;

    gSPVertex(POLY_OPA_DISP++, &this->nameEntryVtx[36], 8, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1], this->highlightColor[2],
                    this->highlightColor[3]);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelCharHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 24, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelMediumButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 56, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelSmallButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 40, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    }

    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

    FileSelect_DrawKeyboard(&this->state);
    gDPPipeSync(POLY_OPA_DISP++);
    Gfx_SetupDL42_Opa(this->state.gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    if (this->configMode == CM_NAME_ENTRY) {
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            // place cursor on END button
            this->kbdY = 5;
            this->kbdX = 4;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if ((this->newFileNameCharCount == 7) && (this->fileNames[this->buttonIndex][7] != 0x3E)) {

                for (i = this->newFileNameCharCount; i < 7; i++) {
                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                }

                this->fileNames[this->buttonIndex][i] = 0x3E;
                Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
            } else {
                this->newFileNameCharCount--;

                if (this->newFileNameCharCount < 0) {
                    this->newFileNameCharCount = 0;
                    this->configMode = CM_NAME_ENTRY_TO_MAIN;
                    Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
                } else {
                    for (i = this->newFileNameCharCount; i < 7; i++) {
                        this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                    }

                    this->fileNames[this->buttonIndex][i] = 0x3E;
                    Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
                }
            }
        } else {
            if (this->charPage <= FS_CHAR_PAGE_ENG) {
                if (this->kbdY != 5) {
                    // draw the character the cursor is hovering over in yellow
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);

                    //! @bug D_80814384 is accessed out of bounds when drawing the empty space character (value of 64).
                    //! Under normal circumstances it reads a halfword from D_80814404.
                    // 2S2H - increase D_80814384 size
                    this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[0] =
                        this->keyboardVtx[(this->charIndex * 4) + 2].v.ob[0] =
                            this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[0] + D_80814384[this->charIndex] - 2;

                    this->keyboardVtx[(this->charIndex * 4) + 1].v.ob[0] =
                        this->keyboardVtx[(this->charIndex * 4) + 3].v.ob[0] =
                            this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[0] + 0x10;

                    this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[1] =
                        this->keyboardVtx[(this->charIndex * 4) + 1].v.ob[1] =
                            this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[1] + 2;

                    this->keyboardVtx[(this->charIndex * 4) + 2].v.ob[1] =
                        this->keyboardVtx[(this->charIndex * 4) + 3].v.ob[1] =
                            this->keyboardVtx[(this->charIndex * 4) + 0].v.ob[1] - 0x10;

                    gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[this->charIndex * 4], 4, 0);

                    FileSelect_DrawTexQuadI4(this->state.gfxCtx,
                                             font->fontBuf + D_808141F0[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);

                    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                        Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_S);
                        this->fileNames[this->buttonIndex][this->newFileNameCharCount] = D_808141F0[this->charIndex];

                        this->newFileNameCharCount++;

                        if (this->newFileNameCharCount > 7) {
                            this->newFileNameCharCount = 7;
                        }
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    if (this->charPage != this->kbdButton) {
                        if (this->kbdButton == FS_KBD_BTN_BACKSPACE) {
                            if ((this->newFileNameCharCount == 7) && (this->fileNames[this->buttonIndex][7] != 0x3E)) {

                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                                }

                                this->fileNames[this->buttonIndex][i] = 0x3E;
                                Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
                            } else {
                                this->newFileNameCharCount--;

                                if (this->newFileNameCharCount < 0) {
                                    this->newFileNameCharCount = 0;
                                }

                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                                }

                                this->fileNames[this->buttonIndex][i] = 0x3E;
                                Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
                            }
                        } else if (this->kbdButton == FS_KBD_BTN_END) {
                            validName = false;

                            for (i = 0; i < 8; i++) {
                                if (this->fileNames[this->buttonIndex][i] != 0x3E) {
                                    validName = true;
                                    break;
                                }
                            }

                            if (validName) {
                                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
                                gSaveContext.fileNum = this->buttonIndex;
                                time = CURRENT_TIME;
                                Sram_InitSave(this, sramCtx);
                                gSaveContext.save.time = time;

                                if (!gSaveContext.flashSaveAvailable) {
                                    this->configMode = CM_NAME_ENTRY_TO_MAIN;
                                } else {
                                    Sram_SetFlashPagesDefault(
                                        sramCtx, gFlashSaveStartPages[this->buttonIndex * FLASH_SAVE_MAIN_MULTIPLIER],
                                        gFlashSpecialSaveNumPages[this->buttonIndex * FLASH_SAVE_MAIN_MULTIPLIER]);
                                    Sram_StartWriteToFlashDefault(sramCtx);
                                    this->configMode = CM_NAME_ENTRY_WAIT_FOR_FLASH_SAVE;
                                }

                                this->nameBoxAlpha[this->buttonIndex] = this->nameAlpha[this->buttonIndex] = 200;
                                this->connectorAlpha[this->buttonIndex] = 255;
                                Rumble_Request(300.0f, 180, 20, 100);
                            } else {
                                Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
                            }
                        }
                    }
                }

                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
                    this->newFileNameCharCount++;

                    if (this->newFileNameCharCount > 7) {
                        this->newFileNameCharCount = 7;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
                    this->newFileNameCharCount--;

                    if (this->newFileNameCharCount < 0) {
                        this->newFileNameCharCount = 0;
                    }
                }
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx);
}

/**
 * Fade in the name entry box and slide it to the center of the screen from the right side.
 * After the name entry box is in place, init the keyboard/cursor and change modes.
 * Update function for `CM_START_NAME_ENTRY`
 */
void FileSelect_StartNameEntry(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->kbdX = 0;
        this->kbdY = 0;
        this->kbdButton = FS_KBD_BTN_NONE;
        this->configMode = CM_NAME_ENTRY;
    }
}

/**
 * Update the keyboard cursor and play sound effects at the appropriate times.
 * There are many special cases for warping the cursor depending on where
 * the cursor currently is.
 * Update function for `CM_NAME_ENTRY`
 */
void FileSelect_UpdateKeyboardCursor(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 prevKbdX;

    this->kbdButton = FS_KBD_BTN_NONE;

    if (this->kbdY != 5) {
        if (this->stickAdjX < -30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            this->charIndex--;
            this->kbdX--;
            if (this->kbdX < 0) {
                this->kbdX = 12;
                this->charIndex = (this->kbdY * 13) + this->kbdX;
            }
        } else if (this->stickAdjX > 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            this->charIndex++;
            this->kbdX++;
            if (this->kbdX > 12) {
                this->kbdX = 0;
                this->charIndex = (this->kbdY * 13) + this->kbdX;
            }
        }
    } else {
        if (this->stickAdjX < -30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            this->kbdX--;
            if (this->kbdX < 3) {
                this->kbdX = 4;
            }
        } else if (this->stickAdjX > 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            this->kbdX++;
            if (this->kbdX > 4) {
                this->kbdX = 3;
            }
        }
    }

    if (this->stickAdjY > 30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

        this->kbdY--;

        if (this->kbdY < 0) {
            // Don't go to bottom row
            if (this->kbdX < 8) {
                this->kbdY = 4;
                this->charIndex = (s32)(this->kbdX + 52);
            } else {
                this->kbdY = 5;
                this->charIndex += 52;
                prevKbdX = this->kbdX;

                if (this->kbdX < 10) {
                    this->kbdX = 3;
                } else if (this->kbdX < 13) {
                    this->kbdX = 4;
                }
                this->unk_2451E[this->kbdX] = prevKbdX;
            }
        } else {
            this->charIndex -= 13;

            if (this->kbdY == 4) {
                this->charIndex = 52;
                this->kbdX = this->unk_2451E[this->kbdX];
                this->charIndex += this->kbdX;
            }
        }
    } else if (this->stickAdjY < -30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        this->kbdY++;

        if (this->kbdY > 5) {
            this->kbdY = 0;
            this->kbdX = this->unk_2451E[this->kbdX];
            this->charIndex = this->kbdX;
        } else {
            this->charIndex += 13;

            if (this->kbdY == 5) {
                if (this->kbdX < 8) {
                    this->kbdY = 0;
                    this->charIndex = this->kbdX;
                } else {
                    prevKbdX = this->kbdX;

                    if (this->kbdX < 3) {
                        this->kbdX = 0;
                    } else if (this->kbdX < 6) {
                        this->kbdX = 1;
                    } else if (this->kbdX < 8) {
                        this->kbdX = 2;
                    } else if (this->kbdX < 10) {
                        this->kbdX = 3;
                    } else if (this->kbdX < 13) {
                        this->kbdX = 4;
                    }
                    this->unk_2451E[this->kbdX] = prevKbdX;
                }
            }
        }
    }
    if (this->kbdY == 5) {
        this->kbdButton = this->kbdX;
    }
}

/**
 * Update and wait for the save to flash to complete.
 * Update function for `CM_NAME_ENTRY_WAIT_FOR_FLASH_SAVE`
 */
void FileSelect_NameEntryWaitForFlashSave(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;

    Sram_UpdateWriteToFlashDefault(sramCtx);

    if (sramCtx->status == 0) {
        this->configMode = CM_NAME_ENTRY_TO_MAIN;
    }
}

/**
 * This function is mostly a copy paste of `FileSelect_StartNameEntry`.
 * The name entry box fades and slides in even though it is not visible.
 * After this is complete, change to the options config mode.
 * Update function for `CM_START_OPTIONS`
 */
void FileSelect_StartOptions(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->configMode = CM_OPTIONS_MENU;
    }
}

u8 sSelectedSetting;
// bss padding
s32 D_80814E94;
s32 D_80814E98;
s32 D_80814E9C;
s32 D_80814EA0;

/**
 * Update the cursor and appropriate settings for the options menu.
 * If the player presses B, write the selected options to the SRAM header
 * and set config mode to rotate back to the main menu.
 * Update function for `CM_OPTIONS_MENU`
 */
void FileSelect_UpdateOptionsMenu(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        Sram_WriteSaveOptionsToBuffer(sramCtx);

        if (!gSaveContext.flashSaveAvailable) {
            this->configMode = CM_OPTIONS_TO_MAIN;
        } else {
            Sram_SetFlashPagesDefault(sramCtx, gFlashSaveStartPages[FLASH_SAVE_SRAM_HEADER],
                                      gFlashSpecialSaveNumPages[FLASH_SAVE_SRAM_HEADER]);
            Sram_StartWriteToFlashDefault(sramCtx);
            this->configMode = CM_OPTIONS_WAIT_FOR_FLASH_SAVE;
        }
        Audio_SetFileSelectSettings(gSaveContext.options.audioSetting);
        return;
    }

    if (this->stickAdjX < -30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

        switch (sSelectedSetting) {
            case FS_SETTING_AUDIO:
                gSaveContext.options.audioSetting--;
                // because audio setting is unsigned, can't check for < 0
                if (gSaveContext.options.audioSetting > 0xF0) {
                    gSaveContext.options.audioSetting = SAVE_AUDIO_SURROUND;
                }
                break;

            case FS_SETTING_ZTARGET:
                gSaveContext.options.zTargetSetting ^= 1;
                break;

            case FS_SETTING_LANGUAGE:
                if (ResourceMgr_HasGameVersion(MM_PAL_11)) {
                    FileSelect_ChangeLanguage(-1);
                }
                break;
        }
    } else if (this->stickAdjX > 30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

        switch (sSelectedSetting) {
            case FS_SETTING_AUDIO:
                gSaveContext.options.audioSetting++;
                if (gSaveContext.options.audioSetting > SAVE_AUDIO_SURROUND) {
                    gSaveContext.options.audioSetting = SAVE_AUDIO_STEREO;
                }
                break;

            case FS_SETTING_ZTARGET:
                gSaveContext.options.zTargetSetting ^= 1;
                break;

            case FS_SETTING_LANGUAGE:
                if (ResourceMgr_HasGameVersion(MM_PAL_11)) {
                    FileSelect_ChangeLanguage(1);
                }
                break;
        }
    }

    if ((this->stickAdjY < -30) || (this->stickAdjY > 30)) {
        u8 settingCount = ResourceMgr_HasGameVersion(MM_PAL_11) ? FS_SETTING_MAX : FS_SETTING_LANGUAGE;

        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

        if (this->stickAdjY < -30) {
            sSelectedSetting++;
            if (sSelectedSetting >= settingCount) {
                sSelectedSetting = FS_SETTING_AUDIO;
            }
        } else {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                sSelectedSetting = settingCount - 1;
            } else {
                sSelectedSetting--;
            }
        }
        return;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        u8 settingCount = ResourceMgr_HasGameVersion(MM_PAL_11) ? FS_SETTING_MAX : FS_SETTING_LANGUAGE;

        Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        sSelectedSetting++;
        if (sSelectedSetting >= settingCount) {
            sSelectedSetting = FS_SETTING_AUDIO;
        }
    }
}

/**
 * Update and wait for the save to flash to complete.
 * Update function for `CM_OPTIONS_WAIT_FOR_FLASH_SAVE`
 */
void FileSelect_OptionsWaitForFlashSave(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;

    Sram_UpdateWriteToFlashDefault(sramCtx);

    if (sramCtx->status == 0) {
        this->configMode = CM_OPTIONS_TO_MAIN;
    }
}

typedef struct {
    /* 0x0 */ TexturePtr texture;
    /* 0x4 */ u16 width;
    /* 0x6 */ u16 height;
} OptionsMenuTextureInfo; // size = 0x8

OptionsMenuTextureInfo gOptionsMenuHeaders[] = {
    { gFileSelOptionsENGTex, 128, 16 },          { gFileSelSoundENGTex, 64, 16 },
    { gFileSelTargetingENGTex, 64, 16 },         { gFileSelCheckBrightnessENGTex, 96, 16 },
    { gFileSelDolbySurroundLogoENGTex, 48, 17 },
};

OptionsMenuTextureInfo gOptionsMenuHeadersIA4[] = {
    { gFileSelOptionsIA4ENGTex, 128, 16 },       { gFileSelSoundIA4ENGTex, 64, 16 },
    { gFileSelTargetingIA4ENGTex, 64, 16 },      { gFileSelCheckBrightnessIA4ENGTex, 96, 16 },
    { gFileSelDolbySurroundLogoENGTex, 48, 17 },
};

OptionsMenuTextureInfo gOptionsMenuSettings[] = {
    { gFileSelStereoENGTex, 48, 16 },   { gFileSelMonoENGTex, 48, 16 },   { gFileSelHeadsetENGTex, 48, 16 },
    { gFileSelSurroundENGTex, 48, 16 }, { gFileSelSwitchENGTex, 48, 16 }, { gFileSelHoldENGTex, 48, 16 },
};

OptionsMenuTextureInfo gOptionsMenuSettingsIA4[] = {
    { gFileSelStereoIA4ENGTex, 48, 16 },   { gFileSelMonoIA4ENGTex, 48, 16 },   { gFileSelHeadsetIA4ENGTex, 48, 16 },
    { gFileSelSurroundIA4ENGTex, 48, 16 }, { gFileSelSwitchIA4ENGTex, 48, 16 }, { gFileSelHoldIA4ENGTex, 48, 16 },
};

void FileSelect_DrawOptionsImpl_NES_GC(GameState* thisx) {
    static s16 sCursorPrimRed = 255;
    static s16 sCursorPrimGreen = 255;
    static s16 sCursorPrimBlue = 255;
    static s16 sCursorEnvRed = 0;
    static s16 sCursorEnvGreen = 0;
    static s16 sCursorEnvBlue = 0;
    static s16 sCursorPulseDir = 1;
    static s16 sCursorFlashTimer = 20;
    static s16 sCursorPrimColors[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 sCursorEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    FileSelectState* this = (FileSelectState*)thisx;
    s16 cursorRedStep;
    s16 cursorGreenStep;
    s16 cursorBlueStep;
    s16 i;
    s16 j;
    s16 vtx;

    OPEN_DISPS(this->state.gfxCtx);

    cursorRedStep = ABS_ALT(sCursorPrimRed - sCursorPrimColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorPrimGreen - sCursorPrimColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorPrimBlue - sCursorPrimColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorPrimRed >= sCursorPrimColors[sCursorPulseDir][0]) {
        sCursorPrimRed -= cursorRedStep;
    } else {
        sCursorPrimRed += cursorRedStep;
    }

    if (sCursorPrimGreen >= sCursorPrimColors[sCursorPulseDir][1]) {
        sCursorPrimGreen -= cursorGreenStep;
    } else {
        sCursorPrimGreen += cursorGreenStep;
    }

    if (sCursorPrimBlue >= sCursorPrimColors[sCursorPulseDir][2]) {
        sCursorPrimBlue -= cursorBlueStep;
    } else {
        sCursorPrimBlue += cursorBlueStep;
    }

    cursorRedStep = ABS_ALT(sCursorEnvRed - sCursorEnvColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorEnvGreen - sCursorEnvColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorEnvBlue - sCursorEnvColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorEnvRed >= sCursorEnvColors[sCursorPulseDir][0]) {
        sCursorEnvRed -= cursorRedStep;
    } else {
        sCursorEnvRed += cursorRedStep;
    }

    if (sCursorEnvGreen >= sCursorEnvColors[sCursorPulseDir][1]) {
        sCursorEnvGreen -= cursorGreenStep;
    } else {
        sCursorEnvGreen += cursorGreenStep;
    }

    if (sCursorEnvBlue >= sCursorEnvColors[sCursorPulseDir][2]) {
        sCursorEnvBlue -= cursorBlueStep;
    } else {
        sCursorEnvBlue += cursorBlueStep;
    }

    if (--sCursorFlashTimer == 0) {
        sCursorPrimRed = sCursorPrimColors[sCursorPulseDir][0];
        sCursorPrimGreen = sCursorPrimColors[sCursorPulseDir][1];
        sCursorPrimBlue = sCursorPrimColors[sCursorPulseDir][2];

        sCursorEnvRed = sCursorEnvColors[sCursorPulseDir][0];
        sCursorEnvGreen = sCursorEnvColors[sCursorPulseDir][1];
        sCursorEnvBlue = sCursorEnvColors[sCursorPulseDir][2];

        sCursorFlashTimer = 20;

        if (++sCursorPulseDir > 1) {
            sCursorPulseDir = 0;
        }
    }

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    Matrix_Push();
    Matrix_Translate(0.0f, 0.1f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerVtx, 12, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.2f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.4f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
    Matrix_Pop();

    gSPVertex(POLY_OPA_DISP++, D_80813DF0, 20, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (i = 0, vtx = 0; i < 5; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == 4) {
            if (gSaveContext.options.audioSetting == SAVE_AUDIO_SURROUND) {
                gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }

            gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuHeadersIA4[i].texture, G_IM_FMT_IA, G_IM_SIZ_8b,
                                gOptionsMenuHeadersIA4[i].width, gOptionsMenuHeadersIA4[i].height, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gOptionsMenuHeadersIA4[i].texture, G_IM_FMT_IA,
                                   gOptionsMenuHeadersIA4[i].width, gOptionsMenuHeadersIA4[i].height, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);
        }
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPVertex(POLY_OPA_DISP++, D_80813F30, 24, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == gSaveContext.options.audioSetting) {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gOptionsMenuSettingsIA4[i].texture, G_IM_FMT_IA,
                               gOptionsMenuSettingsIA4[i].width, gOptionsMenuSettingsIA4[i].height, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (gSaveContext.options.zTargetSetting + 4)) {
            if (sSelectedSetting == FS_SETTING_ZTARGET) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gOptionsMenuSettingsIA4[i].texture, G_IM_FMT_IA,
                               gOptionsMenuSettingsIA4[i].width, gOptionsMenuSettingsIA4[i].height, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    vtx = 0;
    gDPPipeSync(POLY_OPA_DISP++);
    // check brightness bars
    gSPVertex(POLY_OPA_DISP++, D_80813EB0_ce0, 8, 0);
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_DrawOptionsImpl(GameState* thisx) {
    static s16 sCursorPrimRed = 255;
    static s16 sCursorPrimGreen = 255;
    static s16 sCursorPrimBlue = 255;
    static s16 sCursorEnvRed = 0;
    static s16 sCursorEnvGreen = 0;
    static s16 sCursorEnvBlue = 0;
    static s16 sCursorPulseDir = 1;
    static s16 sCursorFlashTimer = 20;
    static s16 sCursorPrimColors[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 sCursorEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    FileSelectState* this = (FileSelectState*)thisx;
    s16 cursorRedStep;
    s16 cursorGreenStep;
    s16 cursorBlueStep;
    s16 i;
    s16 j;
    s16 vtx;

    // #region 2S2H [GC US Support]
    if (ResourceMgr_GetGameVersion(0) == MM_NTSC_US_GC) {
        FileSelect_DrawOptionsImpl_NES_GC(thisx);
        return;
    }
    // #endregion

    OPEN_DISPS(this->state.gfxCtx);

    cursorRedStep = ABS_ALT(sCursorPrimRed - sCursorPrimColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorPrimGreen - sCursorPrimColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorPrimBlue - sCursorPrimColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorPrimRed >= sCursorPrimColors[sCursorPulseDir][0]) {
        sCursorPrimRed -= cursorRedStep;
    } else {
        sCursorPrimRed += cursorRedStep;
    }

    if (sCursorPrimGreen >= sCursorPrimColors[sCursorPulseDir][1]) {
        sCursorPrimGreen -= cursorGreenStep;
    } else {
        sCursorPrimGreen += cursorGreenStep;
    }

    if (sCursorPrimBlue >= sCursorPrimColors[sCursorPulseDir][2]) {
        sCursorPrimBlue -= cursorBlueStep;
    } else {
        sCursorPrimBlue += cursorBlueStep;
    }

    cursorRedStep = ABS_ALT(sCursorEnvRed - sCursorEnvColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorEnvGreen - sCursorEnvColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorEnvBlue - sCursorEnvColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorEnvRed >= sCursorEnvColors[sCursorPulseDir][0]) {
        sCursorEnvRed -= cursorRedStep;
    } else {
        sCursorEnvRed += cursorRedStep;
    }

    if (sCursorEnvGreen >= sCursorEnvColors[sCursorPulseDir][1]) {
        sCursorEnvGreen -= cursorGreenStep;
    } else {
        sCursorEnvGreen += cursorGreenStep;
    }

    if (sCursorEnvBlue >= sCursorEnvColors[sCursorPulseDir][2]) {
        sCursorEnvBlue -= cursorBlueStep;
    } else {
        sCursorEnvBlue += cursorBlueStep;
    }

    if (--sCursorFlashTimer == 0) {
        sCursorPrimRed = sCursorPrimColors[sCursorPulseDir][0];
        sCursorPrimGreen = sCursorPrimColors[sCursorPulseDir][1];
        sCursorPrimBlue = sCursorPrimColors[sCursorPulseDir][2];

        sCursorEnvRed = sCursorEnvColors[sCursorPulseDir][0];
        sCursorEnvGreen = sCursorEnvColors[sCursorPulseDir][1];
        sCursorEnvBlue = sCursorEnvColors[sCursorPulseDir][2];

        sCursorFlashTimer = 20;

        if (++sCursorPulseDir > 1) {
            sCursorPulseDir = 0;
        }
    }

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    Matrix_Push();
    Matrix_Translate(0.0f, 0.1f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerTopVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.2f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerMiddleVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.4f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerBottomVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    gSPVertex(POLY_OPA_DISP++, D_80813DF0, 32, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (i = 0, vtx = 0; i < 5; i++, vtx += 4) {
        if (i == 4) {
            gDPPipeSync(POLY_OPA_DISP++);
            if (gSaveContext.options.audioSetting == SAVE_AUDIO_SURROUND) {
                gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuHeaders[i].texture, G_IM_FMT_IA, G_IM_SIZ_8b,
                            gOptionsMenuHeaders[i].width, gOptionsMenuHeaders[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPVertex(POLY_OPA_DISP++, D_80813F30, 32, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == gSaveContext.options.audioSetting) {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuSettings[i].texture, G_IM_FMT_IA, G_IM_SIZ_8b,
                            gOptionsMenuSettings[i].width, gOptionsMenuHeaders[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (gSaveContext.options.zTargetSetting + 4)) {
            if (sSelectedSetting == FS_SETTING_ZTARGET) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        //! @bug the gOptionsMenuHeaders usage here will produce an OoB read for i == 5. It reads the first element of
        //! `gOptionsMenuSettings`
        // 2S2H [Port] Use `gOptionsMenuSettings` instead for correct height values matching textures
        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuSettings[i].texture, G_IM_FMT_IA, G_IM_SIZ_8b,
                            gOptionsMenuSettings[i].width, gOptionsMenuSettings[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // check brightness bars
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_DrawOptionsImpl_PAL(GameState* thisx) {
    static s16 sCursorPrimRed = 255;
    static s16 sCursorPrimGreen = 255;
    static s16 sCursorPrimBlue = 255;
    static s16 sCursorEnvRed = 0;
    static s16 sCursorEnvGreen = 0;
    static s16 sCursorEnvBlue = 0;
    static s16 sCursorPulseDir = 1;
    static s16 sCursorFlashTimer = 20;
    static s16 sCursorPrimColors[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 sCursorEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    FileSelectState* this = (FileSelectState*)thisx;
    s16 cursorRedStep;
    s16 cursorGreenStep;
    s16 cursorBlueStep;
    s16 i;
    s16 vtx;

    u8 languageIndex = FileSelect_GetPalLanguageIndex();

    // The PAL extraction offsets already crop each localized Targeting label
    // to the correct horizontal start. Keep the same quad origin for every
    // language; only German and Spanish need the wider source texture.
    u16 targetingWidth = ((languageIndex == 1) || (languageIndex == 3)) ? 144 : 64;
    s16 targetingX = -100;

    Vtx* targetingHeaderVtx = GRAPH_ALLOC(this->state.gfxCtx, 4 * sizeof(Vtx));

    targetingHeaderVtx[0].v.ob[0] = targetingHeaderVtx[2].v.ob[0] = targetingX;

    targetingHeaderVtx[1].v.ob[0] = targetingHeaderVtx[3].v.ob[0] = targetingX + targetingWidth;

    targetingHeaderVtx[0].v.ob[1] = targetingHeaderVtx[1].v.ob[1] = 15;

    targetingHeaderVtx[2].v.ob[1] = targetingHeaderVtx[3].v.ob[1] = -1;

    for (i = 0; i < 4; i++) {
        targetingHeaderVtx[i].v.ob[2] = 0;
        targetingHeaderVtx[i].v.flag = 0;

        targetingHeaderVtx[i].v.cn[0] = 255;
        targetingHeaderVtx[i].v.cn[1] = 255;
        targetingHeaderVtx[i].v.cn[2] = 255;
        targetingHeaderVtx[i].v.cn[3] = 255;
    }

    targetingHeaderVtx[0].v.tc[0] = 0;
    targetingHeaderVtx[0].v.tc[1] = 0;

    targetingHeaderVtx[1].v.tc[0] = targetingWidth << 5;
    targetingHeaderVtx[1].v.tc[1] = 0;

    targetingHeaderVtx[2].v.tc[0] = 0;
    targetingHeaderVtx[2].v.tc[1] = 16 << 5;

    targetingHeaderVtx[3].v.tc[0] = targetingWidth << 5;
    targetingHeaderVtx[3].v.tc[1] = 16 << 5;

    const PalOptionsTextures* textures = &sPalOptionsTextures[languageIndex];

    const TexturePtr palHeaders[] = {
        textures->options, textures->sound, textures->targeting, textures->language, textures->checkBrightness,
    };

    const TexturePtr palSettings[] = {
        textures->stereo,   textures->mono,         textures->headset,
        textures->surround, textures->switchTarget, textures->holdTarget,
    };

    OPEN_DISPS(this->state.gfxCtx);

    cursorRedStep = ABS_ALT(sCursorPrimRed - sCursorPrimColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorPrimGreen - sCursorPrimColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorPrimBlue - sCursorPrimColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorPrimRed >= sCursorPrimColors[sCursorPulseDir][0]) {
        sCursorPrimRed -= cursorRedStep;
    } else {
        sCursorPrimRed += cursorRedStep;
    }

    if (sCursorPrimGreen >= sCursorPrimColors[sCursorPulseDir][1]) {
        sCursorPrimGreen -= cursorGreenStep;
    } else {
        sCursorPrimGreen += cursorGreenStep;
    }

    if (sCursorPrimBlue >= sCursorPrimColors[sCursorPulseDir][2]) {
        sCursorPrimBlue -= cursorBlueStep;
    } else {
        sCursorPrimBlue += cursorBlueStep;
    }

    cursorRedStep = ABS_ALT(sCursorEnvRed - sCursorEnvColors[sCursorPulseDir][0]) / sCursorFlashTimer;
    cursorGreenStep = ABS_ALT(sCursorEnvGreen - sCursorEnvColors[sCursorPulseDir][1]) / sCursorFlashTimer;
    cursorBlueStep = ABS_ALT(sCursorEnvBlue - sCursorEnvColors[sCursorPulseDir][2]) / sCursorFlashTimer;

    if (sCursorEnvRed >= sCursorEnvColors[sCursorPulseDir][0]) {
        sCursorEnvRed -= cursorRedStep;
    } else {
        sCursorEnvRed += cursorRedStep;
    }

    if (sCursorEnvGreen >= sCursorEnvColors[sCursorPulseDir][1]) {
        sCursorEnvGreen -= cursorGreenStep;
    } else {
        sCursorEnvGreen += cursorGreenStep;
    }

    if (sCursorEnvBlue >= sCursorEnvColors[sCursorPulseDir][2]) {
        sCursorEnvBlue -= cursorBlueStep;
    } else {
        sCursorEnvBlue += cursorBlueStep;
    }

    if (--sCursorFlashTimer == 0) {
        sCursorPrimRed = sCursorPrimColors[sCursorPulseDir][0];
        sCursorPrimGreen = sCursorPrimColors[sCursorPulseDir][1];
        sCursorPrimBlue = sCursorPrimColors[sCursorPulseDir][2];

        sCursorEnvRed = sCursorEnvColors[sCursorPulseDir][0];
        sCursorEnvGreen = sCursorEnvColors[sCursorPulseDir][1];
        sCursorEnvBlue = sCursorEnvColors[sCursorPulseDir][2];

        sCursorFlashTimer = 20;

        if (++sCursorPulseDir > 1) {
            sCursorPulseDir = 0;
        }
    }

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSPVertex(POLY_OPA_DISP++, sOptionsDividersPALVtx, 16, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    {
        const u16 palHeaderHeights[] = {
            16, 16, 16, 16, 16,
        };

        const u16 palHeaderWidths[] = {
            128, 64, targetingWidth, 64, 128,
        };

        gSPVertex(POLY_OPA_DISP++, sOptionsMenuHeadersPALVtx, 24, 0);

        // Replace vertices 8 through 11, which correspond to the Targeting label.
        gSPVertex(POLY_OPA_DISP++, targetingHeaderVtx, 4, 8);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        for (i = 0, vtx = 0; i < 5; i++, vtx += 4) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

            gDPLoadTextureBlock_4b(POLY_OPA_DISP++, palHeaders[i], G_IM_FMT_IA, palHeaderWidths[i], palHeaderHeights[i],
                                   0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPVertex(POLY_OPA_DISP++, sOptionsMenuSettingsPALVtx, 24, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == gSaveContext.options.audioSetting) {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, palSettings[i], G_IM_FMT_IA, 48, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (gSaveContext.options.zTargetSetting + 4)) {
            if (sSelectedSetting == FS_SETTING_ZTARGET) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, palSettings[i], G_IM_FMT_IA, 48, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    /*
     * The PAL Dolby and Surround quads overlap slightly.
     * Keep the original PAL vertex positions.
     */
    gSPVertex(POLY_OPA_DISP++, sOptionsMenuHeadersPALVtx, 24, 0);

    gDPPipeSync(POLY_OPA_DISP++);

    if (gSaveContext.options.audioSetting == SAVE_AUDIO_SURROUND) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, this->titleAlpha[FS_TITLE_CUR]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    }

    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelDolbySurroundLogoENGTex, G_IM_FMT_I, G_IM_SIZ_8b, 48, 17, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    /* Sixth header quad: vertices 20 through 23. */
    gSP1Quadrangle(POLY_OPA_DISP++, 20, 22, 23, 21, 0);

    gSPVertex(POLY_OPA_DISP++, sOptionsLanguageSettingsPALVtx, 16, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == FileSelect_GetPalLanguageIndex()) {
            if (sSelectedSetting == FS_SETTING_LANGUAGE) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrimRed, sCursorPrimGreen, sCursorPrimBlue,
                                this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnvRed, sCursorEnvGreen, sCursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[FS_TITLE_CUR]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sPalLanguageChoiceTextures[i], G_IM_FMT_IA, 48, 16, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gSPVertex(POLY_OPA_DISP++, sOptionsBrightnessPALVtx, 8, 0);

    vtx = 0;

    // check brightness bars
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_DrawOptions(GameState* thisx) {
    if (ResourceMgr_HasGameVersion(MM_PAL_11)) {
        FileSelect_DrawOptionsImpl_PAL(thisx);
    } else {
        FileSelect_DrawOptionsImpl(thisx);
    }
}
