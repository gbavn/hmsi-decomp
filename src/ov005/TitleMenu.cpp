// Overlay 5, part 5: the bottom-screen title menu and the 3D stage it shows.
#include "ov005_title.h"
#include "ov005_menudata.h"


TitleMenu::TitleMenu(TitleScene *scene)
{
    Archive *arc;
    u16 i;
    int j;

    mScene = scene;
    unk4 = 0;
    arc = mScene->GetArchive();
    {
        TitleMenuNames names = gTitleMenuRodata.button;
        for (i = 0; i < 7; i++) {
            mSprites[i] = func_0202b41c(arc->GetFile(names.ncbr), arc->GetFile(names.nclr), arc->GetFile(names.ncer),
                                        arc->GetFile(names.nanr), names.ncbr, names.nclr, names.ncer, names.nanr,
                                        0x80, 0x60);
            func_0202b0ec(mSprites[i], gTitleMenuPriorities[i]);
            func_0202b064(mSprites[i], i);
        }
    }
    {
        TitleMenuNames names = gTitleMenuRodata.choice;
        mCursorSprite = func_0202b41c(arc->GetFile(names.ncbr), arc->GetFile(names.nclr), arc->GetFile(names.ncer),
                                      arc->GetFile(names.nanr), names.ncbr, names.nclr, names.ncer, names.nanr,
                                      0x80, 0x60);
        func_0202b0ec(mCursorSprite, 0x19);
        func_0202b064(mCursorSprite, 0);
    }
    mMark = func_0202b824(gTitleMenuStrings.mentalNcbr, gTitleMenuStrings.mentalNclr, gTitleMenuStrings.mentalNcer,
                          gTitleMenuStrings.mentalNanr, 0, 0, 0);
    mButtons = TitleButton::Create(this);
    mCamera = new (sTitleManagers[5]) TitleCamera;
    mStage = new (sTitleManagers[4]) TitleStage;
    {
        VecFx32 pos = gTitleMenuRodata.shipStart;
        mStage->SetShipPos(&pos);
    }
    {
        TitleChar *a = new TitleChar(0xB2, *mMark);
        TitleChar *b = new TitleChar(0xAA, *mMark);
        if (func_020358d0(100) & 1) {
            mShipChar = a;
            mShoreChar = b;
        } else {
            mShipChar = b;
            mShoreChar = a;
        }
    }
    for (j = 0; j < 47; j++) {
        mChars[j] = NULL;
        mCharFlags[j] = 0;
    }
    PlaceChars();
    mShown = 0;
    mLeaving = 0;
    if (HasSaveData()) {
        mCursor = 1;
    } else {
        mCursor = 0;
    }
    mButtons[mCursor].SetTouched(1);
}

TitleMenu::~TitleMenu()
{
    int i;

    delete[] mButtons;
    if (mCamera != NULL) {
        delete mCamera;
    }
    if (mStage != NULL) {
        delete mStage;
    }
    func_0202ba04(mMark, 0);
    for (i = 0; i < 7; i++) {
        func_0202ba04(mSprites[i], 1);
    }
    func_0202ba04(mCursorSprite, 0);
    if (mShipChar != NULL) {
        delete mShipChar;
    }
    if (mShoreChar != NULL) {
        delete mShoreChar;
    }
    for (i = 0; i < 47; i++) {
        if (mChars[i] != NULL) {
            delete mChars[i];
        }
    }
}

// TitleCamera's deleting destructor: emitted here in the original (an inline destructor instantiated by
// the delete above), so it is kept as asm under its linker name.
extern "C" {
void _ZN4TaskD2Ev(void);
void _ZN4TaskdlEPv(void);
}
extern "C" asm void _ZN11TitleCameraD0Ev(void) {
    stmdb sp!, {r4, lr}
    mov r4, r0
    bl _ZN4TaskD2Ev
    mov r0, r4
    bl _ZN4TaskdlEPv
    mov r0, r4
    ldmia sp!, {r4, pc}
}

#ifdef NONMATCHING
BOOL TitleMenu::Animate()
{
    int i;

    if (!func_020374f8(1) && !func_020374f8(0)) {
        if (mShown) {
            mShown = 0;
        }
        if (mLeaving) {
            if (func_020358d0(100) & 1) {
                TitleChar *tmp = mShipChar;
                mShipChar = mShoreChar;
                mShoreChar = tmp;
            }
            mStage->SetShipPos(0, 0, mStage->GetShipPos()->z);
            mCamera->SetFocus(mStage->GetShipPos()->x, 0);
            mCamera->Update();
            mScene->ShowMenu();
            mLeaving = 0;
            mShown = 1;
            func_02037284(2, -2, 1);
            func_02037284(2, -2, 0);
        }
    }
    if (mScene->IsIdle() && mStage->GetShipPos()->x <= -0x21C000 && !mLeaving && !mShown) {
        func_02037284(7, -2, 1);
        func_02037284(7, -2, 0);
        mLeaving = 1;
    }
    {
        VecFx32 move = gTitleMenuRodata2.shipMove;
        mStage->MoveShip(&move);
    }
    mShipChar->mX = mStage->GetShipPos()->x + 0x2000;
    mShipChar->mY = mStage->GetShipPos()->y;
    mShipChar->mZ = mStage->GetShipPos()->z + 0x4000;
    func_0202b064(mShipChar->mSprite, 2);
    mShipChar->Update();
    mShoreChar->mX = -0x41333;
    mShoreChar->mY = 0;
    mShoreChar->mZ = -0x7000;
    func_0202b064(mShoreChar->mSprite, 2);
    mShoreChar->Update();
    if (mStage->GetShipPos()->x < 0) {
        mCamera->SetFocus(mStage->GetShipPos()->x, 0);
    }
    for (i = 0; i < 47; i++) {
        if (mChars[i] != NULL) {
            mChars[i]->Update();
        }
    }
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN10TitleScene6IsIdleEv(void);
void _ZN10TitleScene8ShowMenuEv(void);
void _ZN10TitleStage10SetShipPosElll(void);
void _ZN10TitleStage8MoveShipEPK7VecFx32(void);
void _ZN11TitleCamera8SetFocusEll(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL TitleMenu::Animate() {
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #0xc
    mov r4, r0
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _021579b0
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _021579b0
    ldrb r0, [r4, #0x36c]
    cmp r0, #0x0
    movne r0, #0x0
    strneb r0, [r4, #0x36c]
    ldrb r0, [r4, #0x36d]
    cmp r0, #0x0
    beq _021579b0
    mov r0, #0x64
    bl func_020358d0
    tst r0, #0x1
    beq _0215790c
    ldr r1, [r4, #0x364]
    ldr r0, [r4, #0x368]
    str r0, [r4, #0x364]
    str r1, [r4, #0x368]
_0215790c:
    ldr r0, [r4, #0x38]
    mvn r1, #0x0
    ldr r2, [r0, #0x258]
    cmp r2, r1
    ldreq r1, [r0, #0x58]
    addne r1, r0, r2, lsl #0x2
    ldrne r1, [r1, #0x58]
    add r2, r1, #0x74
    mov r1, #0x0
    ldr r3, [r2, #0x8]
    mov r2, r1
    bl _ZN10TitleStage10SetShipPosElll
    ldr r2, [r4, #0x38]
    mvn r0, #0x0
    ldr r1, [r2, #0x258]
    cmp r1, r0
    ldreq r0, [r2, #0x58]
    addne r0, r2, r1, lsl #0x2
    ldrne r0, [r0, #0x58]
    mov r2, #0x0
    add r1, r0, #0x74
    ldr r0, [r4, #0x3c]
    ldr r1, [r1, #0x0]
    bl _ZN11TitleCamera8SetFocusEll
    ldr r0, [r4, #0x3c]
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x8]
    blx r1
    ldr r0, [r4, #0x8]
    bl _ZN10TitleScene8ShowMenuEv
    mov r1, #0x0
    mov r0, #0x2
    strb r1, [r4, #0x36d]
    mov r2, #0x1
    sub r1, r0, #0x4
    strb r2, [r4, #0x36c]
    bl func_02037284
    mov r0, #0x2
    sub r1, r0, #0x4
    mov r2, #0x0
    bl func_02037284
_021579b0:
    ldr r0, [r4, #0x8]
    bl _ZN10TitleScene6IsIdleEv
    cmp r0, #0x0
    beq _02157a30
    ldr r2, [r4, #0x38]
    mvn r0, #0x0
    ldr r1, [r2, #0x258]
    cmp r1, r0
    ldreq r0, [r2, #0x58]
    addne r0, r2, r1, lsl #0x2
    ldrne r0, [r0, #0x58]
    add r1, r0, #0x74
    mov r0, #0x21c000
    ldr r1, [r1, #0x0]
    rsb r0, r0, #0x0
    cmp r1, r0
    bgt _02157a30
    ldrb r0, [r4, #0x36d]
    cmp r0, #0x0
    ldreqb r0, [r4, #0x36c]
    cmpeq r0, #0x0
    bne _02157a30
    mov r0, #0x7
    sub r1, r0, #0x9
    mov r2, #0x1
    bl func_02037284
    mov r0, #0x7
    sub r1, r0, #0x9
    mov r2, #0x0
    bl func_02037284
    mov r0, #0x1
    strb r0, [r4, #0x36d]
_02157a30:
    ldr r0, [pc, #0x16c]
    add r3, sp, #0x0
    ldmia r0, {r0, r1, r2}
    stmia r3, {r0, r1, r2}
    ldr r0, [r4, #0x38]
    mov r1, r3
    bl _ZN10TitleStage8MoveShipEPK7VecFx32
    ldr r1, [r4, #0x38]
    mvn r0, #0x0
    ldr r2, [r1, #0x258]
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    add r3, r0, #0x74
    mvn r0, #0x0
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    add r5, r0, #0x74
    mvn r0, #0x0
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    ldr r2, [r5, #0x4]
    add r1, r0, #0x74
    ldr r0, [r3, #0x0]
    ldr r3, [r1, #0x8]
    ldr r1, [r4, #0x364]
    add r0, r0, #0x2000
    str r0, [r1, #0x2c]
    str r2, [r1, #0x30]
    add r0, r3, #0x4000
    str r0, [r1, #0x34]
    ldr r0, [r4, #0x364]
    mov r1, #0x2
    ldr r0, [r0, #0x14]
    bl func_0202b064
    ldr r0, [r4, #0x364]
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x8]
    blx r1
    mov r1, #0x0
    ldr r3, [r4, #0x368]
    ldr r2, [pc, #0xb8]
    sub r0, r1, #0x7000
    str r2, [r3, #0x2c]
    str r1, [r3, #0x30]
    str r0, [r3, #0x34]
    ldr r0, [r4, #0x368]
    ldr r0, [r0, #0x14]
    bl func_0202b064
    ldr r0, [r4, #0x368]
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x8]
    blx r1
    ldr r1, [r4, #0x38]
    mvn r0, #0x0
    ldr r2, [r1, #0x258]
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    add r0, r0, #0x74
    ldr r0, [r0, #0x0]
    cmp r0, #0x0
    bge _02157b6c
    mvn r0, #0x0
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    mov r2, #0x0
    add r1, r0, #0x74
    ldr r0, [r4, #0x3c]
    ldr r1, [r1, #0x0]
    bl _ZN11TitleCamera8SetFocusEll
_02157b6c:
    mov r5, #0x0
_02157b70:
    add r0, r4, r5, lsl #0x2
    ldr r0, [r0, #0x40]
    cmp r0, #0x0
    beq _02157b8c
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x8]
    blx r1
_02157b8c:
    add r5, r5, #0x1
    cmp r5, #0x2f
    blt _02157b70
    mov r0, #0x1
    add sp, sp, #0xc
    ldmia sp!, {r4, r5, pc}
    dcd gTitleMenuRodata2
    dcd 0xfffbeccd
}
#endif

void TitleMenu::Draw()
{
}

void TitleMenu::DrawBlend()
{
}

void TitleMenu::ResetButtons()
{
    int i;

    for (i = 0; i < 2; i++) {
        mButtons[i].ClearPressed();
    }
}

#ifdef NONMATCHING
void TitleMenu::UpdateButtons(BOOL active)
{
    BOOL decided = FALSE;
    int i;

    for (i = 0; i < 2; i++) {
        if (mButtons[i].IsTouched()) {
            mCursor = i;
        }
        if (mButtons[i].IsDecided()) {
            decided = TRUE;
        }
    }
    if (active && gInput.touchFlags == 0 && !decided) {
        if (gInput.trg & PAD_KEY_UP) {
            mButtons[mCursor].SetTouched(0);
            mCursor--;
            if (!HasSaveData()) {
                mCursor = 0;
            }
            if (mCursor < 0) {
                mCursor = 0;
            }
            mButtons[mCursor].SetTouched(1);
            func_02035e8c(0x6B, 2);
        }
        if (gInput.trg & PAD_KEY_DOWN) {
            mButtons[mCursor].SetTouched(0);
            mCursor++;
            if (!HasSaveData()) {
                mCursor = 0;
            }
            if (mCursor > 1) {
                mCursor = 1;
            }
            mButtons[mCursor].SetTouched(1);
            func_02035e8c(0x6B, 2);
        }
    }
    if (active && !decided && gInput.trg & PAD_BUTTON_A) {
        mButtons[mCursor].Decide();
    }
    for (i = 0; i < 2; i++) {
        mButtons[i].Draw(active);
    }
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN11TitleButton10SetTouchedEh(void);
void _ZN11TitleButton4DrawEi(void);
void _ZN11TitleButton6DecideEv(void);
void _ZN11TitleButton9IsDecidedEv(void);
void _ZN11TitleButton9IsTouchedEv(void);
void _ZN9TitleMenu11HasSaveDataEv(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void TitleMenu::UpdateButtons(BOOL active) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    mov r4, #0x0
    mov r8, r0
    mov r7, r1
    mov r5, r4
    mov r6, r4
    mov r9, #0x1
_02157c00:
    ldr r0, [r8, #0xc]
    add r0, r0, r6
    bl _ZN11TitleButton9IsTouchedEv
    cmp r0, #0x0
    strne r5, [r8, #0x10]
    ldr r0, [r8, #0xc]
    add r0, r0, r6
    bl _ZN11TitleButton9IsDecidedEv
    cmp r0, #0x0
    add r5, r5, #0x1
    movne r4, r9
    cmp r5, #0x2
    add r6, r6, #0x2c
    blt _02157c00
    cmp r7, #0x0
    beq _02157d4c
    ldr r0, [pc, #0x168]
    ldrb r0, [r0, #0x0]
    cmp r0, #0x0
    cmpeq r4, #0x0
    bne _02157d4c
    ldr r0, [pc, #0x158]
    ldrh r0, [r0, #0x0]
    tst r0, #0x40
    beq _02157cd0
    ldr r2, [r8, #0xc]
    ldr r1, [r8, #0x10]
    mov r0, #0x2c
    mla r0, r1, r0, r2
    mov r1, #0x0
    bl _ZN11TitleButton10SetTouchedEh
    ldr r1, [r8, #0x10]
    mov r0, r8
    sub r1, r1, #0x1
    str r1, [r8, #0x10]
    bl _ZN9TitleMenu11HasSaveDataEv
    cmp r0, #0x0
    moveq r0, #0x0
    streq r0, [r8, #0x10]
    ldr r0, [r8, #0x10]
    cmp r0, #0x0
    movlt r0, #0x0
    strlt r0, [r8, #0x10]
    ldr r2, [r8, #0xc]
    ldr r1, [r8, #0x10]
    mov r0, #0x2c
    mla r0, r1, r0, r2
    mov r1, #0x1
    bl _ZN11TitleButton10SetTouchedEh
    mov r0, #0x6b
    mov r1, #0x2
    bl func_02035e8c
_02157cd0:
    ldr r0, [pc, #0xdc]
    ldrh r0, [r0, #0x0]
    tst r0, #0x80
    beq _02157d4c
    ldr r2, [r8, #0xc]
    ldr r1, [r8, #0x10]
    mov r0, #0x2c
    mla r0, r1, r0, r2
    mov r1, #0x0
    bl _ZN11TitleButton10SetTouchedEh
    ldr r1, [r8, #0x10]
    mov r0, r8
    add r1, r1, #0x1
    str r1, [r8, #0x10]
    bl _ZN9TitleMenu11HasSaveDataEv
    cmp r0, #0x0
    moveq r0, #0x0
    streq r0, [r8, #0x10]
    ldr r0, [r8, #0x10]
    cmp r0, #0x1
    movgt r0, #0x1
    strgt r0, [r8, #0x10]
    ldr r2, [r8, #0xc]
    ldr r1, [r8, #0x10]
    mov r0, #0x2c
    mla r0, r1, r0, r2
    mov r1, #0x1
    bl _ZN11TitleButton10SetTouchedEh
    mov r0, #0x6b
    mov r1, #0x2
    bl func_02035e8c
_02157d4c:
    cmp r7, #0x0
    beq _02157d84
    cmp r4, #0x0
    bne _02157d84
    ldr r0, [pc, #0x50]
    ldrh r0, [r0, #0x0]
    tst r0, #0x1
    beq _02157d84
    ldr r2, [r8, #0xc]
    ldr r1, [r8, #0x10]
    mov r0, #0x2c
    mla r0, r1, r0, r2
    mov r1, #0x1
    bl _ZN11TitleButton6DecideEv
_02157d84:
    mov r4, #0x0
    mov r5, r4
_02157d8c:
    ldr r0, [r8, #0xc]
    mov r1, r7
    add r0, r0, r5
    bl _ZN11TitleButton4DrawEi
    add r4, r4, #0x1
    cmp r4, #0x2
    add r5, r5, #0x2c
    blt _02157d8c
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    dcd gInput
    dcd gInput+0x6
}
#endif

void TitleMenu::BlinkPrompt()
{
    mBlink++;
    if (mBlink < 30) {
        func_0202bc20(data_020ec87c, mSprites[0]);
    }
    if (mBlink > 60) {
        mBlink = 0;
    }
}

#ifdef NONMATCHING
void TitleMenu::PlaceChar(int index, CharPlace place)
{
    TitleChar *chr;

    if (IsPerson(index)) {
        int id = GetPersonId(index);
        if (id == 0xFF) {
            return;
        }
        mChars[index] = new TitleChar(id, *mMark);
    } else {
        int id = GetAnimalId(index);
        if (id == -1) {
            return;
        }
        mChars[index] = new TitleChar((u32)id, *mMark);
        if (id == 20) {
            mChars[index]->SetAnim(5, 0);
        }
    }
    chr = mChars[index];
    chr->mX = FX_F32_TO_FX32(place.x);
    chr->mY = 0;
    chr->mZ = FX_F32_TO_FX32(place.z - 16.0f);
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN9TitleCharC1Ei8Sprite2D(void);
void _ZN9TitleCharC1Em8Sprite2D(void);
void _ZN9TitleMenu11GetAnimalIdEi(void);
void _ZN9TitleMenu11GetPersonIdEi(void);
void _ZN9TitleMenu8IsPersonEi(void);
void _ZN9XbbSprite7SetAnimEii(void);
void _Znwm(void);
void _fadd(void);
void _ffix(void);
void _fgr(void);
void _fmul(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void TitleMenu::PlaceChar(int index, CharPlace place) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #0x174
    mov r6, r0
    mov r5, r1
    bl _ZN9TitleMenu8IsPersonEi
    cmp r0, #0x0
    mov r0, r6
    mov r1, r5
    beq _02157e90
    bl _ZN9TitleMenu11GetPersonIdEi
    mov r9, r0
    cmp r9, #0xff
    addeq sp, sp, #0x174
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, pc}
    mov r0, #0x44
    bl _Znwm
    movs lr, r0
    beq _02157e84
    sub ip, sp, #0x8
    ldr r8, [r6, #0x34]
    mov r7, ip
    mov r4, #0x17
_02157e58:
    ldmia r8!, {r0, r1, r2, r3}
    stmia r7!, {r0, r1, r2, r3}
    subs r4, r4, #0x1
    bne _02157e58
    ldmia r8, {r0, r1, r2}
    stmia r7, {r0, r1, r2}
    mov r0, lr
    mov r1, r9
    ldmia ip, {r2, r3}
    bl _ZN9TitleCharC1Ei8Sprite2D
    mov lr, r0
_02157e84:
    add r0, r6, r5, lsl #0x2
    str lr, [r0, #0x40]
    b _02157f14
_02157e90:
    bl _ZN9TitleMenu11GetAnimalIdEi
    mov r4, r0
    mvn r0, #0x0
    cmp r4, r0
    addeq sp, sp, #0x174
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, pc}
    mov r0, #0x44
    bl _Znwm
    movs r9, r0
    beq _02157ef4
    sub r8, sp, #0x8
    ldr r7, [r6, #0x34]
    mov lr, r8
    mov ip, #0x17
_02157ec8:
    ldmia r7!, {r0, r1, r2, r3}
    stmia lr!, {r0, r1, r2, r3}
    subs ip, ip, #0x1
    bne _02157ec8
    ldmia r7, {r0, r1, r2}
    stmia lr, {r0, r1, r2}
    mov r0, r9
    mov r1, r4
    ldmia r8, {r2, r3}
    bl _ZN9TitleCharC1Em8Sprite2D
    mov r9, r0
_02157ef4:
    add r0, r6, r5, lsl #0x2
    str r9, [r0, #0x40]
    cmp r4, #0x14
    bne _02157f14
    ldr r0, [r0, #0x40]
    mov r1, #0x5
    mov r2, #0x0
    bl _ZN9XbbSprite7SetAnimEii
_02157f14:
    ldr r0, [sp, #0x190]
    add r2, r6, r5, lsl #0x2
    mov r1, #0x0
    ldr r4, [r2, #0x40]
    bl _fgr
    ldr r0, [pc, #0xa4]
    bls _02157f48
    ldr r1, [sp, #0x190]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02157f58
_02157f48:
    ldr r1, [sp, #0x190]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02157f58:
    bl _ffix
    str r0, [r4, #0x2c]
    mov r2, #0x0
    ldr r0, [sp, #0x194]
    ldr r1, [pc, #0x68]
    str r2, [r4, #0x30]
    bl _fsub
    mov r1, #0x0
    bl _fgr
    ldr r1, [pc, #0x54]
    bls _02157fa8
    ldr r0, [sp, #0x194]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x3c]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02157fc4
_02157fa8:
    ldr r0, [sp, #0x194]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x18]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02157fc4:
    bl _ffix
    str r0, [r4, #0x34]
    add sp, sp, #0x174
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
    dcd 0x45800000
    dcd 0x41800000
}
#endif

void TitleMenu::DebugPlace(int index, f32 value)
{
}

#ifdef NONMATCHING
void TitleMenu::PlaceChars()
{
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN9TitleMenu10DebugPlaceEif(void);
void _ZN9TitleMenu9PlaceCharEi9CharPlace(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void TitleMenu::PlaceChars() {
    stmdb sp!, {r4, lr}
    sub sp, sp, #0x10
    ldr r1, [pc, #0x934]
    mov ip, #0x0
    stmia sp, {r1, ip}
    str ip, [sp, #0x8]
    sub r2, r1, #0x1200000
    add r3, r1, #0x780000
    mov r1, #0x6
    mov r4, r0
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x910]
    mov ip, #0x0
    str r1, [sp, #0x0]
    rsb r0, r1, #0x2a40000
    stmib sp, {r0, ip}
    add r3, r1, #0x640000
    mov r0, r4
    sub r2, ip, #0x3ec00000
    mov r1, #0x7
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x8e8]
    ldr r0, [pc, #0x8e8]
    mov ip, #0x0
    stmia sp, {r0, r1, ip}
    mov r0, r4
    rsb r2, r1, #0x2100000
    rsb r3, r1, #0x3880000
    mov r1, #0x5
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x8c0]
    ldr r1, [pc, #0x8c4]
    mov r0, #0x0
    stmia sp, {r1, r3}
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    rsb r2, r3, #0x2300000
    mov r0, r4
    mov r1, #0x4
    rsb r3, r3, #0x3880000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x8a0]
    mov r0, #0x0
    str r2, [sp, #0x0]
    rsb r1, r2, #0x2d40000
    str r1, [sp, #0x4]
    str r0, [sp, #0x8]
    add r3, r2, #0x340000
    str r0, [sp, #0xc]
    sub r2, r0, #0x3e800000
    mov r0, r4
    mov r1, #0x2
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x874]
    ldr r2, [pc, #0x874]
    str r1, [sp, #0x0]
    mov r0, #0x0
    str r0, [sp, #0x4]
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x3
    add r3, r2, #0x1400000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x850]
    mov r0, r4
    mov r1, #0x6
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x844]
    mov r0, r4
    mov r1, #0x7
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x838]
    mov r0, r4
    mov r1, #0x5
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x82c]
    mov r0, r4
    mov r1, #0x1
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x820]
    mov r0, r4
    mov r1, #0x4
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x814]
    mov r0, r4
    mov r1, #0x2
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x808]
    mov r0, r4
    mov r1, #0x3
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r3, #0xc0000000
    ldr r0, [pc, #0x7f8]
    mov r1, #0xb
    stmia sp, {r0, r3}
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    add r2, r3, #0x2480000
    mov r0, r4
    add r3, r3, #0x3200000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x7d0]
    ldr r0, [pc, #0x7d0]
    mov r1, #0x13
    stmia sp, {r0, r3}
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    rsb r2, r3, #0x32c0000
    mov r0, r4
    rsb r3, r3, #0x4000000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r3, #0xc0000000
    ldr r0, [pc, #0x7a4]
    mov r1, #0xf
    stmia sp, {r0, r3}
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    add r2, r3, #0x2500000
    mov r0, r4
    add r3, r3, #0x3200000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r3, #0xc0000000
    ldr r0, [pc, #0x778]
    mov r1, #0xd
    stmia sp, {r0, r3}
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    add r2, r3, #0x2540000
    mov r0, r4
    add r3, r3, #0x3200000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x750]
    ldr r0, [pc, #0x750]
    mov r1, #0x8
    stmia sp, {r0, r3}
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    rsb r2, r3, #0x3680000
    mov r0, r4
    rsb r3, r3, #0x4300000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r0, [pc, #0x728]
    mov r3, #0x40000000
    stmia sp, {r0, r3}
    mov r1, #0x0
    str r1, [sp, #0x8]
    mov ip, #0x1
    rsb r2, r3, #0x25c0000
    mov r0, r4
    rsb r3, r3, #0x3200000
    mov r1, #0xc
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x6fc]
    ldr r0, [pc, #0x6fc]
    mov r1, #0x0
    stmia sp, {r0, r3}
    str r1, [sp, #0x8]
    mov ip, #0x2
    rsb r2, r3, #0x3900000
    mov r0, r4
    rsb r3, r3, #0x4500000
    mov r1, #0x9
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r3, #0x40000000
    ldr r0, [pc, #0x6d0]
    mov r1, #0x0
    stmia sp, {r0, r3}
    str r1, [sp, #0x8]
    mov ip, #0x1
    rsb r2, r3, #0x2640000
    mov r0, r4
    rsb r3, r3, #0x3200000
    mov r1, #0x12
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x69c]
    ldr r1, [pc, #0x6a4]
    mov r0, #0x0
    stmia sp, {r1, r3}
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    rsb r2, r3, #0x3980000
    mov r0, r4
    mov r1, #0x10
    rsb r3, r3, #0x4500000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x67c]
    mov r0, r4
    mov r1, #0xb
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x670]
    mov r0, r4
    mov r1, #0x13
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x664]
    mov r0, r4
    mov r1, #0xf
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x658]
    mov r0, r4
    mov r1, #0xd
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x64c]
    mov r0, r4
    mov r1, #0x8
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x640]
    mov r0, r4
    mov r1, #0xc
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x634]
    mov r0, r4
    mov r1, #0x9
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x628]
    mov r0, r4
    mov r1, #0x12
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x61c]
    mov r0, r4
    mov r1, #0x10
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r3, #0xc0000000
    ldr r0, [pc, #0x5fc]
    mov r1, #0x0
    stmia sp, {r0, r3}
    str r1, [sp, #0x8]
    mov ip, #0x2
    add r2, r3, #0x2dc0000
    mov r0, r4
    add r3, r3, #0x35c0000
    mov r1, #0xe
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r3, #0xc0000000
    ldr r1, [pc, #0x5dc]
    mov r0, #0x0
    stmia sp, {r1, r3}
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    add r2, r3, #0x2e00000
    mov r0, r4
    mov r1, #0xa
    add r3, r3, #0x35c0000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x5ac]
    ldr r0, [pc, #0x56c]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    sub r2, r3, #0x4a0000
    mov r0, r4
    mov r1, #0x17
    add r3, r3, #0x340000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x580]
    ldr r0, [pc, #0x538]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    sub r2, r3, #0x4c0000
    mov r0, r4
    mov r1, #0x16
    add r3, r3, #0x2e0000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x550]
    mov r0, #0x0
    str r3, [sp, #0x0]
    rsb r1, r3, #0x3d40000
    str r1, [sp, #0x4]
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    sub r2, r3, #0x500000
    mov r0, r4
    mov r1, #0x14
    add r3, r3, #0x280000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x520]
    mov r0, #0xc0000000
    str r1, [sp, #0x0]
    sub r2, r1, #0x540000
    str r0, [sp, #0x4]
    add r3, r0, #0x35c0000
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x15
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r0, #0x3cc00000
    mov r3, #0xc0000000
    rsb r0, r0, #0x0
    stmia sp, {r0, r3}
    mov r1, #0x0
    str r1, [sp, #0x8]
    mov ip, #0x2
    add r2, r3, #0x2e80000
    mov r0, r4
    add r3, r3, #0x35c0000
    mov r1, #0x11
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x4bc]
    mov r0, r4
    mov r1, #0xe
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x4ac]
    mov r0, r4
    mov r1, #0xa
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x49c]
    mov r0, r4
    mov r1, #0x17
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x48c]
    mov r0, r4
    mov r1, #0xf
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x47c]
    mov r0, r4
    mov r1, #0x16
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x46c]
    mov r0, r4
    mov r1, #0x14
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x45c]
    mov r0, r4
    mov r1, #0x15
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x44c]
    mov r0, r4
    mov r1, #0x11
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r3, [pc, #0x440]
    ldr r0, [pc, #0x3e8]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    sub r2, r3, #0x250000
    mov r0, r4
    mov r1, #0x1b
    add r3, r3, #0x290000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x410]
    ldr r0, [pc, #0x3b4]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    sub r2, r3, #0x2a0000
    mov r0, r4
    mov r1, #0x1c
    add r3, r3, #0x230000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x3e0]
    mov r0, r4
    mov r1, #0x1b
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x3d0]
    mov r0, r4
    mov r1, #0x1c
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r0, [pc, #0x3c4]
    ldr r2, [pc, #0x3c4]
    str r0, [sp, #0x0]
    mov r0, #0x40000000
    str r0, [sp, #0x4]
    mov r1, #0x0
    mov r0, r4
    str r1, [sp, #0x8]
    mov ip, #0x1
    add r3, r2, #0x5a0000
    mov r1, #0x1a
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x398]
    ldr r2, [pc, #0x398]
    ldr r0, [pc, #0x398]
    str r1, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r1, #0x0
    mov r0, r4
    str r1, [sp, #0x8]
    mov ip, #0x2
    add r3, r2, #0x5c0000
    mov r1, #0x18
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x370]
    mov r0, #0x0
    str r1, [sp, #0x0]
    str r0, [sp, #0x4]
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    ldr r2, [pc, #0x35c]
    mov r0, r4
    mov r1, #0x19
    add r3, r2, #0x5b0000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r0, r4
    mov r1, #0x1a
    ldr r2, [pc, #0x344]
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r0, r4
    mov r1, #0x18
    ldr r2, [pc, #0x334]
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r0, r4
    mov r1, #0x19
    ldr r2, [pc, #0x324]
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r1, [pc, #0x320]
    ldr r0, [pc, #0x320]
    str r1, [sp, #0x0]
    add r3, r1, #0x160000
    str r0, [sp, #0x4]
    rsb r2, r0, #0x3dc0000
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x2
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x1e
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x2f4]
    ldr r0, [pc, #0x2ec]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x1f
    sub r2, r3, #0x148000
    add r3, r3, #0x120000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x2c4]
    mov r0, r4
    mov r1, #0x1e
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x2b4]
    mov r0, r4
    mov r1, #0x1f
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r1, [pc, #0x2a8]
    ldr r2, [pc, #0x2a8]
    ldr r0, [pc, #0x2a8]
    str r1, [sp, #0x0]
    str r0, [sp, #0x4]
    mov ip, #0x0
    str ip, [sp, #0x8]
    mov r0, r4
    add r3, r2, #0x288000
    mov r1, #0x1d
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x284]
    ldr r0, [pc, #0x27c]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x20
    sub r2, r3, #0xfc000
    add r3, r3, #0x184000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x254]
    mov r0, #0xc0000000
    str r1, [sp, #0x0]
    sub r2, r1, #0xdc000
    str r0, [sp, #0x4]
    add r3, r0, #0x3dc0000
    mov r0, #0x0
    str r0, [sp, #0x8]
    mov r0, #0x1
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x21
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r0, r4
    mov r1, #0x1d
    ldr r2, [pc, #0x21c]
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r0, r4
    mov r1, #0x20
    ldr r2, [pc, #0x20c]
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r0, r4
    mov r1, #0x21
    ldr r2, [pc, #0x1fc]
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r1, [pc, #0x1f8]
    mov r0, #0x0
    str r1, [sp, #0x0]
    str r0, [sp, #0x4]
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x26
    ldr r2, [pc, #0x1dc]
    add r3, r2, #0x2b8000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r3, [pc, #0x1d4]
    ldr r0, [pc, #0x1a4]
    str r3, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x24
    sub r2, r3, #0x170000
    add r3, r3, #0x138000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r1, [pc, #0x1a8]
    mov r0, #0x3fc00000
    ldr r2, [pc, #0x1a4]
    str r1, [sp, #0x0]
    rsb r0, r0, #0x0
    str r0, [sp, #0x4]
    mov ip, #0x0
    str ip, [sp, #0x8]
    mov r0, r4
    add r3, r2, #0x298000
    mov r1, #0x25
    str ip, [sp, #0xc]
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    ldr r2, [pc, #0x17c]
    mov r0, r4
    mov r1, #0x22
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r2, [pc, #0x16c]
    mov r0, r4
    mov r1, #0x24
    bl _ZN9TitleMenu10DebugPlaceEif
    mov r0, r4
    mov r1, #0x26
    ldr r2, [pc, #0x154]
    bl _ZN9TitleMenu10DebugPlaceEif
    ldr r1, [pc, #0x150]
    mov r0, #0x41000000
    str r1, [sp, #0x0]
    str r0, [sp, #0x4]
    mov r0, #0x0
    str r0, [sp, #0x8]
    str r0, [sp, #0xc]
    mov r0, r4
    mov r1, #0x28
    ldr r2, [pc, #0x130]
    add r3, r2, #0x1d0000
    bl _ZN9TitleMenu9PlaceCharEi9CharPlace
    mov r0, r4
    mov r1, #0x28
    ldr r2, [pc, #0x120]
    bl _ZN9TitleMenu10DebugPlaceEif
    add sp, sp, #0x10
    ldmia sp!, {r4, pc}
    dcd 0xc2500000
    dcd 0xc2640000
    dcd 0x40c00000
    dcd 0xc2733333
    dcd 0xc28b3333
    dcd 0xc2940000
    dcd 0xc29ccccd
    dcd 0xc1880000
    dcd 0xc2ca0000
    dcd 0xc2cc0000
    dcd 0xc2ce0000
    dcd 0xc2d00000
    dcd 0xc2d20000
    dcd 0xc2d40000
    dcd 0xc2d60000
    dcd 0xc2da0000
    dcd 0x40e00000
    dcd 0xc2ea0000
    dcd 0xc2e40000
    dcd 0xc2f00000
    dcd 0x41100000
    dcd 0xc2f60000
    dcd 0xc2ff0000
    dcd 0x41300000
    dcd 0xc3020000
    dcd 0xc3048000
    dcd 0xc3070000
    dcd 0xc3200000
    dcd 0xc3210000
    dcd 0xc3220000
    dcd 0xc3230000
    dcd 0xc3240000
    dcd 0xc3250000
    dcd 0xc3260000
    dcd 0xc3270000
    dcd 0xc3280000
    dcd 0xc32b0000
    dcd 0xc32e0000
    dcd 0xc3340000
    dcd 0xc33a0000
    dcd 0xc35c0000
    dcd 0xc3630000
    dcd 0xc3690000
    dcd 0xc38c0000
    dcd 0xc38d8ccd
    dcd 0xc34b0000
    dcd 0xc3908ccd
    dcd 0xc3490000
    dcd 0x40a00000
    dcd 0xc3960ccd
    dcd 0xc34a0000
    dcd 0xc3a50000
    dcd 0xc3ad0000
    dcd 0x40400000
    dcd 0xc3b10000
    dcd 0xc3c30000
    dcd 0xc3cc199a
    dcd 0xc3b38000
    dcd 0x402ccccd
    dcd 0xc3c3c000
    dcd 0xc3c6c000
    dcd 0xc3dc0000
    dcd 0xc3e3199a
    dcd 0xc3cd8000
    dcd 0xc3e58000
    dcd 0xc3e7e666
    dcd 0xc3cf8000
    dcd 0xc3f90000
    dcd 0xc400eccd
    dcd 0xc3ed0000
    dcd 0xc40a0000
}
#endif

BOOL TitleMenu::IsPerson(int index)
{
    switch (index) {
    case 0:
    case 1:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
        return TRUE;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 25:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
        return FALSE;
    }
    return FALSE;
}

int TitleMenu::GetPersonId(int index)
{
    TitleMenuIds ids = gTitleMenuRodata2.personIds;
    return ids.ids[index];
}

int TitleMenu::GetAnimalId(int index)
{
    TitleMenuIds ids = gTitleMenuRodata2.animalIds;
    return ids.ids[index];
}

u8 TitleMenu::HasSaveData()
{
    return mScene->HasSaveData();
}

TitleScene *TitleMenu::GetScene()
{
    return mScene;
}

Sprite2D *TitleMenu::GetButtonSprite(u8 index, int part)
{
    Sprite2D *sprite = NULL;

    switch (index) {
    case 0:
        sprite = part == 0 ? mSprites[1] : mSprites[4];
        break;
    case 1:
        sprite = part == 0 ? mSprites[2] : mSprites[5];
        break;
    }
    return sprite;
}

Sprite2D *TitleMenu::GetCursorSprite()
{
    return mCursorSprite;
}

#ifdef NONMATCHING
void TitleMenu::Show()
{
    mStage->SetShipPos(-0x2E000, mStage->GetShipPos()->y, mStage->GetShipPos()->z);
    mShipChar->mX = mStage->GetShipPos()->x + 0x2000;
    mShipChar->mY = mStage->GetShipPos()->y;
    mShipChar->mZ = mStage->GetShipPos()->z + 0x4000;
    func_0202b064(mShipChar->mSprite, 2);
    mShipChar->Update();
    mCamera->SetFocus(mStage->GetShipPos()->x, 0);
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN10TitleStage10SetShipPosElll(void);
void _ZN11TitleCamera8SetFocusEll(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void TitleMenu::Show() {
    stmdb sp!, {r4, lr}
    mov r4, r0
    ldr r0, [r4, #0x38]
    mvn r1, #0x0
    ldr r2, [r0, #0x258]
    cmp r2, r1
    ldreq r1, [r0, #0x58]
    addne r1, r0, r2, lsl #0x2
    ldrne r1, [r1, #0x58]
    add r3, r1, #0x74
    mvn r1, #0x0
    cmp r2, r1
    ldreq r1, [r0, #0x58]
    addne r1, r0, r2, lsl #0x2
    ldrne r1, [r1, #0x58]
    ldr r2, [r3, #0x4]
    add ip, r1, #0x74
    mov r1, #0x2e000
    ldr r3, [ip, #0x8]
    rsb r1, r1, #0x0
    bl _ZN10TitleStage10SetShipPosElll
    ldr r1, [r4, #0x38]
    mvn r0, #0x0
    ldr r2, [r1, #0x258]
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    add r3, r0, #0x74
    mvn r0, #0x0
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    add ip, r0, #0x74
    mvn r0, #0x0
    cmp r2, r0
    ldreq r0, [r1, #0x58]
    addne r0, r1, r2, lsl #0x2
    ldrne r0, [r0, #0x58]
    ldr r2, [ip, #0x4]
    add r1, r0, #0x74
    ldr r0, [r3, #0x0]
    ldr r3, [r1, #0x8]
    ldr r1, [r4, #0x364]
    add r0, r0, #0x2000
    str r0, [r1, #0x2c]
    str r2, [r1, #0x30]
    add r0, r3, #0x4000
    str r0, [r1, #0x34]
    ldr r0, [r4, #0x364]
    mov r1, #0x2
    ldr r0, [r0, #0x14]
    bl func_0202b064
    ldr r0, [r4, #0x364]
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x8]
    blx r1
    ldr r2, [r4, #0x38]
    mvn r0, #0x0
    ldr r1, [r2, #0x258]
    cmp r1, r0
    ldreq r0, [r2, #0x58]
    addne r0, r2, r1, lsl #0x2
    ldrne r0, [r0, #0x58]
    mov r2, #0x0
    add r1, r0, #0x74
    ldr r0, [r4, #0x3c]
    ldr r1, [r1, #0x0]
    bl _ZN11TitleCamera8SetFocusEll
    ldmia sp!, {r4, pc}
}
#endif
