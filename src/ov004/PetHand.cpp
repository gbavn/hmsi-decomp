// Overlay 4, part 2: the hand cursor of the petting minigame.
#include "ov004_minigame.h"
#include "input.h"

static const int sRankScores[7] = {200, 2000, 4000, 6000, 8000, 9999, 10000};

PetHand::PetHand(MgAnimal *animal)
{
    mAnimal = animal;
    mPrevX = 0;
    mPrevY = 0;
    mTimer = 0;
    mTouching = 0;
    mOnAnimal = 0;
    Load();
}

PetHand::~PetHand()
{
    Release();
}

BOOL PetHand::Update()
{
    UpdateTouch();
    if (!gMiniGame->mDone) {
        if (mTouching) {
            UpdateTouch();
            Stroke();
        }
        Tick();
    }
    Func63F0();
    return TRUE;
}

void PetHand::Draw()
{
    if (mTouching) {
        mSprite.Draw();
    }
}

void *PetHand::operator new(size_t size)
{
    MgGroup *group = gMiniGame->mObjects.GetGroup(3);
    group->Func();
    return Task::operator new(size, group->mManager);
}

int PetHand::GetMaxScore()
{
    return 10000;
}

int PetHand::GetRank()
{
    int count = gMiniGame->mCount;
    u32 i;

    for (i = 0; i < 7; i++) {
        if (count <= sRankScores[i]) {
            return i;
        }
    }
    return 0;
}

void PetHand::Load()
{
    Release();
    mSprite.Load("HAND.NCBR", "HAND.NCLR", 0x80, 0x60, 5);
}

void PetHand::Release()
{
    mSprite.Release();
}

#ifdef NONMATCHING
void PetHand::UpdateTouch()
{
    mTouching = (gInput.touchFlags & 0x10) ? 1 : 0;
    if (mTouching) {
        mSprite.SetPos(gInput.touchX, gInput.touchY);
    }
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN9SpriteRef6SetPosEii(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void PetHand::UpdateTouch() {
    stmdb sp!, {r3, lr}
    ldr r1, [pc, #0x34]
    ldrb r1, [r1, #0x0]
    tst r1, #0x10
    movne r1, #0x1
    moveq r1, #0x0
    strb r1, [r0, #0x28]
    tst r1, #0xff
    ldmeqia sp!, {r3, pc}
    ldr r1, [pc, #0x18]
    ldr r2, [pc, #0x18]
    ldrh r1, [r1, #0x0]
    ldrh r2, [r2, #0x0]
    add r0, r0, #0x14
    bl _ZN9SpriteRef6SetPosEii
    ldmia sp!, {r3, pc}
    dcd gInput
    dcd gInput+0xc
    dcd gInput+0x8
}
#endif

#ifdef NONMATCHING
void PetHand::Stroke()
{
    int x = gInput.touchX;
    int y = gInput.touchY;

    if (!mOnAnimal) {
        mPrevX = x;
        mPrevY = y;
    }
    mOnAnimal = (y - mAnimal->mY) * (y - mAnimal->mY) + (x - mAnimal->mX) * (x - mAnimal->mX) < 48 * 48;
    if (mOnAnimal) {
        int d = (y - mPrevY) * (y - mPrevY) + (x - mPrevX) * (x - mPrevX);
        Rub((int)FX_FX32_TO_F32(FX_Sqrt(FX32_CONST(d))));
    }
    mPrevX = x;
    mPrevY = y;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN7PetHand3RubEi(void);
void _fadd(void);
void _fdiv(void);
void _ffix(void);
void _fflt(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void PetHand::Stroke() {
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r0
    ldr r1, [pc, #0xc0]
    ldr r0, [pc, #0xc0]
    ldrb r2, [r6, #0x29]
    ldrh r4, [r1, #0x0]
    ldrh r5, [r0, #0x0]
    cmp r2, #0x0
    streq r4, [r6, #0x1c]
    streq r5, [r6, #0x20]
    ldr r1, [r6, #0x18]
    ldr r0, [r1, #0xc0]
    ldr r2, [r1, #0xbc]
    sub r1, r5, r0
    mul r0, r1, r1
    sub r1, r4, r2
    mla r0, r1, r1, r0
    cmp r0, #0x900
    movlt r0, #0x1
    movge r0, #0x0
    strb r0, [r6, #0x29]
    tst r0, #0xff
    beq _021562f0
    ldr r0, [r6, #0x20]
    ldr r2, [r6, #0x1c]
    sub r1, r5, r0
    mul r0, r1, r1
    sub r1, r4, r2
    mla r0, r1, r1, r0
    cmp r0, #0x0
    mov r0, r0, lsl #0xc
    ble _021562c0
    bl _fflt
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _021562cc
_021562c0:
    bl _fflt
    mov r1, #0x3f000000
    bl _fsub
_021562cc:
    bl _ffix
    bl FX_Sqrt
    bl _fflt
    ldr r1, [pc, #0x24]
    bl _fdiv
    bl _ffix
    mov r1, r0
    mov r0, r6
    bl _ZN7PetHand3RubEi
_021562f0:
    str r4, [r6, #0x1c]
    str r5, [r6, #0x20]
    ldmia sp!, {r4, r5, r6, pc}
    dcd gInput+0xc
    dcd gInput+0x8
    dcd 0x45800000
}
#endif

void PetHand::Rub(int dist)
{
    if (dist <= 1) {
        return;
    }
    gMiniGame->AddCount(dist);
    if (gMiniGame->mDone) {
        gMiniGame->Func2();
    }
    mTimer = 0;
}

void PetHand::Tick()
{
    mTimer++;
    if (mTimer < 90) {
        mAnimal->SetMood(0);
    } else if (mTimer == 90) {
        mAnimal->SetMood(1);
    } else if (mTimer == 180) {
        mAnimal->SetMood(2);
    } else if (mTimer > 180 && mTimer % 30 == 0) {
        gMiniGame->AddCount(-400);
    }
}

void PetHand::Func63F0()
{
}
