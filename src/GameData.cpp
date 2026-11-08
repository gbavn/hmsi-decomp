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

void func_02031840(void *self, const char *name);
void func_0203375c(void *self, int id, int value);
extern const u32 data_020b1e04[];

BOOL func_02031f7c(void *self, int i);
int func_02080c9c(void *obj, int i);
extern void *data_020f57c4;

BOOL func_0203f1f0(void *obj, int i);
extern const s32 data_020bc874;
extern const s32 data_020bc878;
extern const s32 data_020bc87c;
extern void *gResidents;
const s32 *func_02031a60(const s32 *a, const s32 *b);
const s32 *func_02031a74(const s32 *a, const s32 *b);

extern void *data_020bc8bc[];
extern const char data_020b2718[];
extern const char data_020b2720[];
extern const u8 data_020b1df0[];
extern const u8 data_020b1dfc[];        // start time (2 bytes), then the start date (8 bytes)
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

typedef struct Name7 {
    char c[7];
} Name7;

// One of the 137 entries at +0x8a78 (12 bytes).
typedef struct ItemRecordBits {
    s32 a : 7;
    u32 b : 1;
    u32 c : 1;
    u32 d : 1;
    s32 e : 6;
    u32 f : 1;
    s32 g : 7;
    s32 h : 4;
    u32 i : 1;
    u32 j : 3;
} ItemRecordBits;

typedef struct ItemRecordBits2 {
    s32 k : 6;
    s32 l : 26;
} ItemRecordBits2;

typedef struct ItemRecord {
    u8 unk0;
    u16 unk2;
    ItemRecordBits bits;
    ItemRecordBits2 bits2;
} ItemRecord;

typedef struct GameData {
    void **vt;
    u8 unk4;
    s8 initialized;
    u8 unk6[0x18 - 6];
    u32 weather[7];         // forecast: [0] = today
    u32 weatherYesterday;
    u8 unk38[0x8a68 - 0x38];
    char ranchName[13];
    u8 unk8a75[3];
    ItemRecord records[0x89];
} GameData;

#define RECORDS(self) (((GameData *)(self))->records)

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
// Category of item `item` (0x1fd..0x25c), -1 for the others.
int func_020315a8(void *self, int item)
{
    int category = -1;

    switch (item) {
    case 0x1fd:
    case 0x1fe:
        category = 5;
        break;
    case 0x227:
    case 0x228:
    case 0x229:
    case 0x22a:
    case 0x22b:
        category = 6;
        break;
    case 0x246:
    case 0x247:
        category = 15;
        break;
    case 0x203:
    case 0x204:
    case 0x205:
    case 0x206:
        category = 1;
        break;
    case 0x1ff:
    case 0x200:
    case 0x201:
    case 0x202:
        category = 9;
        break;
    case 0x23b:
    case 0x23c:
    case 0x23d:
        category = 15;
        break;
    case 0x207:
    case 0x208:
    case 0x209:
    case 0x20a:
        category = 0;
        break;
    case 0x230:
    case 0x231:
    case 0x232:
        category = 10;
        break;
    case 0x21a:
    case 0x21b:
    case 0x21c:
    case 0x21d:
        category = 2;
        break;
    case 0x221:
    case 0x222:
    case 0x223:
        category = 7;
        break;
    case 0x23e:
    case 0x23f:
    case 0x240:
        category = 16;
        break;
    case 0x22d:
    case 0x22e:
    case 0x22f:
        category = 11;
        break;
    case 0x20b:
    case 0x20c:
    case 0x20d:
    case 0x20e:
        category = 3;
        break;
    case 0x235:
    case 0x236:
    case 0x237:
        category = 12;
        break;
    case 0x241:
    case 0x242:
    case 0x243:
        category = 17;
        break;
    case 0x238:
    case 0x239:
        category = 8;
        break;
    case 0x216:
    case 0x217:
    case 0x218:
    case 0x219:
        category = 4;
        break;
    case 0x248:
    case 0x249:
        category = 18;
        break;
    case 0x224:
    case 0x225:
    case 0x226:
        category = 13;
        break;
    case 0x25b:
    case 0x25c:
        category = 14;
        break;
    case 0x214:
    case 0x215:
        category = 19;
        break;
    case 0x233:
    case 0x234:
        category = 20;
        break;
    }
    return category;
}

void func_020317f0(void *self)
{
    Name7 name = *(const Name7 *)data_020b1dfc;

    func_02031840(self, name.c);
}

#ifdef NONMATCHING
void func_02031840(void *self, const char *name)
{
    MI_CpuFill8(((GameData *)self)->ranchName, 0, 13);
    MI_CpuCopy8(name, ((GameData *)self)->ranchName, func_02021d48(name));
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02031840(void *self, const char *name) {
    stmdb sp!, {r3, r4, r5, lr}
    mov r5, r0
    add r0, r5, #0x268
    mov r4, r1
    add r0, r0, #0x8800
    mov r1, #0x0
    mov r2, #0xd
    bl MI_CpuFill8
    mov r0, r4
    bl func_02021d48
    add r1, r5, #0x268
    mov r2, r0
    mov r0, r4
    add r1, r1, #0x8800
    bl MI_CpuCopy8
    ldmia sp!, {r3, r4, r5, pc}
}
#endif

s16 func_02031880(void *self)
{
    return func_0203366c(self, 0x321);
}

void func_0203189c(void *self)
{
    func_0203375c(self, 0x321, 1);
}

int func_020318b4(void *self)
{
    return func_0203366c(self, 0x24a);
}

#ifdef NONMATCHING
void func_020318c8(void *self)
{
    ItemRecord *records = &AT(ItemRecord, 0x8a78);
    int i;

    for (i = 0; i < 0x89; i++) {
        records[i].unk0 = 0;
        records[i].unk2 = 0;
        records[i].bits.a = 0;
        records[i].bits.b = 0;
        records[i].bits.c = 0;
        records[i].bits.d = 0;
        records[i].bits.e = 10;
        records[i].bits.f = 0;
        records[i].bits.g = 0;
        records[i].bits.h = 0;
        records[i].bits.i = 0;
        records[i].bits2.k = 0;
        records[i].bits2.l = -1;
    }
    for (i = 0; i < 0x18; i++) {
        records[data_020b1e04[i]].bits.h = 1;
        records[data_020b1e04[i]].bits2.k = 1;
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_020318c8(void *self) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, lr}
    add r1, r0, #0x27c
    add r2, r0, #0xa80
    add ip, r1, #0x8800
    mov r1, #0x0
    add lr, r2, #0x8000
    mov r3, r0
    mov r2, r1
    mvn r6, #0x3f
_020318ec:
    add r4, r3, #0x8000
    add r5, r3, #0x8a00
    strb r2, [r4, #0xa78]
    strh r2, [r5, #0x7a]
    ldr r4, [ip, #0x0]
    add r1, r1, #0x1
    bic r7, r4, #0x7f
    bic r10, r7, #0x80
    bic r9, r10, #0x100
    bic r8, r9, #0x200
    bic r4, r8, #0xfc00
    orr r5, r4, #0x2800
    bic r7, r5, #0x10000
    bic r4, r7, #0xfe0000
    bic r5, r4, #0xf000000
    bic r4, r5, #0x10000000
    str r5, [ip, #0x0]
    str r4, [ip], #0xc
    ldr r4, [lr, #0x0]
    cmp r1, #0x89
    bic r5, r4, #0x3f
    and r4, r5, #0x3f
    orr r4, r4, r6
    str r5, [lr, #0x0]
    str r4, [lr], #0xc
    add r3, r3, #0xc
    blt _020318ec
    add r1, r0, #0x27c
    add r0, r0, #0xa80
    add r3, r0, #0x8000
    ldr r5, [pc, #0x3c]
    add r4, r1, #0x8800
    mov r0, #0xc
_02031970:
    ldr r1, [r5, r2, lsl #0x2]
    add r2, r2, #0x1
    mul r6, r1, r0
    ldr r1, [r4, r6]
    cmp r2, #0x18
    bic r1, r1, #0xf000000
    orr r1, r1, #0x1000000
    str r1, [r4, r6]
    ldr r1, [r3, r6]
    bic r1, r1, #0x3f
    orr r1, r1, #0x1
    str r1, [r3, r6]
    blo _02031970
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, pc}
    dcd data_020b1e04
}
#endif
// Adds `amount` (scaled by the multiplier at +0x98) to record `i`'s value, clamped to 0..0xffff.
#ifdef NONMATCHING
void func_020319ac(void *self, int i, int amount)
{
    s32 value;

    if (func_02031f7c(self, i)) {
        return;
    }
    value = RECORDS(self)[i].unk2 + (s32)(amount * AT(f32, 0x98));
    value = *func_02031a60(&value, &data_020bc874);
    if (func_0203f1f0(gResidents, i)) {
        value = *func_02031a74(&value, &data_020bc878);
    } else {
        value = *func_02031a74(&value, &data_020bc87c);
    }
    RECORDS(self)[i].unk2 = value;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN9Residents10IsResidentEi(void);
void _ffix(void);
void _fflt(void);
void _fmul(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void func_020319ac(void *self, int i, int amount) {
    stmdb sp!, {r3, r4, r5, r6, r7, lr}
    mov r7, r0
    mov r4, r1
    mov r6, r2
    bl func_02031f7c
    cmp r0, #0x0
    ldmneia sp!, {r3, r4, r5, r6, r7, pc}
    mov r0, #0xc
    mul r5, r4, r0
    add r1, r7, #0x7a
    mov r0, r6
    ldr r7, [r7, #0x98]
    add r6, r1, #0x8a00
    bl _fflt
    mov r1, r7
    bl _fmul
    bl _ffix
    ldrh r2, [r6, r5]
    ldr r1, [pc, #0x54]
    add r2, r2, r0
    add r0, sp, #0x0
    str r2, [sp, #0x0]
    bl func_02031a60
    ldr r2, [r0, #0x0]
    ldr r0, [pc, #0x40]
    mov r1, r4
    ldr r0, [r0, #0x0]
    str r2, [sp, #0x0]
    bl _ZN9Residents10IsResidentEi
    cmp r0, #0x0
    add r0, sp, #0x0
    beq _02031a38
    ldr r1, [pc, #0x24]
    bl func_02031a74
    b _02031a40
_02031a38:
    ldr r1, [pc, #0x1c]
    bl func_02031a74
_02031a40:
    ldr r0, [r0, #0x0]
    str r0, [sp, #0x0]
    strh r0, [r6, r5]
    ldmia sp!, {r3, r4, r5, r6, r7, pc}
    dcd data_020bc874
    dcd gResidents
    dcd data_020bc878
    dcd data_020bc87c
}
#endif

const s32 *func_02031a60(const s32 *a, const s32 *b)
{
    return *a > *b ? a : b;
}

const s32 *func_02031a74(const s32 *a, const s32 *b)
{
    return *a < *b ? a : b;
}

void func_02031a88(void *self, int i, u16 value)
{
    if (func_02031f7c(self, i)) {
        return;
    }
    RECORDS(self)[i].unk2 = value;
}

u16 func_02031ab8(void *self, int i)
{
    if (i == 0xd7) {
        i = func_0203366c(self, 4) == 0 ? 0x1c : 0x1d;
    }
    return RECORDS(self)[i].unk2;
}

void func_02031af0(void *self, int i, int value)
{
    RECORDS(self)[i].bits.h = value;
}

int func_02031b18(void *self, int i)
{
    return RECORDS(self)[i].bits.h;
}

int func_02031b34(void)
{
    void *obj = data_020f57c4;
    int sum = 0;
    int i;

    for (i = 0; i < 7; i++) {
        sum += func_02080c9c(obj, i);
    }
    return sum;
}

void func_02031b70(void *self, int i, int value)
{
    RECORDS(self)[i].bits.a = value;
}

void func_02031b98(void *self, int i)
{
    if (RECORDS(self)[i].bits.a < 30) {
        RECORDS(self)[i].bits.a++;
    }
}

int func_02031bd4(void *self, int i)
{
    return RECORDS(self)[i].bits.a;
}

void func_02031bf0(void *self, int i, BOOL value)
{
    RECORDS(self)[i].bits.b = value;
}

BOOL func_02031c18(void *self, int i)
{
    return RECORDS(self)[i].bits.b == 1;
}

void func_02031c40(void *self, int i, BOOL value)
{
    RECORDS(self)[i].bits.c = value;
}

BOOL func_02031c68(void *self, int i)
{
    return RECORDS(self)[i].bits.c == 1;
}

void func_02031c90(void *self, int i, BOOL value)
{
    RECORDS(self)[i].bits.d = value;
}

BOOL func_02031cb8(void *self, int i)
{
    return RECORDS(self)[i].bits.d == 1;
}

void func_02031ce0(void *self, int i, int value)
{
    RECORDS(self)[i].bits.e = value;
    RECORDS(self)[i].bits.e = RECORDS(self)[i].bits.e <= 0 ? 0 : RECORDS(self)[i].bits.e;
    RECORDS(self)[i].bits.e = RECORDS(self)[i].bits.e >= 10 ? 10 : RECORDS(self)[i].bits.e;
}

int func_02031d50(void *self, int i)
{
    return RECORDS(self)[i].bits.e;
}

void func_02031d6c(void *self, int i, int value)
{
    RECORDS(self)[i].bits.g = value;
    RECORDS(self)[i].bits.g = RECORDS(self)[i].bits.g <= 0 ? 0 : RECORDS(self)[i].bits.g;
    RECORDS(self)[i].bits.g = RECORDS(self)[i].bits.g >= 30 ? 30 : RECORDS(self)[i].bits.g;
}

int func_02031ddc(void *self, int i)
{
    return RECORDS(self)[i].bits.g;
}

void func_02031df8(void *self, int i, int value)
{
    RECORDS(self)[i].bits2.k = value;
}

void func_02031e20(void *self, int i)
{
    RECORDS(self)[i].bits2.k = RECORDS(self)[i].bits2.k + 1;
    RECORDS(self)[i].bits2.k = RECORDS(self)[i].bits2.k >= 16 ? 16 : RECORDS(self)[i].bits2.k;
}

int func_02031e78(void *self, int i)
{
    return RECORDS(self)[i].bits2.k;
}

void func_02031e94(void *self, int i, BOOL value)
{
    RECORDS(self)[i].bits.f = value;
}

BOOL func_02031ebc(void *self, int i)
{
    return RECORDS(self)[i].bits.f == 1;
}

void func_02031ee4(void *self, int i, BOOL value)
{
    RECORDS(self)[i].bits.i = value;
}

BOOL func_02031f0c(void *self, int i)
{
    if (i == 0x25) {
        return FALSE;
    }
    return RECORDS(self)[i].bits.i == 1;
}

void func_02031f40(void *self, int i, int value)
{
    RECORDS(self)[i].bits2.l = value;
}

int func_02031f64(void *self, int i)
{
    return RECORDS(self)[i].bits2.l;
}
}
