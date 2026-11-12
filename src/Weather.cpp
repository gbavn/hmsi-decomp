// Rain and snow on the field (ARM9 0x02040390..0x02040ab8). Names are ours.
//
// A Task (vtable 0x020bdb48 in the asm gap: [~Weather, ~Weather + delete, Update, Task::Draw, Task::Func4])
// that moves 10 (rain), 4 or 7 (snow) sprites down the screen. The sprites come from
// rom/effect/weather.bin, an archive with RAIN.NCBR/NCLR/NCER/NANR and SNOW.*.
#include <nitro.h>
#include <stdio.h>
#include "game_file.h"
#include "Sprite2D.h"

u32 RandRange(u32 min, u32 max);

extern "C" {

typedef struct ArchiveView {
    void *vtable;
    void *data;
} ArchiveView;

typedef struct WeatherDrop {
    Sprite2D *sprite;
    VecFx32 pos;
    fx32 speed;             // snow: falling speed
    u16 angle;              // snow: sway
    u16 angleStep;
    fx32 sway;
} WeatherDrop;

typedef struct Weather {
    u8 task[0x14];
    u8 *data;               // 0x14 weather.bin
    int kind;               // 0x18 3 = rain, 4 = ?, 5 = snow
    int unk1C;
    int count;              // 0x20
    WeatherDrop drops[10];  // 0x24
} Weather;

extern void *data_020bdb48[];       // vtable
extern void *_ZTV11GFileObject[];
extern char data_020bdb5c[];        // "rom/effect/weather.bin"
extern char data_020bdb74[];        // "%s.%s"
extern char *data_020bdb18[2];      // "RAIN", "SNOW"
extern char *data_020bdb20[4];      // "NCBR", "NCLR", "NCER", "NANR"
extern VecFx32 data_020b2c90;       // rain speed
extern VecFx32 data_020b2c9c;

void _ZN4TaskC2Ev(void *self);
void _ZN4TaskD2Ev(void *self);
void _ZN4TaskdlEPv(void *self);
ArchiveView *_ZN7ArchiveC1EPv(ArchiveView *self, void *data);
void *_ZN7Archive7GetFileEPKc(ArchiveView *self, const char *name);
u32 func_020358d0(u32 max);

void func_02040940(Weather *self, int i, fx32 x, fx32 y);

#ifdef NONMATCHING
Weather *func_02040390(Weather *self, int unk, int kind)
{
    _ZN4TaskC2Ev(self);
    *(void ***)self = data_020bdb48;
    self->kind = kind;
    self->unk1C = unk;
    self->count = 10;
    int snow = self->kind == 5 ? 1 : 0;
    GFileObject file;
    GFile_Open(file.Get(), data_020bdb5c);
    self->data = new u8[GFile_GetLength(file.Get())];
    GFile_Read(file.Get(), GFile_GetLength(file.Get()), self->data);
    GFile_Close(file.Get());
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, self->data);
    u16 seq = 0;
    switch (self->kind) {
    case 5:
        self->count = 7;
        break;
    case 4:
        self->count = 4;
        break;
    case 3:
        seq = 1;
        break;
    }
    for (int i = 0; i < self->count; i++) {
        char names[4][0x20];
        for (int j = 0; j < 4; j++) {
            sprintf(names[j], data_020bdb74, data_020bdb18[snow], data_020bdb20[j]);
        }
        WeatherDrop *d = &self->drops[i];
        void *ncbr = _ZN7Archive7GetFileEPKc(&arc, names[0]);
        void *nclr = _ZN7Archive7GetFileEPKc(&arc, names[1]);
        void *ncer = _ZN7Archive7GetFileEPKc(&arc, names[2]);
        void *nanr = _ZN7Archive7GetFileEPKc(&arc, names[3]);
        Sprite2D *s = func_0202b41c(ncbr, nclr, ncer, nanr, names[0], names[1], names[2], names[3], 0, 0);
        d->sprite = s;
        NNS_G2dSetCellAnimationSequence(d->sprite->cellAnim, NNS_G2dGetAnimSequenceByIdx(s->animBank, seq));
        func_02040940(self, i, FX_F32_TO_FX32(RandRange(10, 0x128)), FX_F32_TO_FX32(func_020358d0(0xc0)));
        func_0202b1d0(self->drops[i].sprite, 2);
    }
    return self;
}
#else
// Linker names used by the asm below.
extern "C" {
void _Z10GFile_InitP5GFile(void);
void _Z10GFile_OpenP5GFilePKc(void);
void _Z10GFile_ReadP5GFilemPv(void);
void _Z11GFile_CloseP5GFile(void);
void _Z15GFile_GetLengthP5GFile(void);
void _Z9RandRangemm(void);
void _Znam(void);
void _fadd(void);
void _ffix(void);
void _ffltu(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm Weather *func_02040390(Weather *self, int unk, int kind) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0xf4
    mov r10, r0
    mov r5, r1
    mov r4, r2
    bl _ZN4TaskC2Ev
    ldr r1, [pc, #0x2b0]
    mov r0, #0xa
    str r1, [r10, #0x0]
    str r4, [r10, #0x18]
    str r5, [r10, #0x1c]
    str r0, [r10, #0x20]
    ldr r0, [r10, #0x18]
    ldr r1, [pc, #0x298]
    cmp r0, #0x5
    moveq r9, #0x1
    add r0, sp, #0xa8
    movne r9, #0x0
    str r1, [sp, #0xa8]
    bl _Z10GFile_InitP5GFile
    ldr r1, [pc, #0x280]
    add r0, sp, #0xa8
    bl _Z10GFile_OpenP5GFilePKc
    add r0, sp, #0xa8
    bl _Z15GFile_GetLengthP5GFile
    rsb r0, r0, r0, lsl #0x1
    bl _Znam
    str r0, [r10, #0x14]
    add r0, sp, #0xa8
    bl _Z15GFile_GetLengthP5GFile
    mov r1, r0
    ldr r2, [r10, #0x14]
    add r0, sp, #0xa8
    bl _Z10GFile_ReadP5GFilemPv
    add r0, sp, #0xa8
    bl _Z11GFile_CloseP5GFile
    ldr r1, [r10, #0x14]
    add r0, sp, #0x20
    bl _ZN7ArchiveC1EPv
    mov r0, #0x0
    str r0, [sp, #0x1c]
    ldr r0, [r10, #0x18]
    cmp r0, #0x3
    beq _02040464
    cmp r0, #0x4
    beq _02040458
    cmp r0, #0x5
    moveq r0, #0x7
    streq r0, [r10, #0x20]
    b _0204046c
_02040458:
    mov r0, #0x4
    str r0, [r10, #0x20]
    b _0204046c
_02040464:
    mov r0, #0x1
    str r0, [sp, #0x1c]
_0204046c:
    ldr r1, [r10, #0x20]
    mov r0, #0x0
    str r0, [sp, #0x18]
    cmp r1, #0x0
    ble _02040644
    mov r7, r0
_02040484:
    ldr r11, [pc, #0x1e0]
    ldr r5, [pc, #0x1e0]
    ldr r4, [pc, #0x1e0]
    add r8, sp, #0x28
    mov r6, #0x0
_02040498:
    ldr r2, [r5, r9, lsl #0x2]
    ldr r3, [r4, r6, lsl #0x2]
    mov r0, r8
    mov r1, r11
    bl sprintf
    add r6, r6, #0x1
    cmp r6, #0x4
    add r8, r8, #0x20
    blt _02040498
    add r0, sp, #0x20
    add r1, sp, #0x28
    add r4, r10, r7
    bl _ZN7Archive7GetFileEPKc
    mov r8, r0
    add r0, sp, #0x20
    add r1, sp, #0x48
    bl _ZN7Archive7GetFileEPKc
    mov r6, r0
    add r0, sp, #0x20
    add r1, sp, #0x68
    bl _ZN7Archive7GetFileEPKc
    mov r5, r0
    add r0, sp, #0x20
    add r1, sp, #0x88
    bl _ZN7Archive7GetFileEPKc
    mov r1, r6
    add r6, sp, #0x28
    mov r2, r5
    str r6, [sp, #0x0]
    add r5, sp, #0x48
    str r5, [sp, #0x4]
    add r6, sp, #0x68
    mov r3, r0
    add r5, sp, #0x88
    str r6, [sp, #0x8]
    str r5, [sp, #0xc]
    mov r5, #0x0
    str r5, [sp, #0x10]
    mov r0, r8
    str r5, [sp, #0x14]
    bl func_0202b41c
    str r0, [r4, #0x24]
    ldr r0, [r0, #0x4c]
    ldr r1, [sp, #0x1c]
    bl NNS_G2dGetAnimSequenceByIdx
    mov r1, r0
    mov r0, r4
    ldr r0, [r0, #0x24]
    ldr r0, [r0, #0x50]
    bl NNS_G2dSetCellAnimationSequence
    mov r0, #0xc0
    bl func_020358d0
    cmp r0, #0x0
    mov r0, #0xc0
    beq _02040590
    bl func_020358d0
    mov r0, r0, lsl #0xc
    bl _ffltu
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _020405a4
_02040590:
    bl func_020358d0
    mov r0, r0, lsl #0xc
    bl _ffltu
    mov r1, #0x3f000000
    bl _fsub
_020405a4:
    mov r5, r0
    mov r0, #0xa
    mov r1, #0x128
    bl _Z9RandRangemm
    cmp r0, #0x0
    mov r0, #0xa
    mov r1, #0x128
    beq _020405e0
    bl _Z9RandRangemm
    mov r0, r0, lsl #0xc
    bl _ffltu
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _020405f4
_020405e0:
    bl _Z9RandRangemm
    mov r0, r0, lsl #0xc
    bl _ffltu
    mov r1, #0x3f000000
    bl _fsub
_020405f4:
    bl _ffix
    mov r4, r0
    mov r0, r5
    bl _ffix
    mov r3, r0
    ldr r1, [sp, #0x18]
    mov r2, r4
    mov r0, r10
    bl func_02040940
    add r0, r10, r7
    ldr r0, [r0, #0x24]
    mov r1, #0x2
    bl func_0202b1d0
    ldr r0, [sp, #0x18]
    ldr r1, [r10, #0x20]
    add r0, r0, #0x1
    add r7, r7, #0x1c
    str r0, [sp, #0x18]
    cmp r0, r1
    blt _02040484
_02040644:
    ldr r1, [pc, #0x18]
    add r0, sp, #0xa8
    str r1, [sp, #0xa8]
    bl _Z11GFile_CloseP5GFile
    mov r0, r10
    add sp, sp, #0xf4
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_020bdb48
    dcd _ZTV11GFileObject+8
    dcd data_020bdb5c
    dcd data_020bdb74
    dcd data_020bdb18
    dcd data_020bdb20
}
#endif

Weather *func_02040678(Weather *self)
{
    *(void ***)self = data_020bdb48;
    for (int i = 0; i < self->count; i++) {
        delete self->drops[i].sprite->cellAnim;
        func_0202ba04(self->drops[i].sprite, TRUE);
    }
    delete[] self->data;
    _ZN4TaskD2Ev(self);
    return self;
}

Weather *func_020406e8(Weather *self)
{
    *(void ***)self = data_020bdb48;
    for (int i = 0; i < self->count; i++) {
        delete self->drops[i].sprite->cellAnim;
        func_0202ba04(self->drops[i].sprite, TRUE);
    }
    delete[] self->data;
    _ZN4TaskD2Ev(self);
    _ZN4TaskdlEPv(self);
    return self;
}

BOOL func_02040760(Weather *self)
{
    for (int i = 0; i < self->count; i++) {
        if (self->kind == 3) {
            VEC_Add(&self->drops[i].pos, &data_020b2c90, &self->drops[i].pos);
        } else if (self->kind == 4) {
            VEC_Add(&self->drops[i].pos, &data_020b2c9c, &self->drops[i].pos);
        } else {
            VecFx32 v = {0, 0, 0};
            v.x = FX_Mul(FX_SinIdx(self->drops[i].angle), self->drops[i].sway);
            v.y = self->drops[i].speed;
            self->drops[i].angle += self->drops[i].angleStep;
            self->drops[i].angle %= 0xffff;
            VEC_Add(&self->drops[i].pos, &v, &self->drops[i].pos);
        }
        if (self->drops[i].pos.y >= 0xc0000) {
            func_02040940(self, i, FX_F32_TO_FX32(RandRange(10, 0x128)), -0xa000);
        }
        self->drops[i].sprite->x = self->drops[i].pos.x;
        self->drops[i].sprite->y = self->drops[i].pos.y;
        func_0202bc20(gRenderer2D, self->drops[i].sprite);
    }
    return TRUE;
}

// Puts drop i at (x, y) (snow: new random sway).
void func_02040940(Weather *self, int i, fx32 x, fx32 y)
{
    self->drops[i].pos.x = x;
    self->drops[i].pos.y = y;
    if (self->kind != 5) {
        return;
    }
    NNS_G2dRestartCellAnimation(self->drops[i].sprite->cellAnim);
    self->drops[i].angle = func_020358d0(0xffff);
    self->drops[i].angleStep = RandRange(500, 1000);
    self->drops[i].speed = FX_F32_TO_FX32(RandRange(25, 35) / 10.0f);
    self->drops[i].sway = FX_F32_TO_FX32(RandRange(5, 10) / 10.0f);
}

}
