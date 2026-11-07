#ifndef OV005_TITLE_H
#define OV005_TITLE_H

#include <nitro.h>
#include "Screen2D.h"
#include "Button.h"
#include "Archive.h"
#include "Task.h"
#include "game_file.h"

// ARM9 2D helpers used by the title screen (names unknown yet).
extern "C" {
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
void func_02035e8c(int se, int unk);   // play a sound effect
extern void *data_020ec87c;            // the 2D sprite renderer
BOOL func_020374f8(int screen);        // a screen fade is running
void func_02037284(int type, int a, int b);   // start a screen fade
void func_ov005_02159608(int a, int b);
}

// A 2D sprite drawn by the renderer at data_020ec87c (ARM9 0x0202b...).
struct Sprite2D {
    u8 unk0[0x10];
    fx32 y;
    u8 unk14[0xa];
    u8 alpha;
};
extern "C" {
void func_0202bc20(void *renderer, Sprite2D *sprite);   // draw
void func_0202b088(Sprite2D *sprite, int value);
}

class TitleMenu;

// The title screen task (overlay 5, 0x02156c3c..).
class TitleScene {
public:
    void OnLogoDone();       // 0x021571c0
    void Select(u8 index);   // 0x021571cc
    Archive *GetArchive();   // 0x0215720c
    BOOL IsLoaded();         // 0x02157288
    u8 HasSaveData();        // 0x02157214

    u8 unk0[0x18];
    u8 mState;
    u8 unk19[0x30 - 0x19];
    Archive *mArchive;
};

// The license / publisher logos shown before the title (overlay 5, 0x02156750).
class SplashScreen : public Task {
public:
    SplashScreen(void *owner);
    virtual ~SplashScreen();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();

    void *mOwner;
    u8 mFrame;
    Archive *mArchive;
    u8 *mArchiveData;
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

// One of the two bottom-screen title buttons (overlay 5, 0x021562f4).
class TitleButton : public Button {
public:
    TitleButton();
    virtual ~TitleButton();
    virtual void OnTouch();
    virtual void OnLeave();
    virtual void OnDecide();

    BOOL Draw(BOOL active);
    static TitleButton *Create(TitleMenu *menu);
    void Init(TitleMenu *menu, u8 index, u8 hasSave);
    void ClearPressed();
    void SetTouched(u8 touched);
    void Decide();
    u8 IsTouched();
    u8 IsDecided();

    u8 mHasSave;
    u8 mPressed;
    u8 mDecided;
    u8 mTouched;
    u8 mIndex;
    TitleMenu *mMenu;
    Sprite2D *mSprites[3];
    u8 unk28[4];
};

// The bottom-screen title menu (overlay 5, 0x02157310..).
class TitleMenu {
public:
    u8 HasSaveData();                           // 0x02158ba8
    TitleScene *GetScene();                     // 0x02158bb8
    Sprite2D *GetButtonSprite(u8 index, int part);   // 0x02158bc0
    Sprite2D *GetCursorSprite();                // 0x02158bfc
};

#endif
