#ifndef BUTTON_H
#define BUTTON_H

#include <nitro/types.h>

// A touch-screen button (ARM9 0x02059324..). Update() reads the touch panel and calls the
// OnTouch/OnLeave/OnDecide hooks. Names are ours; vtable layout from the ROM (0x020bf9dc).
class Button {
public:
    Button();
    virtual ~Button();
    virtual void Update();
    virtual void Func3();
    virtual void OnTouch();
    virtual void OnLeave();
    virtual void OnDecide();

    BOOL IsPressed();
    void SetRect(s16 x, s16 y, u8 w, u8 h);

    s16 mX;
    s16 mY;
    u8 mW;
    u8 mH;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
};

#endif
