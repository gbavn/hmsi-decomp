// The game state / save data (ARM9 0x02030cfc..): one big object (pointer at 0x020f57c0) with the
// calendar, the weather forecast, the flags and everything else that is saved. Names are ours.
//
// The original is a single C++ file with ~190 functions; it is decompiled in pieces (this unit covers
// 0x02030cfc..0x020311e4). Its .data/.rodata/.bss stay in assembly for now and are used by symbol.
#include <nitro.h>
#include <string.h>

u32 RandRange(u32 min, u32 max);    // 0x020358ec

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
void *func_020330c8(void *self);
void func_020330dc(void *self);
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

int func_02031f7c(void *self, int i);
void func_02081280(void *obj);
BOOL func_02070180(void *obj);
void func_02070174(void *obj);
void func_02090f28(void *item);
void func_02082e34(void *obj, void *item);
void func_02083b28(void *obj, int id, int count);
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

typedef struct Entry16 {
    u8 b[16];
} Entry16;

typedef struct Entry8 {
    u8 b[8];
} Entry8;

typedef struct GameBits9384 {
    u8 b0 : 1;
    u8 b1 : 1;
    u8 b2 : 1;
} GameBits9384;

// 12-byte item/shipping entry (constructor func_02090f28).
typedef struct Bytes4 {
    u8 b[4];
} Bytes4;

typedef struct Bytes2 {
    u8 b[2];
} Bytes2;

typedef struct Item12 {
    Bytes4 head;
    s16 id;
    Bytes2 fg;
    u8 h;
    u8 i;
    u16 count;
} Item12;

// The 12 objects at +0xe290 (16 bytes; ctor func_0207005c).
typedef struct Obj16 {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
    u8 unk4;
    u8 flag;
    u8 unk6[10];
} Obj16;

// The 0xfe animals (0x4c bytes; ctor func_0205e898). Slots 0..0x1b are the farm animals.
typedef struct Animal {
    u8 id;
    u8 unk1[0xd];
    u8 kind;
    u8 unkf[0x43 - 0xf];
    u8 b43;
    u8 unk44[8];
} Animal;

// Four animal slot numbers (-1 = empty).
typedef struct AnimalSlots {
    s32 slot[4];
} AnimalSlots;

typedef struct Obj20 {
    u8 unk0[6];
    s8 b6;
    u8 unk7[0xd];
} Obj20;

typedef struct Obj10 {
    u32 a;
    u32 b;
    u8 c;
    u8 d;
} Obj10;

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
    Entry16 unk90e4[42];
    GameBits9384 bits9384;
    u8 unk9385[3];
    Entry8 unk9388[2];
    Entry8 unk9398[4];
    Entry8 unk93b8[5];
    Item12 items[40];
    Animal animals[0xfe];
    AnimalSlots unke128[4];
    Obj20 unke168[6];
    u8 unke1e0[8];
    u8 unke1e8[0x98];
    s32 unke280;
    Obj10 unke284;
    Obj16 objs[12];
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

// 1 if `i` is the current something (flag 0x240); 2 if its pair's state (flags 0x2ba..0x2be) is 3.
int func_02031f7c(void *self, int i)
{
    if (i == func_0203366c(self, 0x240)) {
        return 1;
    }
    switch (i) {
    case 4:
    case 10:
        if (func_0203366c(self, 0x2bb) == 3) {
            return 2;
        }
        break;
    case 2:
    case 9:
        if (func_0203366c(self, 0x2ba) == 3) {
            return 2;
        }
        break;
    case 3:
    case 11:
        if (func_0203366c(self, 0x2bc) == 3) {
            return 2;
        }
        break;
    case 1:
    case 12:
        if (func_0203366c(self, 0x2bd) == 3) {
            return 2;
        }
        break;
    case 5:
    case 13:
        if (func_0203366c(self, 0x2be) == 3) {
            return 2;
        }
        break;
    }
    return 0;
}

int func_02032080(void *self, int i)
{
    if (func_0203366c(self, 2) != 0 && i == func_0203366c(self, 0x242)) {
        return 1;
    }
    switch (i) {
    case 4:
    case 10:
        if (func_0203366c(self, 0x2bb) == 4) {
            return 2;
        }
        break;
    case 2:
    case 9:
        if (func_0203366c(self, 0x2ba) == 4) {
            return 2;
        }
        break;
    case 3:
    case 11:
        if (func_0203366c(self, 0x2bc) == 4) {
            return 2;
        }
        break;
    case 1:
    case 12:
        if (func_0203366c(self, 0x2bd) == 4) {
            return 2;
        }
        break;
    case 5:
    case 13:
        if (func_0203366c(self, 0x2be) == 4) {
            return 2;
        }
        break;
    }
    return 0;
}

BOOL func_0203219c(void *self, int i)
{
    if (i == 0) {
        if (func_0203366c(self, 0xca) == 1 && func_0203366c(self, 0xcb) == 0) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL func_020321e4(void *self)
{
    int i;

    for (i = 1; i < 0x22; i++) {
        if (i == 0x20 || i == 0x21 || i == 0x1f || i == 0x1e || i == 0x1c || i == 0x1d || i == 6 || i == 0xe) {
            continue;
        }
        if (func_02031b18(self, i) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL func_02032240(void *self)
{
    int i;

    for (i = 0x22; i < 0x88; i++) {
        if (i >= 0x22 && i <= 0x28) {
            continue;
        }
        if (func_02031b18(self, i) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

void func_02032288(void *self)
{
    func_02081280(data_020f57c4);
    ((GameData *)self)->bits9384.b1 = 0;
    ((GameData *)self)->bits9384.b1 = 0;
    ((GameData *)self)->bits9384.b2 = 0;
}

Entry16 *func_020322c8(void *self, int i)
{
    return &((GameData *)self)->unk90e4[i];
}

void func_020322d8(void *self)
{
    u32 i;
    Obj16 *obj = (Obj16 *)&AT(u8, 0xe290);

    for (i = 0; i < 12; i++, obj++) {
        if (func_02070180(obj) == 0) {
            Item12 item;

            func_02090f28(&item);
            item.id = obj->flag != 0 ? 0x52 : 0x51;
            item.count = 1;
            item.fg.b[0] = obj->b;
            item.fg.b[1] = obj->c;
            item.h = obj->d;
            func_02082e34(data_020f57c4, &item);
            func_02070174(obj);
            func_02083b28(data_020f57c4, item.id, 1);
        }
    }
}

void func_0203238c(void *self, int on)
{
    ((GameData *)self)->bits9384.b0 = (u8)on;
}

void func_020323ac(void *self, int on)
{
    ((GameData *)self)->bits9384.b1 = (u8)on;
}

void func_020323cc(void *self, int on)
{
    ((GameData *)self)->bits9384.b2 = (u8)on;
}

bool func_020323ec(void *self)
{
    return ((GameData *)self)->bits9384.b0;
}

bool func_02032408(void *self)
{
    return ((GameData *)self)->bits9384.b1;
}

bool func_02032424(void *self)
{
    return ((GameData *)self)->bits9384.b2;
}

Entry8 *func_02032440(void *self, int i)
{
    return &((GameData *)self)->unk9388[i];
}

Entry8 *func_02032450(void *self, int i)
{
    return &((GameData *)self)->unk9398[i];
}

Entry8 *func_02032460(void *self, int i)
{
    return &((GameData *)self)->unk93b8[i];
}

void func_02032470(void *self)
{
    int i;

    for (i = 0; i < 40; i++) {
        Item12 item;

        func_02090f28(&item);
        ((GameData *)self)->items[i] = item;
    }
}

Item12 *func_020324fc(void *self, int i)
{
    return &((GameData *)self)->items[i];
}

void func_0205e910(void *a, const char *name, int kind, int arg);
void func_0205f91c(void *a, int v);
void func_0205f998(void *a, int v);
void func_0205f9e4(void *a, int v);
void func_0205f930(void *a, int v);
void func_0205f938(void *a, int v);
void func_0205fcf0(void *a, int v);
BOOL func_0205ec34(void *a);
void func_02060798(void *a);
void func_0205eab0(void *a);
void func_0205ff98(void);
void func_02083ab4(void *obj);
void func_02083be0(void *obj);
BOOL func_020837e4(void *obj);
void func_02082820(void *obj);
void func_02082984(void *obj);
void func_02082ad8(void *obj);
int func_02044180(int a, int b);
char *func_02054bac(void *obj, int file, int id);
void func_020778a8(void *obj, int i);
void func_020a01f0(void *obj);
int func_02032fd8(void *obj, int kind);
int func_02032e20(void *obj, int kind);
int func_020330b8(void *self, int i);
void func_02033130(AnimalSlots *self);
void func_020331d0(AnimalSlots *self, int i, int slot);
void func_020331d8(AnimalSlots *self, int i);
BOOL func_020331e4(AnimalSlots *self, int i);
extern void *data_020f57b8;
extern GameData *data_020f57c0;
extern const s32 data_020b1e64[];
extern const char data_020bc8d4[];
extern const char data_020bc8dc[];
extern const char data_020bc8e4[];
extern const char data_020bc8ec[];
extern const char data_020bc8f4[];
extern const char data_020bc8fc[];

// Resets the animals of slots 0x1c..0xd5 (six groups of kinds 0x14..0x19).
#ifdef NONMATCHING
void func_02032510(void *obj)
{
    GameData *self = (GameData *)obj;
    u32 i;

    for (i = 0x1c; i < 0x3b; i++) {
        func_0205e910(&self->animals[i], data_020bc8d4, 0x14, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
    for (i = 0x3b; i < 0x5a; i++) {
        func_0205e910(&self->animals[i], data_020bc8dc, 0x15, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
    for (i = 0x5a; i < 0x79; i++) {
        func_0205e910(&self->animals[i], data_020bc8e4, 0x16, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
    for (i = 0x79; i < 0x98; i++) {
        func_0205e910(&self->animals[i], data_020bc8ec, 0x17, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
    for (i = 0x98; i < 0xb7; i++) {
        func_0205e910(&self->animals[i], data_020bc8f4, 0x18, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
    for (i = 0xb7; i < 0xd6; i++) {
        func_0205e910(&self->animals[i], data_020bc8fc, 0x19, 0);
        func_0205f91c(&self->animals[i], -1);
        func_0205f998(&self->animals[i], 0);
        func_0205f9e4(&self->animals[i], 0);
        self->animals[i].b43 = 0;
        func_0205f930(&self->animals[i], 0);
        func_0205f938(&self->animals[i], 0);
        func_0205fcf0(&self->animals[i], 10);
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02032510(void *obj) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r6, #0x0
    str r0, [sp, #0x0]
    add r0, r0, #0xe10
    ldr r8, [pc, #0x398]
    add r10, r0, #0x9000
    mov r9, #0x1c
    mov r7, #0x14
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_0203253c:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0x3b
    blo _0203253c
    ldr r0, [sp, #0x0]
    mov r6, #0x0
    add r0, r0, #0x344
    ldr r8, [pc, #0x300]
    add r10, r0, #0xa400
    mov r9, #0x3b
    mov r7, #0x15
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_020325d8:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0x5a
    blo _020325d8
    ldr r0, [sp, #0x0]
    mov r6, #0x0
    add r0, r0, #0x78
    ldr r8, [pc, #0x268]
    add r10, r0, #0xb000
    mov r9, #0x5a
    mov r7, #0x16
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_02032674:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0x79
    blo _02032674
    ldr r0, [sp, #0x0]
    mov r6, #0x0
    add r0, r0, #0x1ac
    ldr r8, [pc, #0x1d0]
    add r10, r0, #0xb800
    mov r9, #0x79
    mov r7, #0x17
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_02032710:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0x98
    blo _02032710
    ldr r0, [sp, #0x0]
    mov r6, #0x0
    add r0, r0, #0x2e0
    ldr r8, [pc, #0x138]
    add r10, r0, #0xc000
    mov r9, #0x98
    mov r7, #0x18
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_020327ac:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0xb7
    blo _020327ac
    ldr r0, [sp, #0x0]
    mov r6, #0x0
    add r0, r0, #0x14
    ldr r8, [pc, #0xa0]
    add r10, r0, #0xcc00
    mov r9, #0xb7
    mov r7, #0x19
    mvn r5, #0x0
    mov r4, r6
    mov r11, r6
_02032848:
    mov r0, r10
    mov r1, r8
    mov r2, r7
    mov r3, r6
    bl func_0205e910
    mov r0, r10
    mov r1, r5
    bl func_0205f91c
    mov r0, r10
    mov r1, r4
    bl func_0205f998
    mov r0, r10
    mov r1, r11
    bl func_0205f9e4
    mov r0, #0x0
    strb r0, [r10, #0x43]
    mov r0, r10
    mov r1, #0x0
    bl func_0205f930
    mov r0, r10
    mov r1, #0x0
    bl func_0205f938
    mov r0, r10
    mov r1, #0xa
    bl func_0205fcf0
    add r9, r9, #0x1
    add r10, r10, #0x4c
    cmp r9, #0xd6
    blo _02032848
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_020bc8d4
    dcd data_020bc8dc
    dcd data_020bc8e4
    dcd data_020bc8ec
    dcd data_020bc8f4
    dcd data_020bc8fc
}
#endif

// Puts a new animal in the first free slot of 0..0x1b; returns the slot or -1.
int func_020328d8(void *obj, const char *name, int kind, int arg)
{
    GameData *self = (GameData *)obj;
    u32 i;

    for (i = 0; i < 0x1c; i++) {
        if (func_0205ec34(&self->animals[i])) {
            func_0205e910(&self->animals[i], name, kind, arg);
            return i;
        }
    }
    return -1;
}

#define RAND_FX32(f) ((fx32)(((f) > 0) ? (f) * 4096.0f + 0.5f : (f) * 4096.0f - 0.5f))

// Fills the slots 0xd6..0xfd with animals of kind `kind` (9 or 0x13) up to the count given by a record.
#ifdef NONMATCHING
void func_02032948(void *obj, int kind)
{
    GameData *self = (GameData *)obj;
    int max;
    int speed;
    int count;
    int add;
    int n;
    int i;
    int k;
    u8 flag;

    if (kind == 9) {
        if (func_0203366c(self, 0x1a8) == 0) {
            return;
        }
        max = func_02044180(func_02031ab8(self, 0x65), 0xffff);
        speed = 0x28;
    } else {
        if (func_0203366c(self, 0x1a7) == 0) {
            return;
        }
        max = func_02044180(func_02031ab8(self, 0x5e), 0xffff);
        speed = 0x2a;
    }
    flag = self->animals[func_02032fd8(self, kind)].b43 == 0 ? 1 : 0;
    count = 0;
    if (max == 0) {
        max = 1;
    }
    for (i = 0xd6; i < 0xfe; i++) {
        if (kind == self->animals[i].kind) {
            count++;
        }
    }
    if (count == max) {
        return;
    }
    add = max - count;
    for (n = 0; n < add; n++) {
        k = 0;
        for (i = 0xd6; i < 0xfe; i++) {
            Animal *a = &self->animals[i];

            if (kind == a->kind) {
                k++;
                continue;
            }
            if (func_0205ec34(a)) {
                if (kind == 9) {
                    char *name = func_02054bac(data_020f57b8, 0xaa, k + 0x65);

                    func_0205e910(a, name, kind, 0);
                    delete[] name;
                } else if (kind == 0x13) {
                    char *name = func_02054bac(data_020f57b8, 0xaa, k + 0x6f);

                    func_0205e910(a, name, kind, 0);
                    delete[] name;
                }
                if (k == 8) {
                    a->b43 = 2;
                } else if (k == 9) {
                    a->b43 = 3;
                } else {
                    a->b43 = flag;
                }
                func_0205f91c(a, speed);
                func_0205f930(a, RAND_FX32(RandRange(0, 10) - 2.6f));
                func_0205f938(a, RAND_FX32(RandRange(0, 27) - 20.0f));
                func_0205fcf0(a, 10);
                break;
            }
        }
    }
}
#else
// Linker names used by the asm below.
extern "C" {
void _Z9RandRangemm(void);
void _ZdaPv(void);
void _fadd(void);
void _ffix(void);
void _ffltu(void);
void _fgr(void);
void _fmul(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02032948(void *obj, int kind) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0xc
    mov r9, r1
    mov r10, r0
    cmp r9, #0x9
    bne _02032998
    mov r1, #0x1a8
    bl func_0203366c
    cmp r0, #0x0
    addeq sp, sp, #0xc
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r0, r10
    mov r1, #0x65
    bl func_02031ab8
    ldr r1, [pc, #0x314]
    bl func_02044180
    mov r4, r0
    mov r0, #0x28
    str r0, [sp, #0x8]
    b _020329cc
_02032998:
    ldr r1, [pc, #0x300]
    bl func_0203366c
    cmp r0, #0x0
    addeq sp, sp, #0xc
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r0, r10
    mov r1, #0x5e
    bl func_02031ab8
    ldr r1, [pc, #0x2dc]
    bl func_02044180
    mov r4, r0
    mov r0, #0x2a
    str r0, [sp, #0x8]
_020329cc:
    mov r0, r10
    mov r1, r9
    bl func_02032fd8
    mov r1, #0x4c
    mla r1, r0, r1, r10
    add r0, r1, #0x9000
    ldrb r0, [r0, #0x603]
    mov r2, #0x0
    mov r3, #0xd6
    cmp r0, #0x0
    moveq r0, #0x1
    streq r0, [sp, #0x0]
    movne r0, #0x0
    strne r0, [sp, #0x0]
    cmp r4, #0x0
    moveq r4, #0x1
    mov r0, #0x4c
_02032a10:
    mla r1, r3, r0, r10
    add r1, r1, #0x9000
    ldrb r1, [r1, #0x5ce]
    add r3, r3, #0x1
    cmp r9, r1
    addeq r2, r2, #0x1
    cmp r3, #0xfe
    blt _02032a10
    cmp r2, r4
    addeq sp, sp, #0xc
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    sub r0, r4, r2
    str r0, [sp, #0x4]
    cmp r0, #0x0
    mov r11, #0x0
    addle sp, sp, #0xc
    ldmleia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
_02032a54:
    add r0, r10, #0x15c0
    mov r8, #0x0
    mov r6, #0xd6
    add r5, r0, #0x8000
    mov r4, #0x4c
_02032a68:
    mla r7, r6, r4, r5
    ldrb r0, [r7, #0xe]
    cmp r9, r0
    addeq r8, r8, #0x1
    beq _02032c78
    mov r0, r7
    bl func_0205ec34
    cmp r0, #0x0
    beq _02032c78
    cmp r9, #0x9
    bne _02032acc
    ldr r0, [pc, #0x208]
    add r2, r8, #0x65
    ldr r0, [r0, #0x0]
    mov r1, #0xaa
    bl func_02054bac
    mov r4, r0
    mov r0, r7
    mov r1, r4
    mov r2, r9
    mov r3, #0x0
    bl func_0205e910
    mov r0, r4
    bl _ZdaPv
    b _02032b08
_02032acc:
    cmp r9, #0x13
    bne _02032b08
    ldr r0, [pc, #0x1c8]
    add r2, r8, #0x6f
    ldr r0, [r0, #0x0]
    mov r1, #0xaa
    bl func_02054bac
    mov r4, r0
    mov r0, r7
    mov r1, r4
    mov r2, r9
    mov r3, #0x0
    bl func_0205e910
    mov r0, r4
    bl _ZdaPv
_02032b08:
    cmp r8, #0x8
    moveq r0, #0x2
    streqb r0, [r7, #0x43]
    beq _02032b2c
    cmp r8, #0x9
    moveq r0, #0x3
    streqb r0, [r7, #0x43]
    ldrne r0, [sp, #0x0]
    strneb r0, [r7, #0x43]
_02032b2c:
    ldr r1, [sp, #0x8]
    mov r0, r7
    bl func_0205f91c
    mov r0, #0x0
    mov r1, #0xa
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0x154]
    bl _fsub
    mov r1, #0x0
    bl _fgr
    mov r0, #0x0
    mov r1, #0xa
    bls _02032b98
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0x12c]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x124]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02032bc0
_02032b98:
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0xfc]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0xf4]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02032bc0:
    bl _ffix
    mov r1, r0
    mov r0, r7
    bl func_0205f930
    mov r0, #0x0
    mov r1, #0x1b
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0xc4]
    bl _fsub
    mov r1, #0x0
    bl _fgr
    mov r0, #0x0
    mov r1, #0x1b
    bls _02032c30
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0x9c]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x8c]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02032c58
_02032c30:
    bl _Z9RandRangemm
    bl _ffltu
    mov r1, r0
    ldr r0, [pc, #0x6c]
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x5c]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02032c58:
    bl _ffix
    mov r1, r0
    mov r0, r7
    bl func_0205f938
    mov r0, r7
    mov r1, #0xa
    bl func_0205fcf0
    b _02032c84
_02032c78:
    add r6, r6, #0x1
    cmp r6, #0xfe
    blt _02032a68
_02032c84:
    ldr r0, [sp, #0x4]
    add r11, r11, #0x1
    cmp r11, r0
    blt _02032a54
    add sp, sp, #0xc
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd 0xffff
    dcd 0x1a7
    dcd data_020f57b8
    dcd 0xc0266666
    dcd 0x45800000
    dcd 0x41a00000
}
#endif

void func_02032cb4(void *obj)
{
    GameData *self = (GameData *)obj;
    u32 i;
    Animal *a = self->animals;

    for (i = 0; i < 0xfe; i++, a++) {
        if (func_0205ec34(a) == 0) {
            func_02060798(a);
        }
    }
}

#ifdef NONMATCHING
void func_02032cf0(void *obj)
{
    GameData *self = (GameData *)obj;
    Animal *a;
    u32 i;
    Obj16 *o = self->objs;

    for (i = 0; i < 12; i++, o++) {
        func_02070174(o);
    }
    a = self->animals;
    for (i = 0; i < 0xfe; i++, a++) {
        if (func_0205ec34(a) == 0) {
            func_0205eab0(a);
        }
    }
    func_0205ff98();
    func_02083ab4(data_020f57c4);
    func_02083be0(data_020f57c4);
    if (func_020837e4(data_020f57c4)) {
        func_020322d8(self);
    }
    func_02082820(data_020f57c4);
    func_02082984(data_020f57c4);
    func_02082ad8(data_020f57c4);
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02032cf0(void *obj) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r4, r0
    add r0, r4, #0x290
    add r6, r0, #0xe000
    mov r5, #0x0
_02032d04:
    mov r0, r6
    bl func_02070174
    add r5, r5, #0x1
    cmp r5, #0xc
    add r6, r6, #0x10
    blo _02032d04
    add r0, r4, #0x15c0
    add r6, r0, #0x8000
    mov r5, #0x0
_02032d28:
    mov r0, r6
    bl func_0205ec34
    cmp r0, #0x0
    bne _02032d40
    mov r0, r6
    bl func_0205eab0
_02032d40:
    add r5, r5, #0x1
    cmp r5, #0xfe
    add r6, r6, #0x4c
    blo _02032d28
    bl func_0205ff98
    ldr r0, [pc, #0x54]
    ldr r0, [r0, #0x0]
    bl func_02083ab4
    ldr r0, [pc, #0x48]
    ldr r0, [r0, #0x0]
    bl func_02083be0
    ldr r0, [pc, #0x3c]
    ldr r0, [r0, #0x0]
    bl func_020837e4
    cmp r0, #0x0
    beq _02032d88
    mov r0, r4
    bl func_020322d8
_02032d88:
    ldr r0, [pc, #0x20]
    ldr r0, [r0, #0x0]
    bl func_02082820
    ldr r0, [pc, #0x14]
    ldr r0, [r0, #0x0]
    bl func_02082984
    ldr r0, [pc, #0x8]
    ldr r0, [r0, #0x0]
    bl func_02082ad8
    ldmia sp!, {r4, r5, r6, pc}
    dcd data_020f57c4
}
#endif

} // extern "C"

// An animal in use whose kind is `kind`.
struct AnimalIsKind {
    AnimalIsKind(int kind) : kind(kind) {}
    bool operator()(Animal &a) const { return func_0205ec34(&a) == 0 && a.kind == kind; }

    int kind;
};

template <class It, class Pred> inline It FindIf(It first, It last, Pred pred)
{
    while (first != last && !pred(*first)) {
        ++first;
    }
    return first;
}

template <class It, class Pred> inline int CountIf(It first, It last, Pred pred)
{
    int n = 0;

    for (; first != last; ++first) {
        if (pred(*first)) {
            n++;
        }
    }
    return n;
}

extern "C" {

#ifdef NONMATCHING
BOOL func_02032db4(void *obj, int kind)
{
    GameData *self = (GameData *)obj;

    return FindIf(self->animals, self->animals + 0x1c, AnimalIsKind(kind)) != self->animals + 0x1c;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL func_02032db4(void *obj, int kind) {
    stmdb sp!, {r3, r4, r5, r6, r7, lr}
    sub sp, sp, #0x8
    mov r7, r0
    mov r6, r1
    add r0, r7, #0xe10
    add r1, r7, #0x15c0
    str r6, [sp, #0x4]
    str r6, [sp, #0x0]
    add r5, r0, #0x9000
    add r4, r1, #0x8000
    b _02032de4
_02032de0:
    add r4, r4, #0x4c
_02032de4:
    cmp r4, r5
    beq _02032e04
    mov r0, r4
    bl func_0205ec34
    cmp r0, #0x0
    ldreqb r0, [r4, #0xe]
    cmpeq r6, r0
    bne _02032de0
_02032e04:
    add r0, r7, #0xe10
    add r0, r0, #0x9000
    cmp r4, r0
    movne r0, #0x1
    moveq r0, #0x0
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, pc}
}
#endif

#ifdef NONMATCHING
int func_02032e20(void *obj, int kind)
{
    GameData *self = (GameData *)obj;

    Animal *last = self->animals + 0x1c;

    return CountIf(self->animals, last, AnimalIsKind(kind));
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_02032e20(void *obj, int kind) {
    stmdb sp!, {r3, r4, r5, r6, r7, lr}
    sub sp, sp, #0x8
    add r2, r0, #0xe10
    mov r7, r1
    add r0, r0, #0x15c0
    add r5, r2, #0x9000
    add r4, r0, #0x8000
    str r7, [sp, #0x4]
    str r7, [sp, #0x0]
    cmp r4, r5
    mov r6, #0x0
    beq _02032e74
_02032e50:
    mov r0, r4
    bl func_0205ec34
    cmp r0, #0x0
    ldreqb r0, [r4, #0xe]
    add r4, r4, #0x4c
    cmpeq r7, r0
    addeq r6, r6, #0x1
    cmp r4, r5
    bne _02032e50
_02032e74:
    mov r0, r6
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, pc}
}
#endif

int func_02032e80(void *self, int kind)
{
    int n = 0;

    switch (kind) {
    case 0:
    case 1:
        n = func_02032e20(self, 0);
        n += func_02032e20(self, 1);
        break;
    case 2:
    case 3:
    case 4:
        n = func_02032e20(self, 2);
        n += func_02032e20(self, 3);
        n += func_02032e20(self, 4);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        n = func_02032e20(self, 5);
        n += func_02032e20(self, 6);
        n += func_02032e20(self, 7);
        n += func_02032e20(self, 8);
        break;
    case 9:
    case 10:
    case 18:
    case 19:
        n = func_02032e20(self, kind);
        break;
    }
    return n;
}

int func_02032f88(void *self)
{
    int a = func_02032e20(self, 9);
    int b = func_02032e20(self, 10);
    int c = func_02032e20(self, 0x13);
    int d = func_02032e20(self, 0x12);

    return a + b + c + d;
}

#ifdef NONMATCHING
int func_02032fd8(void *obj, int kind)
{
    GameData *self = (GameData *)obj;
    Animal *a = FindIf(self->animals, self->animals + 0x1c, AnimalIsKind(kind));

    return a != self->animals + 0x1c ? a->id : -1;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_02032fd8(void *obj, int kind) {
    stmdb sp!, {r3, r4, r5, r6, r7, lr}
    sub sp, sp, #0x8
    mov r7, r0
    mov r6, r1
    add r0, r7, #0xe10
    add r1, r7, #0x15c0
    str r6, [sp, #0x4]
    str r6, [sp, #0x0]
    add r5, r0, #0x9000
    add r4, r1, #0x8000
    b _02033008
_02033004:
    add r4, r4, #0x4c
_02033008:
    cmp r4, r5
    beq _02033028
    mov r0, r4
    bl func_0205ec34
    cmp r0, #0x0
    ldreqb r0, [r4, #0xe]
    cmpeq r6, r0
    bne _02033004
_02033028:
    add r0, r7, #0xe10
    add r0, r0, #0x9000
    cmp r4, r0
    ldrneb r0, [r4, #0x0]
    mvneq r0, #0x0
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, pc}
}
#endif

int func_02033044(void *self)
{
    int count;
    int n;
    int i;

    if (func_0203366c(self, 0xf) == 0) {
        return 0;
    }
    count = 0;
    n = func_0203366c(self, 0x25a);
    for (i = 0; i < n; i++) {
        if (func_0203366c(self, func_020330b8(self, i))) {
            count++;
        }
    }
    return count;
}

int func_020330b8(void *self, int i)
{
    return data_020b1e64[i];
}

// Four animal slot numbers (-1 = empty).
void *func_020330c8(void *self)
{
    func_02033130((AnimalSlots *)self);
    return self;
}

void func_020330dc(void *self)
{
}

BOOL func_020330e0(AnimalSlots *self, int slot)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        if (func_020331e4(self, i) == 0) {
            func_020331d0(self, i, slot);
            return TRUE;
        }
    }
    return FALSE;
}

void func_02033130(AnimalSlots *self)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        func_020331d8(self, i);
    }
}

BOOL func_02033158(AnimalSlots *self)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        if (func_020331e4(self, i)) {
            return TRUE;
        }
    }
    return FALSE;
}

Animal *func_02033190(AnimalSlots *self, int i)
{
    if (func_020331e4(self, i) == 0) {
        return NULL;
    }
    return &data_020f57c0->animals[self->slot[i]];
}

void func_020331d0(AnimalSlots *self, int i, int slot)
{
    self->slot[i] = slot;
}

void func_020331d8(AnimalSlots *self, int i)
{
    self->slot[i] = -1;
}

BOOL func_020331e4(AnimalSlots *self, int i)
{
    return self->slot[i] != -1;
}

void func_020331fc(void *obj)
{
    GameData *self = (GameData *)obj;
    int i;
    Obj20 *o;

    self->unke280 = -1;
    o = self->unke168;
    for (i = 0; i < 6; i++, o++) {
        func_020778a8(o, i);
    }
}

void func_02033238(void *obj, s32 value)
{
    ((GameData *)obj)->unke280 = value;
}

s32 func_02033244(void *obj)
{
    return ((GameData *)obj)->unke280;
}

int func_02033250(void *obj)
{
    GameData *self = (GameData *)obj;
    int sum = 0;
    int i;
    Obj20 *o = self->unke168;

    for (i = 0; i < 6; i++, o++) {
        sum += o->b6;
    }
    return sum;
}

void func_0203327c(void *obj)
{
    func_020a01f0(&((GameData *)obj)->unke1e8);
}

void func_02033290(void *obj)
{
    Obj10 *o = (Obj10 *)obj;

    o->a = 0;
    o->b = 0;
    o->c = 0;
    o->d = 0;
}

void func_020332a8(void *self)
{
}
}
