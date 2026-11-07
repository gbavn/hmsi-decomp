#ifndef OV005_TITLE_H
#define OV005_TITLE_H

#include <nitro.h>
#include "Screen2D.h"

// ARM9 2D helpers used by the title screen (names unknown yet).
extern "C" {
void *func_02036fdc(void *archive, const char *name);   // file data inside an archive
u32 func_02037010(void *archive, const char *name);     // file size inside an archive
void func_02036404(void *anim, void *data);
void func_0203646c(void *anim, int frame);
int func_020365f8(void *data, int vram, int offset, int unk);
void func_020366c4(void *buf, int count, int unk);
void func_02036708(int a, int b);
void func_0203675c(int a, int b, fx32 x, fx32 y, void *anim);
void func_020368b4(void);
void func_02036918(int a);
void func_02036974(int a);
void func_02036c74(int vram, int offset);
int func_02036d48(int vram);
extern u8 data_0214d920[];
}

// The title screen task (overlay 5, 0x02156c3c..).
class TitleScene {
public:
    void OnLogoDone();       // 0x021571c0
    void *GetArchive();      // 0x0215720c
    BOOL IsLoaded();         // 0x02157288

    u8 unk0[0x18];
    u8 mState;
    u8 unk19[0x30 - 0x19];
    void *mArchive;
};

// The top-screen logo: sky, two logo layers and a lens flare sprite, fading in (overlay 5, 0x02155c80).
class TitleLogo : public Screen2D {
public:
    TitleLogo(TitleScene *scene);
    virtual ~TitleLogo();
    BOOL Update();
    void DrawFlare();
    void DrawBlend();
    void Skip();
    void LoadFlare();

    u8 mFlicker;
    u16 mTimer;
    u16 mFrame;
    TitleScene *mScene;
    u8 mFlareAnim[0x2c];
    int mFlareChar;
    u8 unk40[8];
    int mFade;
    int unk4C;
};

#endif
