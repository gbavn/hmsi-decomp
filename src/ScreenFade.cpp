// Screen fades with the master brightness (ARM9 0x02037238..0x0203772c), one fade per screen
// (index 0 = sub screen, 1 = main screen). Names are ours.
//
// The variables are file statics in the original (in the asm gap); the init function addresses them
// relative to the first one (view `gFade`), the others use one symbol per array.
#include <nitro.h>

extern "C" {

typedef struct FadeVars {
    s16 cur;            // screen being updated
    s16 delay[2];       // frames between steps
    s16 step[2];        // brightness change per step
    s16 timer[2];
    s16 mode[2];        // 1/6 fade from black, 2/7 fade to white... (see func_02037558)
    s16 active[2];
} FadeVars;

extern s16 data_0214f6c8;           // gFade.cur
extern s16 data_0214f6ca[2];        // delay
extern s16 data_0214f6ce[2];        // step
extern s16 data_0214f6d2[2];        // timer
extern s16 data_0214f6d6[2];        // mode
extern s16 data_0214f6da[2];        // active
extern s8 data_0214f6e4[2];         // brightness
extern u8 data_0214f6e8[2];         // fading
extern u32 data_0214f6ec;

#define gFade (*(FadeVars *)&data_0214f6c8)

void func_02037558(int kind);

void func_02037238(void)
{
    gFade.active[0] = 0;
    gFade.active[1] = 0;
    gFade.timer[0] = 0;
    gFade.timer[1] = 0;
    gFade.mode[0] = 0;
    gFade.mode[1] = 0;
    gFade.cur = 0;
    data_0214f6e4[0] = 0;
    data_0214f6e4[1] = 0;
    data_0214f6e8[0] = 0;
    data_0214f6e8[1] = 0;
}

// Starts fade `mode` on `screen`; `frames` < 0: one step every -frames frames, > 0: steps of frames + 1.
void func_02037284(int mode, int frames, int screen)
{
    if (data_0214f6da[screen] != 0) {
        return;
    }
    data_0214f6d6[screen] = mode;
    data_0214f6da[screen] = 1;
    data_0214f6d2[screen] = 0;
    switch (data_0214f6d6[screen]) {
    case 1:
        data_0214f6e4[screen] = -16;
        data_0214f6e8[screen] = 1;
        break;
    case 6:
        data_0214f6e4[screen] = 0;
        data_0214f6e8[screen] = 1;
        break;
    case 2:
        data_0214f6e4[screen] = 16;
        data_0214f6e8[screen] = 1;
        break;
    case 7:
        data_0214f6e4[screen] = 0;
        data_0214f6e8[screen] = 1;
        break;
    case 10:
        break;
    }
    if (screen != 0) {
        GX_SetMasterBrightness(data_0214f6e4[screen]);
    } else {
        GXS_SetMasterBrightness(data_0214f6e4[screen]);
    }
    if (frames < 0) {
        data_0214f6ca[screen] = -frames;
        data_0214f6ce[screen] = 1;
    } else if (frames == 0) {
        data_0214f6ca[screen] = 0;
        data_0214f6ce[screen] = 1;
    } else if (frames > 0) {
        data_0214f6ca[screen] = 0;
        data_0214f6ce[screen] = frames + 1;
    }
}

// Called every frame.
void func_02037410(void)
{
    for (data_0214f6c8 = 1; data_0214f6c8 >= 0; data_0214f6c8--) {
        if (data_0214f6da[data_0214f6c8] == 0) {
            continue;
        }
        if (data_0214f6d6[data_0214f6c8] == 1) {
            func_02037558(0);
        } else if (data_0214f6d6[data_0214f6c8] == 6) {
            func_02037558(1);
        } else if (data_0214f6d6[data_0214f6c8] == 2) {
            func_02037558(2);
        } else if (data_0214f6d6[data_0214f6c8] == 7) {
            func_02037558(3);
        } else {
            data_0214f6d2[data_0214f6c8]++;
            if (data_0214f6d2[data_0214f6c8] > data_0214f6ca[data_0214f6c8]) {
                data_0214f6d2[data_0214f6c8] = 0;
                data_0214f6da[data_0214f6c8]++;
            }
        }
    }
}

BOOL func_020374f8(int screen)
{
    return data_0214f6da[screen] != 0;
}

// FALSE when both screens are at normal brightness.
BOOL func_02037518(void)
{
    BOOL normal = FALSE;

    if (GX_GetMasterBrightness() == 0 && GXS_GetMasterBrightness() == 0) {
        normal = TRUE;
    }
    return normal == FALSE;
}

// One fade step: 0 = from black, 1 = to black, 2 = from white, 3 = to white.
void func_02037558(int kind)
{
    int s = data_0214f6c8;

    data_0214f6d2[s]++;
    if (data_0214f6d2[s] > data_0214f6ca[s]) {
        data_0214f6d2[s] = 0;
        switch (kind) {
        case 0:
            data_0214f6e4[s] += data_0214f6ce[s];
            if (data_0214f6e4[s] >= 0) {
                data_0214f6e4[s] = 0;
                data_0214f6da[s] = 0;
                data_0214f6e8[s] = 0;
            }
            break;
        case 1:
            data_0214f6e4[s] -= data_0214f6ce[s];
            if (data_0214f6e4[s] <= -16) {
                data_0214f6e4[s] = -16;
                data_0214f6da[s] = 0;
                data_0214f6e8[s] = 0;
            }
            break;
        case 2:
            data_0214f6e4[s] -= data_0214f6ce[s];
            if (data_0214f6e4[s] <= 0) {
                data_0214f6e4[s] = 0;
                data_0214f6da[s] = 0;
                data_0214f6e8[s] = 0;
            }
            break;
        case 3:
            data_0214f6e4[s] += data_0214f6ce[s];
            if (data_0214f6e4[s] >= 16) {
                data_0214f6e4[s] = 16;
                data_0214f6da[s] = 0;
                data_0214f6e8[s] = 0;
            }
            break;
        }
    }
    if (s != 0) {
        while (!(reg_GX_DISPSTAT & REG_GX_DISPSTAT_VBLK_MASK)) {
        }
        GX_SetMasterBrightness(data_0214f6e4[s]);
    } else {
        while (!(reg_GX_DISPSTAT & REG_GX_DISPSTAT_VBLK_MASK)) {
        }
        GXS_SetMasterBrightness(data_0214f6e4[s]);
    }
}

void func_0203771c(u32 value)
{
    data_0214f6ec = value;
}

}
