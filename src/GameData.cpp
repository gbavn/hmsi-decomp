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

void func_02031394(void *self, int value);
void func_020313a4(void *self, int value);
void func_020313d0(void *self, int value);
void func_020313e0(void *self, int value);
void func_0203140c(void *self, int value);
void func_0203142c(void *self, int value);
void func_0203144c(void *self, const char *name);
void func_0203149c(void *self, const char *name);
void func_0203508c(void *self, int value);
void func_02031484(void *self, s32 money);
void func_02031578(void *self, BOOL on);
void func_02031590(void *self, BOOL on);
void func_02035524(void *date);
const void *func_020351b0(void);
u32 func_02021d48(const char *s);
int func_0203366c(void *self, int id);
void func_02034c94(void *self, void *out, int a, int b, int c, int d, int e, int f);

extern void *data_020bc8bc[];
extern const char data_020b2718[];
extern const char data_020b2720[];
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

// A position-like triple, zeroed by its constructor.
struct ZeroVec {
    ZeroVec() : x(0), y(0), z(0) {}

    u32 x;
    u32 y;
    u32 z;
};

// The current date as returned by func_02035524.
struct GameDay {
    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
};

// A time of day zeroed by its constructor.
struct ZeroTime {
    ZeroTime() : hour(0), minute(0) {}

    s8 hour;
    s8 minute;
};

struct PlayerSlot {
    s16 a;
    u8 unk2[4];
    s16 b;
    u8 unk8[4];
};

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
// Starts the player's state (stats, names, money, flags, date/time markers).
#ifdef NONMATCHING
void func_020311e4(void *obj)
{
    GameData *self = (GameData *)obj;
    int i;

    func_02031394(self, 100);
    func_0203140c(self, 0);
    func_020313a4(self, 100);
    func_020313d0(self, 100);
    func_0203142c(self, 0);
    func_020313e0(self, 100);
    func_0203144c(self, data_020b2718);
    func_0203149c(self, data_020b2720);
    func_0203508c(self, 500);
    AT(u8, 0x106b8) = 0;
    func_02031484(self, 0);
    AT(u32, 0x90) = 0;
    func_02031578(self, TRUE);
    func_02031590(self, FALSE);
    {
        ZeroVec pos;

        AT(ZeroVec, 0x38) = pos;
    }
    AT(s32, 0x44) = -1;
    AT(u32, 0x98) = 0x3f800000;
    AT(u32, 0x9c) = 0x3f800000;
    AT(u32, 0xa4) = 0;
    for (i = 0; i < 2; i++) {
        (&AT(PlayerSlot, 0xae))[i].a = -1;
        (&AT(PlayerSlot, 0xae))[i].b = 0;
    }
    AT(u8, 0xa8) = 0;
    AT(u8, 0x106b8) = 0;
    AT(u8, 0x106b9) = 0;
    {
        GameDay today;
        ZeroTime time;

        func_02035524(&today);
        AT(GameDay, 0xd4) = today;
        AT(u32, 0xc4) = 0x3f000000;
        AT(ZeroTime, 0xc8) = time;
        AT(u32, 0xcc) = 0x3f000000;
        time = *(const ZeroTime *)func_020351b0();
        time.hour += 2;
        AT(ZeroTime, 0xd0) = time;
    }
    AT(u8, 0xd2) = 0;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_020311e4(void *obj) {
    stmdb sp!, {r4, lr}
    sub sp, sp, #0x18
    mov r4, r0
    mov r1, #0x64
    bl func_02031394
    mov r0, r4
    mov r1, #0x0
    bl func_0203140c
    mov r0, r4
    mov r1, #0x64
    bl func_020313a4
    mov r0, r4
    mov r1, #0x64
    bl func_020313d0
    mov r0, r4
    mov r1, #0x0
    bl func_0203142c
    mov r0, r4
    mov r1, #0x64
    bl func_020313e0
    ldr r1, [pc, #0x150]
    mov r0, r4
    bl func_0203144c
    ldr r1, [pc, #0x148]
    mov r0, r4
    bl func_0203149c
    mov r0, r4
    mov r1, #0x1f4
    bl func_0203508c
    mov r1, #0x0
    add r0, r4, #0x10000
    strb r1, [r0, #0x6b8]
    mov r0, r4
    bl func_02031484
    mov r0, #0x0
    str r0, [r4, #0x90]
    mov r0, r4
    mov r1, #0x1
    bl func_02031578
    mov r0, r4
    mov r1, #0x0
    bl func_02031590
    mov r3, #0x0
    add r0, sp, #0xc
    str r3, [r0, #0x0]
    str r3, [r0, #0x4]
    str r3, [r0, #0x8]
    str r3, [r4, #0x38]
    ldr r0, [sp, #0x10]
    sub r1, r3, #0x1
    str r0, [r4, #0x3c]
    ldr r2, [sp, #0x14]
    mov r0, #0x3f800000
    str r2, [r4, #0x40]
    str r1, [r4, #0x44]
    str r0, [r4, #0x98]
    str r0, [r4, #0x9c]
    str r3, [r4, #0xa4]
    mov r0, r4
    mov r2, r3
_020312d4:
    strh r1, [r0, #0xae]
    add r3, r3, #0x1
    strh r2, [r0, #0xb4]
    cmp r3, #0x2
    add r0, r0, #0xc
    blt _020312d4
    strb r2, [r4, #0xa8]
    add r1, r4, #0x10000
    strb r2, [r1, #0x6b8]
    add r0, sp, #0x4
    strb r2, [r1, #0x6b9]
    bl func_02035524
    ldr r0, [sp, #0x4]
    mov r3, #0x3f000000
    str r0, [r4, #0xd4]
    ldrsb r0, [sp, #0x8]
    add r2, sp, #0x0
    mov r1, #0x0
    strb r0, [r4, #0xd8]
    ldrsb r0, [sp, #0x9]
    strb r0, [r4, #0xd9]
    ldrsb r0, [sp, #0xa]
    strb r0, [r4, #0xda]
    str r3, [r4, #0xc4]
    strb r1, [r2, #0x0]
    ldrsb r0, [sp, #0x0]
    strb r1, [r2, #0x1]
    strb r0, [r4, #0xc8]
    ldrsb r0, [sp, #0x1]
    strb r0, [r4, #0xc9]
    str r3, [r4, #0xcc]
    bl func_020351b0
    ldrsb r3, [r0, #0x0]
    mov r1, #0x0
    strb r3, [sp, #0x0]
    ldrsb r2, [r0, #0x1]
    add r0, r3, #0x2
    strb r0, [sp, #0x0]
    ldrsb r0, [sp, #0x0]
    strb r2, [sp, #0x1]
    strb r0, [r4, #0xd0]
    ldrsb r0, [sp, #0x1]
    strb r0, [r4, #0xd1]
    strb r1, [r4, #0xd2]
    add sp, sp, #0x18
    ldmia sp!, {r4, pc}
    dcd data_020b2718
    dcd data_020b2720
}
#endif

void func_02031394(void *self, int value)
{
    if (value > 250) {
        value = 250;
    }
    AT(s16, 0x56) = value;
}

void func_020313a4(void *self, int value)
{
    if (value < 0) {
        value = 0;
    } else if (AT(s16, 0x56) + AT(s16, 0x5e) < value) {
        value = (s16)(AT(s16, 0x56) + AT(s16, 0x5e));
    }
    AT(s16, 0x58) = value;
}

void func_020313d0(void *self, int value)
{
    if (value > 250) {
        value = 250;
    }
    AT(s16, 0x5a) = value;
}

void func_020313e0(void *self, int value)
{
    if (value < 0) {
        value = 0;
    } else if (AT(s16, 0x5a) + AT(s16, 0x60) < value) {
        value = (s16)(AT(s16, 0x5a) + AT(s16, 0x60));
    }
    AT(s16, 0x5c) = value;
}

void func_0203140c(void *self, int value)
{
    if (value + AT(s16, 0x56) > 250) {
        value = (s16)(250 - AT(s16, 0x56));
    }
    AT(s16, 0x5e) = value;
}

void func_0203142c(void *self, int value)
{
    if (value + AT(s16, 0x5a) > 250) {
        value = (s16)(250 - AT(s16, 0x5a));
    }
    AT(s16, 0x60) = value;
}

void func_0203144c(void *self, const char *name)
{
    MI_CpuFill8(&AT(u8, 0x48), 0, 13);
    MI_CpuCopy8(name, &AT(u8, 0x48), func_02021d48(name));
}

void func_02031484(void *self, s32 money)
{
    if (money > 1000000000) {
        money = 1000000000;
    }
    AT(s32, 0x68) = money;
}

void func_0203149c(void *self, const char *name)
{
    MI_CpuFill8(&AT(u8, 0x6c), 0, 13);
    MI_CpuCopy8(name, &AT(u8, 0x6c), func_02021d48(name));
}

void func_020314d4(void *self)
{
    int a = func_0203366c(self, 0x331);
    int b = func_0203366c(self, 0x332);
    int c = func_0203366c(self, 0x333);
    int d = func_0203366c(self, 0x334);
    int e = func_0203366c(self, 0x335);
    int f = func_0203366c(self, 0x336);

    func_02034c94(self, &AT(u8, 0x79), a, b, c, d, e, f);
}

void func_02031578(void *self, BOOL on)
{
    if (on) {
        AT(s8, 0x94) |= 1;
    } else {
        AT(s8, 0x94) &= ~1;
    }
}

void func_02031590(void *self, BOOL on)
{
    if (on) {
        AT(s8, 0x94) |= 2;
    } else {
        AT(s8, 0x94) &= ~2;
    }
}
}
