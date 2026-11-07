// Overlay 5, part 3: the license and publisher logo screens shown before the title.
#include "ov005_title.h"

SplashScreen::SplashScreen(void *owner)
{
    mOwner = owner;
    mFrame = 0;
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2S_SetBG0Priority(0);
    {
        GFileObject file;
        mArchiveData = (u8 *)GFile_Load(file.Get(), "rom/console/Title_Logo.LZ", 0);
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), mArchiveData);
        mArchive = new Archive(mArchiveData);
        GFile_Close(file.Get());
        GXS_LoadBG0Char(mArchive->GetFile("title_up_license.kncg"), 0, mArchive->GetSize("title_up_license.kncg"));
        GXS_LoadBGPltt(mArchive->GetFile("title_up_license.kncl"), 0, mArchive->GetSize("title_up_license.kncl"));
        GXS_LoadBG0Scr(mArchive->GetFile("title_up_license.knsc"), 0, mArchive->GetSize("title_up_license.knsc"));
    }
}

SplashScreen::~SplashScreen()
{
    if (mArchive != NULL) {
        delete mArchive;
    }
    delete[] mArchiveData;
}

BOOL SplashScreen::Update()
{
    mFrame++;
    if (mFrame == 1) {
        if (func_020374f8(1) || func_020374f8(0)) {
            return TRUE;
        }
        func_02037284(2, 0, 0);
        return TRUE;
    } else if (mFrame == 60) {
        if (func_020374f8(1) || func_020374f8(0)) {
            mFrame--;
            return TRUE;
        }
        func_02037284(7, 0, 0);
        return TRUE;
    } else if (mFrame == 90) {
        if (func_020374f8(1) || func_020374f8(0)) {
            mFrame--;
            return TRUE;
        }
        GXS_LoadBG0Char(mArchive->GetFile("title_up_Rising.kncg"), 0, mArchive->GetSize("title_up_Rising.kncg"));
        GXS_LoadBGPltt(mArchive->GetFile("title_up_Rising.kncl"), 0, mArchive->GetSize("title_up_Rising.kncl"));
        GXS_LoadBG0Scr(mArchive->GetFile("title_up_Rising.knsc"), 0, mArchive->GetSize("title_up_Rising.knsc"));
        func_02037284(2, 0, 0);
        return TRUE;
    } else if (mFrame == 150) {
        func_02037284(7, 0, 0);
        return TRUE;
    } else if (mFrame == 180) {
        if (func_020374f8(1) || func_020374f8(0)) {
            mFrame--;
            return TRUE;
        }
        GXS_LoadBG0Char(mArchive->GetFile("title_up_mmv.kncg"), 0, mArchive->GetSize("title_up_mmv.kncg"));
        GXS_LoadBGPltt(mArchive->GetFile("title_up_mmv.kncl"), 0, mArchive->GetSize("title_up_mmv.kncl"));
        GXS_LoadBG0Scr(mArchive->GetFile("title_up_mmv.knsc"), 0, mArchive->GetSize("title_up_mmv.knsc"));
        func_02037284(2, 0, 0);
        return TRUE;
    } else if (mFrame == 240) {
        func_ov005_02159608(0x42, 1);
        return FALSE;
    }
    return TRUE;
}

void SplashScreen::Draw()
{
}

void SplashScreen::Func4()
{
}

// The original emits its own (unused) copy of GFile's vtable here; its references all go to ARM9's.
// (func_0202aec0/func_0202afe0 are GFile's destructors; named so to not clash with the inline ones.)
extern "C" void func_0202aec0(void);
extern "C" void func_0202afe0(void);
void *gSplashFileVtable[4] = {NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0};
