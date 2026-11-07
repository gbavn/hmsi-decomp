#ifndef NITRO_GX_GX_LOAD_H
#define NITRO_GX_GX_LOAD_H

#include "types.h"

#define reg_GX_DISPCNT (*(vu32 *)0x04000000)
#define reg_GXS_DB_DISPCNT (*(vu32 *)0x04001000)

#define REG_GX_DISPCNT_EXOBJ_CH_MASK 0x00300000
#define REG_GX_DISPCNT_OBJMAP_CH_MASK 0x00000010

typedef enum {
    GX_OBJVRAMMODE_CHAR_2D = 0,
    GX_OBJVRAMMODE_CHAR_1D_32K = 0x10,
    GX_OBJVRAMMODE_CHAR_1D_64K = 0x00100010,
    GX_OBJVRAMMODE_CHAR_1D_128K = 0x00200010,
    GX_OBJVRAMMODE_CHAR_1D_256K = 0x00300010
} GXOBJVRamModeChar;

typedef enum {
    GX_TEXSIZE_S8 = 0,
    GX_TEXSIZE_S16 = 1,
    GX_TEXSIZE_S32 = 2,
    GX_TEXSIZE_S64 = 3,
    GX_TEXSIZE_S128 = 4,
    GX_TEXSIZE_S256 = 5,
    GX_TEXSIZE_S512 = 6,
    GX_TEXSIZE_S1024 = 7
} GXTexSizeS;

typedef enum {
    GX_TEXSIZE_T8 = 0,
    GX_TEXSIZE_T16 = 1,
    GX_TEXSIZE_T32 = 2,
    GX_TEXSIZE_T64 = 3,
    GX_TEXSIZE_T128 = 4,
    GX_TEXSIZE_T256 = 5,
    GX_TEXSIZE_T512 = 6,
    GX_TEXSIZE_T1024 = 7
} GXTexSizeT;

typedef enum {
    GX_TEXFMT_NONE = 0,
    GX_TEXFMT_A3I5 = 1,
    GX_TEXFMT_PLTT4 = 2,
    GX_TEXFMT_PLTT16 = 3,
    GX_TEXFMT_PLTT256 = 4,
    GX_TEXFMT_COMP4x4 = 5,
    GX_TEXFMT_A5I3 = 6,
    GX_TEXFMT_DIRECT = 7
} GXTexFmt;

typedef enum {
    GX_TEXPLTTCOLOR0_USE = 0,
    GX_TEXPLTTCOLOR0_TRNS = 1
} GXTexPlttColor0;

static inline void GX_SetOBJVRamModeChar(GXOBJVRamModeChar mode) {
    reg_GX_DISPCNT = (u32)(reg_GX_DISPCNT & ~(REG_GX_DISPCNT_EXOBJ_CH_MASK | REG_GX_DISPCNT_OBJMAP_CH_MASK) | mode);
}

static inline void GXS_SetOBJVRamModeChar(GXOBJVRamModeChar mode) {
    reg_GXS_DB_DISPCNT =
        (u32)(reg_GXS_DB_DISPCNT & ~(REG_GX_DISPCNT_EXOBJ_CH_MASK | REG_GX_DISPCNT_OBJMAP_CH_MASK) | mode);
}

void GX_LoadOBJ(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadOBJ(const void *pSrc, u32 offset, u32 szByte);
void GX_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte);

void GX_BeginLoadOBJExtPltt(void);
void GX_LoadOBJExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte);
void GX_EndLoadOBJExtPltt(void);
void GXS_BeginLoadOBJExtPltt(void);
void GXS_LoadOBJExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte);
void GXS_EndLoadOBJExtPltt(void);

void GX_BeginLoadTex(void);
void GX_LoadTex(const void *pSrc, u32 destSlotAddr, u32 szByte);
void GX_EndLoadTex(void);
void GX_BeginLoadTexPltt(void);
void GX_LoadTexPltt(const void *pSrc, u32 destSlotAddr, u32 szByte);
void GX_EndLoadTexPltt(void);

#endif
