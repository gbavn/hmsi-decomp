// Overlay 5, part 4: the title screen task (top logo + bottom menu).
#include "ov005_title.h"

#define ANY_BUTTON(trg)                                                                             ((trg) & PAD_BUTTON_A || (trg) & PAD_BUTTON_B || (trg) & PAD_BUTTON_X || (trg) & PAD_BUTTON_Y ||      (trg) & PAD_BUTTON_L || (trg) & PAD_BUTTON_R || (trg) & PAD_BUTTON_START ||                      (trg) & PAD_BUTTON_SELECT)

enum {
    TITLE_PRESS_START,
    TITLE_OPEN_MENU,
    TITLE_WAIT_MENU,
    TITLE_LOGO_DONE,
    TITLE_TOUCHED,
    TITLE_MENU,
    TITLE_SELECTED,
    TITLE_START,
    TITLE_END,
};

TitleScene::TitleScene(TitleMain *main)
{
    GFileObject file;

    mArchiveData = (u8 *)GFile_Load(file.Get(), "rom/console/Title.LZ", 0);
    GFile_Read(file.Get(), GFile_GetLength(file.Get()), mArchiveData);
    mArchive = new Archive(mArchiveData);
    GFile_Close(file.Get());
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    mMain = main;
    mState = TITLE_PRESS_START;
    mHasSave = CheckSaveData();
    mMenu = new TitleMenu(this);
    mLogo = new TitleLogo(this);
    func_02035f30();
    func_02035e44(0x19);
    func_02035e8c(0x19, 0);
}

TitleScene::~TitleScene()
{
    if (mMenu != NULL) {
        delete mMenu;
    }
    if (mLogo != NULL) {
        delete mLogo;
    }
    if (mArchive != NULL) {
        delete mArchive;
    }
    delete[] mArchiveData;
}

#ifdef NONMATCHING
BOOL TitleScene::Update()
{
    mMenu->Animate();
    switch (mState) {
    case TITLE_PRESS_START:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        if (gInput.touchFlags & INPUT_TOUCH_PRESSED || ANY_BUTTON(gInput.trg)) {
            func_02037284(7, 2, 1);
            func_02037284(7, 2, 0);
            func_02035e8c(0x6D, 2);
            mState = TITLE_OPEN_MENU;
        }
        break;
    case TITLE_OPEN_MENU:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        mLogo->Skip();
        mMenu->Show();
        func_02037284(2, 2, 1);
        func_02037284(2, 2, 0);
        mState = TITLE_WAIT_MENU;
        break;
    case TITLE_WAIT_MENU:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        mState = TITLE_LOGO_DONE;
        break;
    case TITLE_LOGO_DONE:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        if (gInput.touchFlags & INPUT_TOUCH_PRESSED) {
            func_02035e8c(0x6D, 2);
            mState = TITLE_TOUCHED;
        } else if (ANY_BUTTON(gInput.trg)) {
            gInput.trg = 0;
            func_02035e8c(0x65, 2);
            unk1A = 0;
            mState = TITLE_MENU;
            mMenu->UpdateButtons(FALSE);
        }
        mMenu->BlinkPrompt();
        break;
    case TITLE_TOUCHED:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        if (gInput.touchFlags & INPUT_TOUCH_RELEASED) {
            unk1A = 0;
            mState = TITLE_MENU;
            mMenu->UpdateButtons(FALSE);
        } else {
            mMenu->BlinkPrompt();
        }
        break;
    case TITLE_MENU:
        if (func_020374f8(1) || func_020374f8(0)) {
            mMenu->UpdateButtons(FALSE);
        } else {
            mMenu->UpdateButtons(TRUE);
        }
        break;
    case TITLE_SELECTED:
        mMenu->UpdateButtons(FALSE);
        mTimer--;
        if (mTimer < 0) {
            mState = TITLE_START;
        }
        break;
    case TITLE_START:
        mMain->StartGame(mChoice);
        mMenu->UpdateButtons(FALSE);
        mState = TITLE_END;
        break;
    case TITLE_END:
        break;
    }
    mLogo->Update();
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN9TitleLogo4SkipEv(void);
void _ZN9TitleLogo6UpdateEv(void);
void _ZN9TitleMain9StartGameEi(void);
void _ZN9TitleMenu13UpdateButtonsEi(void);
void _ZN9TitleMenu4ShowEv(void);
void _ZN9TitleMenu7AnimateEv(void);
void _ZN9TitleMenu11BlinkPromptEv(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL TitleScene::Update() {
    stmdb sp!, {r4, lr}
    mov r4, r0
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu7AnimateEv
    ldrb r0, [r4, #0x18]
    cmp r0, #0x8
    addls pc, pc, r0, lsl #0x2
    b _02157170
_02156e68:
    b _02156e8c
    b _02156f3c
    b _02156f98
    b _02156fc0
    b _0215708c
    b _021570e4
    b _02157124
    b _02157150
    b _02157170
_02156e8c:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    ldr r0, [pc, #0x2cc]
    ldrb r0, [r0, #0x0]
    tst r0, #0x8
    bne _02156f04
    ldr r0, [pc, #0x2c0]
    ldrh r0, [r0, #0x0]
    tst r0, #0x1
    bne _02156f04
    tst r0, #0x2
    bne _02156f04
    tst r0, #0x400
    bne _02156f04
    tst r0, #0x800
    bne _02156f04
    tst r0, #0x200
    bne _02156f04
    tst r0, #0x100
    bne _02156f04
    tst r0, #0x8
    bne _02156f04
    tst r0, #0x4
    beq _02157170
_02156f04:
    mov r0, #0x7
    mov r1, #0x2
    mov r2, #0x1
    bl func_02037284
    mov r0, #0x7
    mov r1, #0x2
    mov r2, #0x0
    bl func_02037284
    mov r0, #0x6d
    mov r1, #0x2
    bl func_02035e8c
    mov r0, #0x1
    strb r0, [r4, #0x18]
    b _02157170
_02156f3c:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    ldr r0, [r4, #0x28]
    bl _ZN9TitleLogo4SkipEv
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu4ShowEv
    mov r0, #0x2
    mov r1, r0
    mov r2, #0x1
    bl func_02037284
    mov r0, #0x2
    mov r1, r0
    mov r2, #0x0
    bl func_02037284
    mov r0, #0x2
    strb r0, [r4, #0x18]
    b _02157170
_02156f98:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    moveq r0, #0x3
    streqb r0, [r4, #0x18]
    b _02157170
_02156fc0:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    ldr r0, [pc, #0x198]
    ldrb r0, [r0, #0x0]
    tst r0, #0x8
    beq _02157008
    mov r0, #0x6d
    mov r1, #0x2
    bl func_02035e8c
    mov r0, #0x4
    strb r0, [r4, #0x18]
    b _02157080
_02157008:
    ldr r0, [pc, #0x174]
    ldrh r0, [r0, #0x0]
    tst r0, #0x1
    bne _02157050
    tst r0, #0x2
    bne _02157050
    tst r0, #0x400
    bne _02157050
    tst r0, #0x800
    bne _02157050
    tst r0, #0x200
    bne _02157050
    tst r0, #0x100
    bne _02157050
    tst r0, #0x8
    bne _02157050
    tst r0, #0x4
    beq _02157080
_02157050:
    ldr r2, [pc, #0x12c]
    mov r3, #0x0
    mov r0, #0x65
    mov r1, #0x2
    strh r3, [r2, #0x0]
    bl func_02035e8c
    mov r1, #0x0
    strh r1, [r4, #0x1a]
    mov r0, #0x5
    strb r0, [r4, #0x18]
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu13UpdateButtonsEi
_02157080:
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu11BlinkPromptEv
    b _02157170
_0215708c:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _02157170
    ldr r0, [pc, #0xcc]
    ldrb r0, [r0, #0x0]
    tst r0, #0x20
    beq _021570d8
    mov r1, #0x0
    strh r1, [r4, #0x1a]
    mov r0, #0x5
    strb r0, [r4, #0x18]
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu13UpdateButtonsEi
    b _02157170
_021570d8:
    ldr r0, [r4, #0x24]
    bl _ZN9TitleMenu11BlinkPromptEv
    b _02157170
_021570e4:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _02157114
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _02157114
    ldr r0, [r4, #0x24]
    mov r1, #0x1
    bl _ZN9TitleMenu13UpdateButtonsEi
    b _02157170
_02157114:
    ldr r0, [r4, #0x24]
    mov r1, #0x0
    bl _ZN9TitleMenu13UpdateButtonsEi
    b _02157170
_02157124:
    ldr r0, [r4, #0x24]
    mov r1, #0x0
    bl _ZN9TitleMenu13UpdateButtonsEi
    ldrsb r0, [r4, #0x1c]
    sub r0, r0, #0x1
    strb r0, [r4, #0x1c]
    ldrsb r0, [r4, #0x1c]
    cmp r0, #0x0
    movlt r0, #0x7
    strltb r0, [r4, #0x18]
    b _02157170
_02157150:
    ldr r0, [r4, #0x20]
    ldr r1, [r4, #0x2c]
    bl _ZN9TitleMain9StartGameEi
    ldr r0, [r4, #0x24]
    mov r1, #0x0
    bl _ZN9TitleMenu13UpdateButtonsEi
    mov r0, #0x8
    strb r0, [r4, #0x18]
_02157170:
    ldr r0, [r4, #0x28]
    bl _ZN9TitleLogo6UpdateEv
    mov r0, #0x1
    ldmia sp!, {r4, pc}
    dcd gInput
    dcd gInput+0x6
}
#endif

void TitleScene::Draw()
{
    mMenu->Draw();
    mLogo->DrawFlare();
}

void TitleScene::Func4()
{
    mMenu->DrawBlend();
    mLogo->DrawBlend();
}

void TitleScene::OnLogoDone()
{
    mState = TITLE_LOGO_DONE;
}

void TitleScene::Select(u8 index)
{
    if (mState != TITLE_SELECTED && mState != TITLE_START) {
        func_02035e8c(0x65, 2);
        mState = TITLE_SELECTED;
        mChoice = index;
        mTimer = 60;
    }
}

Archive *TitleScene::GetArchive()
{
    return mArchive;
}

u8 TitleScene::HasSaveData()
{
    return mHasSave;
}

void TitleScene::ShowMenu()
{
    mState = TITLE_LOGO_DONE;
    mMenu->UpdateButtons(TRUE);
    mMenu->ResetButtons();
}

BOOL TitleScene::IsIdle()
{
    if (!func_020374f8(1) && !func_020374f8(0)) {
        return mState <= TITLE_MENU;
    }
    return FALSE;
}

BOOL TitleScene::IsLoaded()
{
    return mState != TITLE_OPEN_MENU;
}

// The header of a save slot (contents unknown).
struct SaveHeader {
    u8 unk0[0x44];
};

BOOL TitleScene::CheckSaveData()
{
    SaveHeader *buf = new SaveHeader;
    int i;

    for (i = 0; i < 2; i++) {
        switch (func_0205ade8(i, buf, sizeof(SaveHeader))) {
        case -3:
        case -2:
        case -1:
            break;
        case 0:
            delete buf;
            return TRUE;
        }
    }
    delete buf;
    return FALSE;
}
