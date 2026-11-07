// Overlay 5, part 2: the two touch buttons of the title menu ("new game" / "continue").
#include "ov005_title.h"

TitleButton::TitleButton()
{
    mPressed = 0;
    mDecided = 0;
    mTouched = 0;
}

TitleButton::~TitleButton()
{
}

void TitleButton::OnTouch()
{
    if (IsPressed()) {
        func_02035e8c(0x6D, 2);
        if (mTouched) {
            mPressed = 1;
        } else {
            mTouched = 1;
        }
    }
}

void TitleButton::OnLeave()
{
    mTouched = 0;
}

void TitleButton::OnDecide()
{
    if (mPressed && !mDecided) {
        mPressed = 0;
        mDecided = 1;
        mMenu->GetScene()->Select(mIndex);
    }
}

BOOL TitleButton::Draw(BOOL active)
{
    void *renderer;

    if (mHasSave == 0 && mIndex == 1) {
        return TRUE;
    }
    if (active) {
        Button::Update();
    }
    renderer = data_020ec87c;
    if (mTouched || mDecided) {
        switch (mIndex) {
        case 0:
            if (mHasSave) {
                mSprites[2]->y = FX32_CONST(68);
            } else {
                mSprites[2]->y = FX32_CONST(98);
            }
            func_0202bc20(renderer, mSprites[2]);
            break;
        case 1:
            if (mHasSave) {
                mSprites[2]->y = FX32_CONST(126);
                func_0202bc20(renderer, mSprites[2]);
            }
            break;
        }
    } else if (mDecided) {
        func_0202b088(mSprites[2], 0x3000);
    } else {
        func_0202bc20(renderer, mSprites[2]);
    }
    func_0202bc20(renderer, !mDecided ? mSprites[0] : mSprites[1]);
    return TRUE;
}

TitleButton *TitleButton::Create(TitleMenu *menu)
{
    TitleButton *buttons = new TitleButton[2];
    u8 hasSave = menu->HasSaveData();
    u8 i;

    for (i = 0; i < 2; i++) {
        buttons[i].Init(menu, i, hasSave);
    }
    return buttons;
}

void TitleButton::Init(TitleMenu *menu, u8 index, u8 hasSave)
{
    u8 i;
    int j;

    mMenu = menu;
    mIndex = index;
    mHasSave = hasSave;
    if (!mHasSave && mIndex == 1) {
        return;
    }
    for (i = 0; i < 3; i++) {
        if (i == 2) {
            mSprites[i] = mMenu->GetCursorSprite();
        } else {
            mSprites[i] = mMenu->GetButtonSprite(mIndex, i);
        }
        mSprites[i]->alpha = 31;
    }
    if (!mHasSave) {
        if (mIndex != 0) {
            return;
        }
        for (j = 0; j < 3; j++) {
            mSprites[j]->y = ((mSprites[j]->y >> FX32_SHIFT) + 50) << FX32_SHIFT;
        }
        SetRect(60, 82, 136, 32);
    } else {
        switch (mIndex) {
        case 0:
            for (j = 0; j < 3; j++) {
                mSprites[j]->y = ((mSprites[j]->y >> FX32_SHIFT) + 20) << FX32_SHIFT;
            }
            SetRect(60, 52, 136, 32);
            break;
        case 1:
            for (j = 0; j < 3; j++) {
                mSprites[j]->y = ((mSprites[j]->y >> FX32_SHIFT) + 30) << FX32_SHIFT;
            }
            SetRect(60, (s16)(mIndex * 48 + 62), 136, 32);
            break;
        }
    }
}

void TitleButton::ClearPressed()
{
    mPressed = 0;
}

void TitleButton::SetTouched(u8 touched)
{
    mTouched = touched;
}

void TitleButton::Decide()
{
    mPressed = 0;
    mDecided = 1;
    mMenu->GetScene()->Select(mIndex);
}

u8 TitleButton::IsTouched()
{
    return mTouched;
}

u8 TitleButton::IsDecided()
{
    return mDecided;
}
