// input.cpp: touch panel and button state, updated once per frame.
// File and function names are ours; the original names are unknown.
#include <nitro/os.h>
#include <nitro/pad.h>
#include <nitro/spi.h>
#include "input.h"

Input gInput;

void Input_InitTP(void)
{
    TPCalibrateParam param;

    TP_Init();
    if (TP_GetUserInfo(&param) == FALSE) {
        OS_Terminate();
    }
    TP_SetCalibrateParam(&param);
}

void Input_StartTP(void)
{
    TP_RequestAutoSamplingStartAsync(0, 4, gInput.tpBuf, INPUT_TP_BUF_COUNT);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
    if (TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON)) {
        OS_Terminate();
    }
}

#ifdef NONMATCHING
// Only the registers of old/now (button edge detection) differ.
void Input_Update(void)
{
    s32 first;
    int i;
    u16 touch;
    u16 prevTouch;
    u16 old;
    u16 now;
    u32 changed;

    gInput.prevTp = gInput.tp;
    first = TP_GetLatestIndexInAuto() - 3;
    for (i = 0; i < 4; i++) {
        s32 idx = first + i;
        TPData *data;

        if (idx < 0) {
            idx += INPUT_TP_BUF_COUNT;
        }
        data = &gInput.tpBuf[idx];
        if (data->touch == 0) {
            gInput.tp.touch = 0;
        } else if (data->validity == 0) {
            TP_GetCalibratedPoint(&gInput.tp, data);
        }
    }

    gInput.touchX = gInput.tp.x;
    gInput.touchY = gInput.tp.y;
    touch = gInput.tp.touch;
    gInput.touchFlags = (u8)(touch << 4);
    prevTouch = gInput.prevTp.touch;
    if (prevTouch == 0 && touch == 1) {
        gInput.touchFlags |= INPUT_TOUCH_PRESSED;
    }
    if (prevTouch == 1 && touch == 0) {
        gInput.touchFlags |= INPUT_TOUCH_RELEASED;
    }

    if (gInput.touchHold == 0) {
        if (touch == 1) {
            gInput.touchHold = 1;
        }
    } else if (touch == 1) {
        if (gInput.touchHoldWait < 8) {
            gInput.touchHoldWait++;
        } else if (gInput.touchHoldCount < 3) {
            gInput.touchHoldCount++;
        } else {
            gInput.touchHoldCount = 0;
            gInput.touchFlags |= INPUT_TOUCH_REPEAT;
        }
    } else {
        gInput.touchHold = 0;
        gInput.touchHoldWait = 0;
        gInput.touchHoldCount = 0;
    }

    gInput.touchFlags |= gInput.tp.validity;
    gInput.keys = (u16)(PAD_Read() & PAD_ALL_MASK);

    old = gInput.cont;
    now = gInput.keys;
    changed = old ^ now;
    gInput.trg = now & changed;
    gInput.release = old & changed;
    gInput.cont = now;

    if (gInput.repeatKeys == 0) {
        if (gInput.trg != 0) {
            gInput.repeatWait = 0;
            gInput.repeatCount = 0;
            gInput.repeatKeys = gInput.trg;
        }
    } else if (now == gInput.repeatKeys) {
        if (gInput.repeatWait < 8) {
            gInput.repeatWait++;
            gInput.repeat = 0;
        } else if (gInput.repeatCount < 3) {
            gInput.repeatCount++;
            gInput.repeat = 0;
        } else {
            gInput.repeatCount = 0;
            gInput.repeat = gInput.repeatKeys;
        }
    } else if (gInput.repeatKeys == 0) {
        gInput.repeatKeys = 0;
        gInput.repeat = 0;
    } else {
        gInput.repeatKeys = now;
        gInput.repeatWait = 0;
        gInput.repeatCount = 0;
        gInput.repeat = 0;
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void Input_Update(void) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    ldr r0, =gInput
    ldrh r1, [r0, #0x36]
    strh r1, [r0, #0x3e]
    ldrh r1, [r0, #0x38]
    strh r1, [r0, #0x40]
    ldrh r1, [r0, #0x3a]
    strh r1, [r0, #0x42]
    ldrh r1, [r0, #0x3c]
    strh r1, [r0, #0x44]
    bl TP_GetLatestIndexInAuto
    mov r6, #0x0
    ldr r4, =gInput+0xe
    ldr r8, =gInput+0x36
    ldr r7, =gInput
    sub r5, r0, #0x3
    mov r9, r6
_02028e64:
    adds r0, r5, r6
    addmi r0, r0, #0x5
    add r1, r4, r0, lsl #0x3
    ldrh r0, [r1, #0x4]
    cmp r0, #0x0
    streqh r9, [r7, #0x3a]
    beq _02028e94
    ldrh r0, [r1, #0x6]
    cmp r0, #0x0
    bne _02028e94
    mov r0, r8
    bl TP_GetCalibratedPoint
_02028e94:
    add r6, r6, #0x1
    cmp r6, #0x4
    blt _02028e64
    ldr r0, =gInput
    ldrh r1, [r0, #0x36]
    strh r1, [r0, #0xc]
    ldrh r1, [r0, #0x38]
    strh r1, [r0, #0x8]
    ldrh r2, [r0, #0x3a]
    mov r1, r2, lsl #0x4
    strb r1, [r0, #0x0]
    ldrh r3, [r0, #0x42]
    cmp r3, #0x0
    cmpeq r2, #0x1
    andeq r1, r1, #0xff
    orreq r1, r1, #0x8
    streqb r1, [r0, #0x0]
    cmp r3, #0x1
    cmpeq r2, #0x0
    bne _02028ef4
    ldr r0, =gInput
    ldrb r1, [r0, #0x0]
    orr r1, r1, #0x20
    strb r1, [r0, #0x0]
_02028ef4:
    ldr r0, =gInput
    ldrh r1, [r0, #0x46]
    cmp r1, #0x0
    bne _02028f14
    cmp r2, #0x1
    moveq r1, #0x1
    streqh r1, [r0, #0x46]
    b _02028f6c
_02028f14:
    cmp r2, #0x1
    bne _02028f5c
    ldrh r1, [r0, #0x48]
    cmp r1, #0x8
    addlo r1, r1, #0x1
    strloh r1, [r0, #0x48]
    blo _02028f6c
    ldrh r1, [r0, #0x4a]
    cmp r1, #0x3
    addlo r1, r1, #0x1
    strloh r1, [r0, #0x4a]
    blo _02028f6c
    mov r1, #0x0
    strh r1, [r0, #0x4a]
    ldrb r1, [r0, #0x0]
    orr r1, r1, #0x4
    strb r1, [r0, #0x0]
    b _02028f6c
_02028f5c:
    mov r1, #0x0
    strh r1, [r0, #0x46]
    strh r1, [r0, #0x48]
    strh r1, [r0, #0x4a]
_02028f6c:
    ldr r2, =gInput
    ldr r3, =0x4000130
    ldrb r5, [r2, #0x0]
    ldrh r4, [r2, #0x3c]
    ldr r0, =0x27fffa8
    ldr r1, =0x2fff
    orr r4, r5, r4
    strb r4, [r2, #0x0]
    ldrh r3, [r3, #0x0]
    ldrh r0, [r0, #0x0]
    orr r0, r3, r0
    eor r0, r0, r1
    and r0, r0, r1
    mov r0, r0, lsl #0x10
    and r0, r1, r0, lsr #0x10
    strh r0, [r2, #0x4c]
    ldrh r4, [r2, #0x4]
    ldrh r3, [r2, #0x4c]
    eor r1, r3, r4
    and r0, r3, r1
    strh r0, [r2, #0x6]
    and r0, r4, r1
    strh r0, [r2, #0x2]
    strh r3, [r2, #0x4]
    ldrh r1, [r2, #0x52]
    cmp r1, #0x0
    bne _02028ff8
    ldrh r1, [r2, #0x6]
    cmp r1, #0x0
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    mov r0, #0x0
    strh r0, [r2, #0x4e]
    strh r0, [r2, #0x50]
    strh r1, [r2, #0x52]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_02028ff8:
    cmp r3, r1
    bne _02029050
    ldrh r0, [r2, #0x4e]
    cmp r0, #0x8
    bhs _02029020
    add r0, r0, #0x1
    strh r0, [r2, #0x4e]
    mov r0, #0x0
    strh r0, [r2, #0xa]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_02029020:
    ldrh r0, [r2, #0x50]
    cmp r0, #0x3
    bhs _02029040
    add r0, r0, #0x1
    strh r0, [r2, #0x50]
    mov r0, #0x0
    strh r0, [r2, #0xa]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_02029040:
    mov r0, #0x0
    strh r0, [r2, #0x50]
    strh r1, [r2, #0xa]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_02029050:
    mov r0, #0x0
    cmp r1, #0x0
    streqh r0, [r2, #0x52]
    streqh r0, [r2, #0xa]
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    strh r3, [r2, #0x52]
    strh r0, [r2, #0x4e]
    strh r0, [r2, #0x50]
    strh r0, [r2, #0xa]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
}
#endif

void Input_Clear(void)
{
    gInput.touchX = 0;
    gInput.touchY = 0;
    gInput.touchFlags = 0;
    gInput.trg = 0;
    gInput.cont = 0;
    gInput.repeat = 0;
    gInput.release = 0;
    gInput.unk1 = 0;
}
