#ifndef OV005_TITLE_H
#define OV005_TITLE_H

#include <nitro.h>
#include <nnsys/g2d/g2d_CellAnimation.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include "Screen2D.h"
#include "XbbSprite.h"
#include "Model3D.h"
#include "Button.h"
#include "Archive.h"
#include "Task.h"
#include "ov005_flow.h"
#include "game_file.h"
#include "input.h"

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
void func_02035f30(void);
void func_02035e44(int bgm);
void func_02035ed4(int a, int b);
void func_02037238(void);
void func_02037410(void);
void func_02035d88(void);
void func_020709bc(void *obj);
void func_02028530(void);
void func_02029fe8(void *obj);
void func_0202bbf4(void *obj);
void func_0202bd70(void *obj);
void func_0202bdd4(void *obj);
void func_0202a1a0(void *obj, int a);
void func_0202abe4(void *obj);
void func_02034c60(void *obj, int a);
void func_02034c10(void *obj);
extern void *data_020ec870;
extern void *data_020f57c0;
extern u8 data_020ec698;
extern u8 data_020f57d0;
int func_0205ade8(int slot, void *buf, int size);   // read a save slot header
}

class TitleLogo;

// The title overlay's main scene (overlay 5, 0x02158d20): graphics setup, the task managers and the
// hand-over to the game once a menu entry was chosen.
class TitleMain : public Task {
public:
    TitleMain();
    virtual ~TitleMain();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();

    void InitGraphics();
    static void BeginFrame();
    void SetupLight();
    void StartGame(int choice);

    u8 mLightR;
    u8 mLightG;
    u8 mLightB;
    u8 mState;
    u8 mChoice;
};

// Task managers of the title overlay (0x0215bb60).
extern TaskManager *sTitleManagers[10];


// Switches between the title overlay's screens (overlay 5, 0x02159478).
class TitleFlow : public Task {
public:
    TitleFlow(TitleMain *main);
    virtual ~TitleFlow();
    virtual BOOL Update();

    static BOOL ChangeScene(u8 next, u8 fade);
    static BOOL SetNext(u8 next);
    void KillCurrent();
    void CreateScene();

    TitleMain *mMain;
};

// A pack of files read by index (ARM9 0x0202eb5c..; names are ours).
class PackFile {
public:
    PackFile(const char *path);
    virtual ~PackFile();
    virtual void Func2();
    virtual u32 GetCount();
    virtual u32 GetSize(u32 index);
    virtual void Read(u32 index, u32 size, void *dst);

    u8 unk4[0x10 - 4];
};

extern "C" {
void *func_0202feb0(const char *path);                       // load a file
void *func_0202ff68(const char *path);                       // load a texture file
Model3D *func_020300b8(void *mdl, void *tex, int index);
void func_02030298(Model3D *model, void *anm, int type);
void func_0203048c(void *tex);
Model3D *func_020301f4(void *mdl, void *tex, int index);
Model3D *func_02030154(void *mdl, int index);
void func_02030374(Model3D *model, void *anm, int type, int loop);
}

// A placement in the .lot file (0x28 bytes).
struct LotEntry {
    char name[0x10];
    VecFx32 pos;
    u8 unk1C[0xC];
};

// Header of the .lot file.
struct LotHeader {
    u8 unk0[0x18C];
    u32 entryOfs;
    u32 numEntries;
    u8 unk194[0x10];
};

// The title screen's 3D stage: the rom/maps/title model, its objects and the ship
// (overlay 5, 0x021597ac).
class TitleStage : public Task {
public:
    TitleStage();
    virtual ~TitleStage();
    virtual BOOL Update();
    virtual void Draw();

    BOOL LoadLot(const char *lot);
    BOOL LoadModels(const char *mdl, const char *ta, const char *ca, const char *ma, const char *tex);
    BOOL LoadObjects(const char *idx, const char *bin, const char *ltr, const char *wna, const char *txa,
                     const char *mta, const char *cha, const char *tex);
    BOOL CopyPrefix(char *dst, const char *src);
    int CountLot(const char *prefix);
    BOOL PlaceModel(Model3D *model, const char *prefix, BOOL first);
    void MoveShip(const VecFx32 *delta);
    void SetShipPos(const VecFx32 *pos);
    void SetShipPos(fx32 x, fx32 y, fx32 z);
    Model3D *GetShip() { return mShip == -1 ? mObjects[0] : mObjects[mShip]; }

    NNSG3dResFileHeader *mMdlRes;
    void *mTexRes;
    NNSGfdTexKey mTexKey;
    NNSGfdPlttKey mPlttKey;
    void *mAnmRes;
    void *unk28;
    void *unk2C;
    Model3D *mModels[10];   // only 8 are used
    Model3D *mObjects[32];
    u8 *mObjTex[32];
    NNSGfdTexKey mObjTexKey[32];
    NNSGfdPlttKey mObjPlttKey[32];
    int mShip;
    PackFile *mBin;
    PackFile *unk260;
    PackFile *unk264;
    PackFile *mTxa;
    PackFile *unk26C;
    PackFile *unk270;
    PackFile *mTexPack;
    LotHeader mLot;
    LotEntry *mEntries;
};

// The title screen's 3D camera (overlay 5, 0x0215aab4): orthographic projection, looking at the
// stage from above-front.
class TitleCamera : public Task {
public:
    TitleCamera();
    virtual ~TitleCamera();   // D1 at 0x0215ad1c; D0 at 0x02157888 (in TitleMenu.cpp's code)
    virtual BOOL Update();
    virtual void Draw();

    void SetFocus(fx32 x, fx32 z);

    VecFx32 mPos;
    VecFx32 mTarget;
};

// A villager or animal shown on the title screen's 3D stage, with an optional mark sprite
// (overlay 5, 0x0215ad30).
class TitleChar : public XbbSprite {
public:
    TitleChar(int person, Sprite2D mark);
    TitleChar(u32 animal, Sprite2D mark);
    virtual ~TitleChar();
    virtual BOOL Update();
    virtual void Func3();
    virtual void Func4();

    void InitMark();

    int mPerson;
    int mAnimal;
    Sprite2D *mMark;
};

// Screens of the title overlay implemented in ARM9 (names unknown).
class TitleSubScene4D : public Task {
public:
    TitleSubScene4D(TitleMain *main);   // 0x020aa9bc
    u8 unk14[0x20 - 0x14];
};

class TitleSubScene4E : public Task {
public:
    TitleSubScene4E();                  // 0x020ab4fc
    u8 unk14[0x38 - 0x14];
};
// A 2D sprite drawn by the renderer at data_020ec87c (ARM9 0x0202b...).
struct Sprite2D {
    u8 unk0[0x10];
    fx32 y;
    u8 unk14[0xa];
    u8 alpha;
    u8 unk1F[0x48 - 0x1F];
    const NNSG2dCellDataBank *cellBank;
    const NNSG2dAnimBankData *animBank;
    NNSG2dCellAnimation *cellAnim;
    u8 unk54[0x17C - 0x54];
};
extern "C" {
void func_0202bc20(void *renderer, Sprite2D *sprite);   // draw
void func_0202b088(Sprite2D *sprite, int value);
void func_0202b064(Sprite2D *sprite, int palette);
void func_0202b1a4(Sprite2D *sprite, fx32 x, fx32 y, fx32 z);   // set position
void func_0202ba04(Sprite2D *sprite, int unk);                   // release
}

class TitleMenu;

// The title screen task (overlay 5, 0x02156c3c).
class TitleScene : public Task {
public:
    TitleScene(TitleMain *main);
    virtual ~TitleScene();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();

    void OnLogoDone();
    void Select(u8 index);
    Archive *GetArchive();
    u8 HasSaveData();
    void ShowMenu();
    BOOL IsIdle();
    BOOL IsLoaded();
    static BOOL CheckSaveData();

    u8 mHasSave;   // at 0x11: mwcc packs it into Task's tail padding
    u8 unk12[6];
    u8 mState;
    u16 unk1A;
    s8 mTimer;
    TitleMain *mMain;
    TitleMenu *mMenu;
    TitleLogo *mLogo;
    int mChoice;
    Archive *mArchive;
    u8 *mArchiveData;
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
    TitleMenu(TitleScene *scene);
    virtual ~TitleMenu();
    void Animate();                             // 0x021578a4
    void Draw();                                // 0x02157bac
    void DrawBlend();                           // 0x02157bb0
    void ResetButtons();                        // 0x02157bb4
    void UpdateButtons(BOOL active);            // 0x02157be4
    void BlinkPrompt();                         // 0x02157db8
    u8 HasSaveData();                           // 0x02158ba8
    TitleScene *GetScene();                     // 0x02158bb8
    Sprite2D *GetButtonSprite(u8 index, int part);   // 0x02158bc0
    Sprite2D *GetCursorSprite();                // 0x02158bfc
    void Show();                                // 0x02158c04

    u8 unk4[0x370 - 4];
};

#endif
