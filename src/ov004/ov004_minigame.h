#ifndef OV004_MINIGAME_H
#define OV004_MINIGAME_H

// Overlay 4: the animal care minigames (milking, petting, shearing...). Class and function names are
// ours; unknown ARM9 functions keep their dsd names.
#include <nitro.h>
#include "Task.h"

// A group of a minigame's tasks (ARM9).
class MgGroup {
public:
    void Func();     // 0x02068ec4 (empty)

    TaskManager *mManager;
};

// The task groups of a minigame (ARM9 0x02068a40.., vtable 0x020c1048).
class MgObjList {
public:
    MgObjList();
    virtual ~MgObjList();
    virtual void Func2();
    virtual void Func3();
    virtual void Func4v();
    virtual void Func5();
    virtual void Func6();
    virtual MgGroup *GetGroup(int index);
    void Clear();    // 0x02068c1c
    void Update();   // 0x02068cac
    void Draw();     // 0x02068d04
    void Func4();    // 0x02068d5c

    u32 unk4;
    u32 unk8;
};

extern "C" {
int func_02043cdc(const char *name);   // open a resource, returns a handle
void func_02043d50(int *handle);      // release it
}

// Holds one sprite/BG resource (overlay 4, 0x021587d0..).
class SpriteRef {
public:
    SpriteRef();
    ~SpriteRef();
    void Setup(const char *a, const char *b, int x, int y, int w, int h, int unk);
    void Load(const char *ncbr, const char *nclr, int x, int y, int unk);   // 0x021587f0
    void SetPos(int x, int y);                                              // 0x021589ac
    void Release();
    void Draw();

    void *mRes;
};

// An animated sprite resource (overlay 4, 0x021585f8..).
class SpriteAnim {
public:
    void Load(const char *ncbr, const char *ncer, const char *nanr, const char *nclr, int w, int h, int unk);

    u8 unk0[4];
};

// A screen rectangle with a virtual destructor (ARM9; vtable 0x020bdf44, destructors 0x02047840 and
// 0x020488a4). The constructor is inline.
class MgRect {
public:
    MgRect(int x1, int y1, int x2, int y2) : mX1(x1), mY1(y1), mX2(x2), mY2(y2) {}
    virtual ~MgRect();

    int mX1;
    int mY1;
    int mX2;
    int mY2;
};

// A touch button task of the minigames (overlay 4, 0x021581ec..).
class MgButton : public Task {
public:
    MgButton(MgRect *rect, int unk);
    virtual ~MgButton();
    virtual BOOL Update();   // 0x021582f8
    virtual void Draw();     // 0x021584cc
    virtual void Func4();
    virtual void OnDecide();

    SpriteAnim mSprite;
};

// A minigame's 2D layer (overlay 4, 0x02155f40..).
class MgLayer : public SpriteRef {
public:
    MgLayer();
    ~MgLayer();
    void Init(const char *a, const char *b);
    void Clear();
    void Draw();
};

// Base of the minigame singletons: registers itself as the current minigame (0x0215d620).
class MiniGameBase {
public:
    MiniGameBase();
    virtual ~MiniGameBase();
};


// A minigame (overlay 4, 0x02155c80).
class MiniGame : public MiniGameBase {
public:
    MiniGame(int arg);
    virtual ~MiniGame();
    virtual void Func2();   // defined in ARM9 (0x0206a6cc, empty)
    virtual void Func3();
    virtual void Func4();

    void Run();
    void Draw();
    void DrawObjects();
    void SetDone();
    void AddCount(int n);
    void TickTimer();
    void LoadResource(const char *name);
    void ReleaseResource();
    void SetTimer(int frames);
    void SetCountMax(int max);

    int mArg;
    MgObjList mObjects;
    int mResource;
    MgLayer mLayer;
    u8 mDone;
    int mTimer;
    int mTimerMax;
    int mCount;
    int mCountMax;
};

extern MiniGame *gMiniGame;

// The animal of the petting minigame (overlay 4, 0x02157054..).
class MgAnimal {
public:
    void SetMood(int mood);   // 0x02157054

    u8 unk0[0xBC];
    int mX;
    int mY;
};

// The hand cursor of the petting minigame (overlay 4, 0x02155fdc).
class PetHand : public Task {
public:
    PetHand(MgAnimal *animal);
    virtual ~PetHand();
    virtual BOOL Update();
    virtual void Draw();

    static void *operator new(size_t size);
    static int GetMaxScore();
    static int GetRank();
    void Load();
    void Release();
    void UpdateTouch();
    void Stroke();
    void Rub(int dist);
    void Tick();
    void Func63F0();

    SpriteRef mSprite;
    MgAnimal *mAnimal;
    int mPrevX;
    int mPrevY;
    int mTimer;
    u8 mTouching;
    u8 mOnAnimal;
};

// The "end" button of the minigames (overlay 4, 0x021563f4).
class EndButton : public MgButton {
public:
    EndButton();
    virtual ~EndButton();
    virtual void OnDecide();

    static void *operator new(size_t size);
};

#endif
