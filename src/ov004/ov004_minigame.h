#ifndef OV004_MINIGAME_H
#define OV004_MINIGAME_H

// Overlay 4: the animal care minigames (milking, petting, shearing...). Class and function names are
// ours; unknown ARM9 functions keep their dsd names.
#include <nitro.h>
#include "Task.h"

// A list of objects updated/drawn by a minigame (ARM9 0x02068a40.., vtable 0x020c1048).
class MgObjList {
public:
    MgObjList();
    ~MgObjList();
    void Clear();    // 0x02068c1c
    void Update();   // 0x02068cac
    void Draw();     // 0x02068d04
    void Func4();    // 0x02068d5c

    void *vtbl;
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
    void Release();
    void Draw();

    void *mRes;
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

extern MiniGameBase *gMiniGame;

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

#endif
