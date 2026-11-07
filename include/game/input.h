#ifndef INPUT_H
#define INPUT_H

#include <nitro/spi.h>

#define INPUT_TP_BUF_COUNT 5

enum {
    INPUT_TOUCH_REPEAT = 0x04,
    INPUT_TOUCH_PRESSED = 0x08,
    INPUT_TOUCH_RELEASED = 0x20
};

typedef struct Input {
    u8 touchFlags;       // bit 4: touching; INPUT_TOUCH_*; low bits: TP validity
    u8 unk1;
    u16 release;         // buttons released this frame
    u16 cont;            // buttons held
    u16 trg;             // buttons pressed this frame
    u16 touchY;
    u16 repeat;          // auto-repeat pulses
    u16 touchX;
    TPData tpBuf[INPUT_TP_BUF_COUNT]; // TP auto-sampling buffer
    TPData tp;
    TPData prevTp;
    u16 touchHold;
    u16 touchHoldWait;
    u16 touchHoldCount;
    u16 keys;
    u16 repeatWait;
    u16 repeatCount;
    u16 repeatKeys;
    u32 unk54;
} Input;

extern Input gInput;

void Input_InitTP(void);
void Input_StartTP(void);
void Input_Update(void);
void Input_Clear(void);

#endif
