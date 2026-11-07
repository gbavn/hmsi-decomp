// Overlay 5, part 1: the title screen's top-screen logo.
#include "ov005_title.h"

#define LOGO_FRAMES 270

TitleLogo::TitleLogo(TitleScene *scene)
{
    void *arc;

    mFade = 0;
    unk4C = 0;
    mScene = scene;
    mTimer = 0;
    mFrame = 0;
    mFlicker = 0xFF;
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1);
    G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                          GX_BLEND_PLANEMASK_BD,
                      0, 31);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_SetBG0Priority(2);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_SetBG1Priority(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2800,
                          GX_BG_CHARBASE_0x10000);
    G2S_SetBG2Priority(0);
    arc = mScene->GetArchive();
    GXS_LoadBG0Char(func_02036fdc(arc, "title_up_rogo.kncg"), 0, func_02037010(arc, "title_up_rogo.kncg"));
    GXS_LoadBGPltt(func_02036fdc(arc, "title_up_rogo.kncl"), 0, func_02037010(arc, "title_up_rogo.kncl"));
    GXS_LoadBG0Scr(func_02036fdc(arc, "title_up_sky.knsc"), 0, func_02037010(arc, "title_up_sky.knsc"));
    GXS_LoadBG1Scr(func_02036fdc(arc, "title_up_rogo.knsc"), 0, func_02037010(arc, "title_up_rogo.knsc"));
    GXS_LoadBG2Char(func_02036fdc(arc, "title_up_rogo2.kncg"), 0, func_02037010(arc, "title_up_rogo2.kncg"));
    GXS_LoadBGPltt(func_02036fdc(arc, "title_up_rogo2.kncl"), 0, func_02037010(arc, "title_up_rogo2.kncl"));
    GXS_LoadBG2Scr(func_02036fdc(arc, "title_up_rogo2.knsc"), 0, func_02037010(arc, "title_up_rogo2.knsc"));
    LoadFlare();
}

TitleLogo::~TitleLogo()
{
    G2S_BlendNone();
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3);
    G2S_SetBG0Offset(0, 0);
    G2S_SetBG1Offset(0, 0);
}

BOOL TitleLogo::Update()
{
    if (!mScene->IsLoaded()) {
        return TRUE;
    }
    mTimer++;
    if (mTimer > 60 && mFrame < LOGO_FRAMES) {
        f32 t;
        mFrame++;
        t = mFrame / 270.0f;
        G2S_SetBG1Offset(0, (int)(64.0f * t) - 64);
        {
            int a = (int)(31.0f * t);
            G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG1,
                              GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                                  GX_BLEND_PLANEMASK_BD,
                              a, 31 - a);
        }
    } else if (mFrame == LOGO_FRAMES) {
        mFrame++;
        mScene->OnLogoDone();
    } else if (mFrame > LOGO_FRAMES) {
        GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ);
        if (mFlicker > 0xBF && mFade != 0) {
            mFade--;
            if (mFade < 6) {
                mFade = 6;
            }
        }
    }
    if (mFlicker != 0) {
        mFlicker--;
        if (mFlicker < 0xBF && (mFlicker & 1) && mFade > 0) {
            mFade--;
        }
    } else {
        mFlicker = 0xFF;
        mFade = 24;
    }
    G2S_SetBG0Offset(mFlicker, 0);
    return TRUE;
}

void TitleLogo::DrawFlare()
{
    if (mFrame < LOGO_FRAMES) {
        return;
    }
    func_02036974(3);
    func_020366c4(data_0214d920, 0x60, 2);
    func_02036708(0, 0);
    func_0203646c(mFlareAnim, 0);
    func_0203675c(1, 0, FX32_CONST(128), FX32_CONST(96), mFlareAnim);
    func_020368b4();
}

void TitleLogo::DrawBlend()
{
    if (mFrame < LOGO_FRAMES) {
        return;
    }
    func_02036918(2);
    G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_OBJ, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
                      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3, mFade, 31 - mFade);
}

void TitleLogo::Skip()
{
    mFrame = LOGO_FRAMES + 1;
    G2S_SetBG1Offset(0, 0);
    G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                          GX_BLEND_PLANEMASK_BD,
                      31, 0);
}

static inline void *GetF(void *a, const char *n) { return func_02036fdc(a, n); }
void TitleLogo::LoadFlare()
{
    void *arc = mScene->GetArchive();
    int ofs = 0;
    int size;

    func_02036c74(2, ofs);
    size = func_020365f8(GetF(arc, "title_up_Lens_flare.knch"), 2, func_02036d48(2), 0);
    func_02036404(mFlareAnim, GetF(arc, "title_up_Lens_flare.knce"));
    mFlareChar = func_02036d48(2);
    ofs += size;
    func_02036c74(2, ofs);
    GXS_LoadOBJPltt(GetF(arc, "title_up_Lens_flare.kncl"), 0,
                    func_02037010(arc, "title_up_Lens_flare.kncl"));
}
