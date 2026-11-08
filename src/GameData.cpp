// The game state / save data (ARM9 0x02030cfc..): one big object (pointer at 0x020f57c0) with the
// calendar, the weather forecast, the flags and everything else that is saved. Names are ours.
//
// The original is a single C++ file with ~190 functions; it is decompiled in pieces (this unit covers
// 0x02030cfc..0x020311e4). Its .data/.rodata/.bss stay in assembly for now and are used by symbol.
#include <nitro.h>
#include <string.h>

extern "C" {
void func_02027698(void *array, u32 count, u32 size, void *ctor, void *dtor);   // __construct_array
void func_020277d8(void *array, u32 count, u32 size, void *dtor);               // __destroy_arr
void _ZdlPv(void *ptr);

void *func_020350a4(void *self);
void *func_020350c0(void *self);
void func_020770ec(void *obj);
void func_02077100(void *obj);
void func_020a01e8(void *obj);
void func_020a01ec(void *obj);
void func_02033290(void *obj);
void func_0205e898(void);
void func_0205e90c(void);
void func_020330c8(void);
void func_020330dc(void);
void func_02077880(void);
void func_020778a4(void);
void func_0207005c(void);
void func_0207008c(void);
void func_0207f538(void);
void func_0207f550(void);

void func_020354c4(void *date);
void func_02035190(void *time);
u32 func_02035968(void *date);

void func_020310fc(void *self);
void func_02031120(void *self);
void func_02031158(void *self);
void func_020311e4(void *self);
void func_0205a0d0(void *self);
void func_0205bfc0(void *self);
void func_020317f0(void *self);
void func_020318c8(void *self);
void func_02032288(void *self);
void func_02032470(void *self);
void func_02032510(void *self);
void func_020331fc(void *self);
void func_0203327c(void *self);
void func_020332a8(void *self);
void func_02033304(void *self);
void func_02033364(void *self);
void func_020333f4(void *self);
void func_02033484(void *self);
void func_02033ca4(void *self);
void func_02033e8c(void *self);
void func_02033fec(void *self);
void func_020340b0(void *self);
void func_020342ac(void *self);
void func_020343dc(void *self);
void func_02070968(void *self);
void func_020348cc(void *self);
void func_0203506c(void *self);
void func_02034bc8(void *self);

extern void *data_020bc8bc[];
extern const u8 data_020b1df0[];        // start time (2 bytes), then the start date (8 bytes)
extern u8 data_020f57d0;
}

// Calendar date (8 bytes; byte 5 is the day).
typedef struct GameDate {
    u32 unk0;
    s8 unk4;
    s8 day;
    u8 unk6[2];
} GameDate;

typedef struct GameTime {
    u8 hour;
    u8 minute;
} GameTime;

typedef struct Bytes5 {
    u8 b[5];
} Bytes5;

typedef struct GameData {
    void **vt;
    u8 unk4;
    s8 initialized;
    u8 unk6[0x18 - 6];
    u32 weather[7];         // forecast: [0] = today
    u32 weatherYesterday;
} GameData;

struct ZeroDate {
    ZeroDate() : a(0), b(0) {}

    u32 a;
    u32 b;
};

#define AT(type, off) (*(type *)((u8 *)self + (off)))

extern "C" {

GameData *func_02030cfc(GameData *self)
{
    func_020350a4(self);
    self->vt = data_020bc8bc;
    self->initialized = 0;
    func_02027698((u8 *)self + 0x95c0, 0xfe, 0x4c, (void *)func_0205e898, (void *)func_0205e90c);
    func_02027698((u8 *)self + 0xe128, 4, 0x10, (void *)func_020330c8, (void *)func_020330dc);
    func_02027698((u8 *)self + 0xe168, 6, 0x14, (void *)func_02077880, (void *)func_020778a4);
    func_020770ec((u8 *)self + 0xe1e0);
    func_020a01e8((u8 *)self + 0xe1e8);
    func_02033290((u8 *)self + 0xe284);
    func_02027698((u8 *)self + 0xe290, 0xc, 0x10, (void *)func_0207005c, (void *)func_0207008c);
    func_02027698((u8 *)self + 0x1080c, 6, 6, (void *)func_0207f538, (void *)func_0207f550);
    strcpy((char *)&AT(u8, 0x1083f), "_Eng");

    return self;
}

GameData *func_02030e54(GameData *self)
{
    func_020277d8((u8 *)self + 0x1080c, 6, 6, (void *)func_0207f550);
    func_020277d8((u8 *)self + 0xe290, 0xc, 0x10, (void *)func_0207008c);
    func_020a01ec((u8 *)self + 0xe1e8);
    func_02077100((u8 *)self + 0xe1e0);
    func_020277d8((u8 *)self + 0xe168, 6, 0x14, (void *)func_020778a4);
    func_020277d8((u8 *)self + 0xe128, 4, 0x10, (void *)func_020330dc);
    func_020277d8((u8 *)self + 0x95c0, 0xfe, 0x4c, (void *)func_0205e90c);
    func_020350c0(self);
    return self;
}

GameData *func_02030f10(GameData *self)
{
    func_020277d8((u8 *)self + 0x1080c, 6, 6, (void *)func_0207f550);
    func_020277d8((u8 *)self + 0xe290, 0xc, 0x10, (void *)func_0207008c);
    func_020a01ec((u8 *)self + 0xe1e8);
    func_02077100((u8 *)self + 0xe1e0);
    func_020277d8((u8 *)self + 0xe168, 6, 0x14, (void *)func_020778a4);
    func_020277d8((u8 *)self + 0xe128, 4, 0x10, (void *)func_020330dc);
    func_020277d8((u8 *)self + 0x95c0, 0xfe, 0x4c, (void *)func_0205e90c);
    func_020350c0(self);
    _ZdlPv(self);
    return self;
}

// Starts a new game: initializes every part of the state once.
void func_02030fd4(GameData *self)
{
    AT(u8, 0x106b8) = 0;
    AT(u8, 0x106b9) = 0;
    if (self->initialized != 0) {
        return;
    }
    self->initialized = 1;
    func_020310fc(self);
    func_02031120(self);
    func_02031158(self);
    func_020311e4(self);
    func_0205a0d0(self);
    func_0205bfc0(self);
    func_020317f0(self);
    func_020318c8(self);
    func_02032288(self);
    func_02032470(self);
    func_02032510(self);
    func_020331fc(self);
    func_0203327c(self);
    func_020332a8(self);
    func_02033304(self);
    func_02033364(self);
    func_020333f4(self);
    func_02033484(self);
    func_02033ca4(self);
    func_02033e8c(self);
    func_02033fec(self);
    func_020340b0(self);
    func_020342ac(self);
    func_020343dc(self);
    func_02070968(self);
    func_020348cc(self);
    AT(u8, 0x10830) = 0;
    func_0203506c(self);
    AT(u32, 0x10838) = 0;
    AT(u8, 0x1083c) = 0;
    func_02034bc8(self);
}

void func_020310fc(void *self)
{
    GameDate date = {0, 0};

    func_020354c4(&date);

}

void func_02031120(void *self)
{
    GameTime time;

    time.minute = ((const GameTime *)data_020b1df0)->minute;
    time.hour = ((const GameTime *)data_020b1df0)->hour;

    func_02035190(&time);
    data_020f57d0 = 0;
}

// Rolls the weather of the first week.
#ifdef NONMATCHING
void func_02031158(void *obj)
{
    GameData *self = (GameData *)obj;
    GameDate date = *(const GameDate *)(data_020b1df0 + 4);
    int i;

    for (i = 0; i < 7; i++) {
        self->weather[i] = func_02035968(&date);
        date.day++;
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02031158(void *obj) {
    stmdb sp!, {r4, r5, r6, lr}
    sub sp, sp, #0x8
    ldr r1, [pc, #0x48]
    add r5, sp, #0x0
    ldr r2, [r1, #0x8]
    ldr r1, [r1, #0x4]
    mov r4, r0
    str r1, [sp, #0x0]
    str r2, [sp, #0x4]
    mov r6, #0x0
_02031180:
    mov r0, r5
    bl func_02035968
    add r1, r4, r6, lsl #0x2
    str r0, [r1, #0x18]
    ldrsb r0, [sp, #0x5]
    add r6, r6, #0x1
    cmp r6, #0x7
    add r0, r0, #0x1
    strb r0, [sp, #0x5]
    blt _02031180
    add sp, sp, #0x8
    ldmia sp!, {r4, r5, r6, pc}
    dcd data_020b1df0
}
#endif

// Weather of day `day` of the forecast (0 = today, -1 = yesterday).
u32 func_020311b4(GameData *self, int day)
{
    if (day == -1) {
        return self->weatherYesterday;
    }
    return self->weather[day];
}

void func_020311cc(GameData *self, int day, u32 weather)
{
    if (day == -1) {
        self->weatherYesterday = weather;
    } else {
        self->weather[day] = weather;
    }
}
}
