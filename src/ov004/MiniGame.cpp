// Overlay 4, part 1: the minigame base class and its 2D layer.
#include "ov004_minigame.h"

MiniGame::MiniGame(int arg)
    : mArg(arg), mResource(0)
{
    mDone = 0;
    mTimer = 0;
    mTimerMax = 0;
    mCount = 0;
    mCountMax = 0;
}

MiniGame::~MiniGame()
{
    mObjects.Clear();
    ReleaseResource();
}

void MiniGame::Run()
{
    Func3();
    mObjects.Update();
    Func4();
}

void MiniGame::Func4()
{
}

void MiniGame::Func3()
{
}

void MiniGame::Draw()
{
    mObjects.Draw();
    mLayer.Draw();
}

void MiniGame::DrawObjects()
{
    mObjects.Func4();
}

void MiniGame::SetDone()
{
    mDone = 1;
}

#ifdef NONMATCHING
void MiniGame::AddCount(int n)
{
    mCount += n;
    mCount = mCount <= 0 ? 0 : mCount;
    if (mCount >= mCountMax) {
        mCount = mCountMax;
        SetDone();
    }
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN8MiniGame7SetDoneEv(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void MiniGame::AddCount(int n) {
    stmdb sp!, {r3, lr}
    ldr r2, [r0, #0x28]
    add r2, r2, r1
    str r2, [r0, #0x28]
    cmp r2, #0x0
    movle r2, #0x0
    str r2, [r0, #0x28]
    ldr r1, [r0, #0x2c]
    cmp r2, r1
    ldmltia sp!, {r3, pc}
    ldr r1, [r0, #0x2c]
    str r1, [r0, #0x28]
    bl _ZN8MiniGame7SetDoneEv
    ldmia sp!, {r3, pc}
}
#endif

void MiniGame::TickTimer()
{
    if (mTimer != 0 && mTimer != -1) {
        mTimer--;
    }
    if (mTimer == 0) {
        SetDone();
    }
}

void MiniGame::LoadResource(const char *name)
{
    ReleaseResource();
    mResource = func_02043cdc(name);
}

void MiniGame::ReleaseResource()
{
    if (mResource == 0) {
        return;
    }
    func_02043d50(&mResource);
}

void MiniGame::SetTimer(int frames)
{
    mTimerMax = frames;
    mTimer = frames;
}

void MiniGame::SetCountMax(int max)
{
    mCount = 0;
    mCountMax = max;
}

// gMiniGame: defined in MiniGameData.cpp (layout workaround, see there).

MiniGameBase::MiniGameBase()
{
    gMiniGame = (MiniGame *)this;
}

#ifdef NONMATCHING
MiniGameBase::~MiniGameBase()
{
    gMiniGame = NULL;
}
#else
// The original has the destructors in the order D2, D1, D0; mwcc emits D1, D0, D2, so they are asm
// here, in ROM order, and MiniGameBase's vtable is spelled out in MiniGameBaseVtable.cpp.
extern "C" {
void _ZdlPv(void);
}
extern "C" asm void _ZN12MiniGameBaseD2Ev(void) {
    ldr r1, =gMiniGame
    mov r2, #0x0
    str r2, [r1, #0x0]
    bx lr
}
extern "C" asm void _ZN12MiniGameBaseD1Ev(void) {
    ldr r1, =gMiniGame
    mov r2, #0x0
    str r2, [r1, #0x0]
    bx lr
}
extern "C" asm void _ZN12MiniGameBaseD0Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, =gMiniGame
    mov r2, #0x0
    mov r4, r0
    str r2, [r1, #0x0]
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
}
#endif

MgLayer::MgLayer()
{
}

MgLayer::~MgLayer()
{
    Clear();
}

void MgLayer::Init(const char *a, const char *b)
{
    Clear();
    Setup(a, b, 0x80, 0x60, 0x100, 0xC0, 0);
}

void MgLayer::Clear()
{
    Release();
}

void MgLayer::Draw()
{
    SpriteRef::Draw();
}
