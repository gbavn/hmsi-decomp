// The game clock and calendar (ARM9 0x020350d4..): time of day, date, seasons. The state lives in the
// save object (data_020f57c0, see GameData.cpp): date at +0x8, frame counter at +0x10, time at +0x14,
// clock speed at +0x9c. Names are ours.
#include <nitro.h>

extern "C" {

typedef struct Words2 {
    u32 a;
    u32 b;
} Words2;

typedef struct GameTime {
    s8 hour;
    s8 minute;
} GameTime;

// Calendar date (8 bytes).
typedef struct GameDay {
    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
} GameDay;

// A date zeroed by its constructor.
struct ZeroDay {
    ZeroDay() { ((u32 *)this)[1] = 0; ((u32 *)this)[0] = 0; }

    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
    s8 pad;
};

typedef struct Animal {
    u8 unk0[0x2c];
    GameTime time;
    u8 unk2e[0x4c - 0x2e];
} Animal;

typedef struct ClockState {
    void **vt;
    u8 unk4[4];
    GameDay date;
    s32 frames;
    GameTime time;
    u8 unk16[0x9c - 0x16];
    f32 speed;
    u8 unka0[0x95c0 - 0xa0];
    Animal animals[0xfe];
} ClockState;

extern ClockState *data_020f57c0;
extern u8 data_020f57d0;           // clock stopped

void _ZdlPv(void *ptr);
void func_020352f8(int minutes);
void func_02035394(int seconds);
int func_02035444(const GameTime *t);
GameTime func_02035460(int seconds);
const GameTime *func_020352c8(int i);
void func_02035524(GameDay *out);
void func_02035560(int days);
void func_02035870(GameDay *date);
int func_02035784(GameDay *date);
BOOL func_0205ec34(void *a);
BOOL func_0205ecf8(void *a);
void func_0205fb74(void *a, GameTime t);
extern const GameTime data_020b2728[4][4];
int func_020356dc(GameDay *date);
void func_0203570c(GameDay *out, u32 n);
u32 func_020357e8(u32 n, int add);
u16 func_02035874(void);
u32 func_020358b8(void);
u32 func_020358d0(u32 max);
void func_02035934(u32 *a, u32 *b);
extern s8 data_020f57d8[36];
extern u32 data_020bc948;

void *func_020350d4(void *self)
{
    data_020f57c0 = NULL;
    return self;
}

void *func_020350e8(void *self)
{
    data_020f57c0 = NULL;
    _ZdlPv(self);
    return self;
}

// Called every frame: one game minute every 12 * speed frames.
#ifdef NONMATCHING
void func_0203510c(void)
{
    if (data_020f57d0) {
        return;
    }
    data_020f57c0->frames++;
    if (12.0f * data_020f57c0->speed <= data_020f57c0->frames) {
        func_020352f8(1);
        data_020f57c0->frames = 0;
    }
}
#else
// Linker names used by the asm below.
extern "C" {
void _fflt(void);
void _fleq(void);
void _fmul(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void func_0203510c(void) {
    stmdb sp!, {r4, lr}
    ldr r0, [pc, #0x6c]
    ldrb r0, [r0, #0x0]
    cmp r0, #0x0
    ldmneia sp!, {r4, pc}
    ldr r1, [pc, #0x60]
    ldr r0, [pc, #0x60]
    ldr r3, [r1, #0x0]
    ldr r2, [r3, #0x10]
    add r2, r2, #0x1
    str r2, [r3, #0x10]
    ldr r1, [r1, #0x0]
    ldr r1, [r1, #0x9c]
    bl _fmul
    ldr r1, [pc, #0x3c]
    mov r4, r0
    ldr r0, [r1, #0x0]
    ldr r0, [r0, #0x10]
    bl _fflt
    mov r1, r0
    mov r0, r4
    bl _fleq
    ldmhiia sp!, {r4, pc}
    mov r0, #0x1
    bl func_020352f8
    ldr r0, [pc, #0x10]
    mov r1, #0x0
    ldr r0, [r0, #0x0]
    str r1, [r0, #0x10]
    ldmia sp!, {r4, pc}
    dcd data_020f57d0
    dcd data_020f57c0
    dcd 0x41400000
}
#endif


void func_02035190(const GameTime *time)
{
    ClockState *s = data_020f57c0;

    s->time.hour = time->hour;
    s->time.minute = time->minute;
}

GameTime *func_020351b0(void)
{
    return &data_020f57c0->time;
}

} // extern "C"

static inline BOOL IsEarlier(const GameTime *a, const GameTime *b)
{
    return (BOOL)(func_02035444(a) < func_02035444(b));
}

extern "C" {

// Part of the day: 0..3 by the season's limits in data_020b2728.
int func_020351c4(void)
{
    GameTime now = data_020f57c0->time;

    if (IsEarlier(&now, func_020352c8(0))) {
        return 3;
    }
    if (IsEarlier(&now, func_020352c8(1))) {
        return 0;
    }
    if (IsEarlier(&now, func_020352c8(2))) {
        return 1;
    }
    if (IsEarlier(&now, func_020352c8(3))) {
        return 2;
    }
    return 3;
}

const GameTime *func_020352c8(int i)
{
    GameDay today;

    func_02035524(&today);
    return &data_020b2728[today.season][i];
}

// Advances the clock by `minutes` (a new day starts after 24:00).
void func_020352f8(int minutes)
{
    int add = minutes * 60;
    ClockState *s = data_020f57c0;
    int t = add + func_02035444(&s->time);

    if (t < 0) {
        t = 0;
    } else if (t >= 86400) {
        int days = 0;

        do {
            t -= 86400;
            days++;
        } while (t >= 86400);
        func_02035560(days);
    }
    s->time = func_02035460(t);
    func_02035394(add);
}

#ifdef NONMATCHING
void func_02035394(int seconds)
{
    Animal *a = data_020f57c0->animals;
    u32 i;

    for (i = 0; i < 0xfe; i++, a++) {
        if (func_0205ec34(a) == 0 && func_0205ecf8(a)) {
            GameTime t;

            t.hour = a->time.hour;
            t.minute = a->time.minute;
            func_0205fb74(a, func_02035460(func_02035444(&t) + seconds));
        }
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02035394(int seconds) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
    sub sp, sp, #0x4
    ldr r1, [pc, #0x9c]
    mov r7, r0
    ldr r0, [r1, #0x0]
    mov r5, #0x0
    add r0, r0, #0x15c0
    add r6, r0, #0x8000
    sub r8, sp, #0x4
    add r4, sp, #0x2
_020353bc:
    mov r0, r6
    bl func_0205ec34
    cmp r0, #0x0
    bne _02035428
    mov r0, r6
    bl func_0205ecf8
    cmp r0, #0x0
    beq _02035428
    ldrb r2, [r6, #0x2c]
    ldrb r1, [r6, #0x2d]
    mov r0, r4
    strb r2, [sp, #0x2]
    strb r1, [sp, #0x3]
    bl func_02035444
    add r0, r7, r0
    bl func_02035460
    mov r3, r0
    strb r3, [sp, #0x0]
    mov r3, r3, lsr #0x8
    ldrb r2, [sp, #0x0]
    and r1, r3, #0xff
    mov r0, r6
    strb r3, [sp, #0x1]
    strb r2, [r8, #0x0]
    strb r1, [r8, #0x1]
    ldr r1, [r8, #0x0]
    bl func_0205fb74
_02035428:
    add r5, r5, #0x1
    cmp r5, #0xfe
    add r6, r6, #0x4c
    blo _020353bc
    add sp, sp, #0x4
    ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
    dcd data_020f57c0
}
#endif

// Seconds since midnight.
int func_02035444(const GameTime *t)
{
    return t->hour * 3600 + t->minute * 60;
}

GameTime func_02035460(int seconds)
{
    GameTime t;

    t.hour = seconds / 3600;
    t.minute = seconds % 3600 / 60;
    return t;
}

void func_020354c4(GameDay *date)
{
    GameDay d;
    ClockState *s;

    func_02035870(date);
    *(Words2 *)&d = *(Words2 *)date;
    d.weekday = func_02035784(date);
    s = data_020f57c0;
    s->date.year = d.year;
    s->date.season = d.season;
    s->date.day = d.day;
    s->date.weekday = d.weekday;
}

#ifdef NONMATCHING
void func_02035524(GameDay *out)
{
    ClockState *s = data_020f57c0;
    u32 a = ((Words2 *)&s->date)->a;
    u32 b = ((Words2 *)&s->date)->b;

    ((Words2 *)out)->a = a;
    ((Words2 *)out)->b = b;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02035524(GameDay *out) {
    ldr r1, [pc, #0x14]
    ldr r1, [r1, #0x0]
    ldr r2, [r1, #0x8]
    ldr r1, [r1, #0xc]
    str r2, [r0, #0x0]
    str r1, [r0, #0x4]
    bx lr
    dcd data_020f57c0
}
#endif

int func_02035544(void)
{
    GameDay today;

    func_02035524(&today);
    return today.season;
}


// Moves the current date by `days`.
void func_02035560(int days)
{
    GameDay d2;
    GameDay d;
    ClockState *s;

    func_0203570c(&d, func_020357e8(func_020356dc(&data_020f57c0->date), days));
    *(Words2 *)&d2 = *(Words2 *)&d;
    d2.weekday = func_02035784(&d2);
    s = data_020f57c0;
    s->date.year = d2.year;
    s->date.season = d2.season;
    s->date.day = d2.day;
    s->date.weekday = d2.weekday;
}

// `out` = `date` moved by `days`.
void func_020355e0(GameDay *out, GameDay *date, int days)
{
    func_02035870(date);
    func_0203570c(out, func_020357e8(func_020356dc(date), days));
}

// Days from `a` to `b`.
int func_02035618(GameDay *a, GameDay *b)
{
    int na;

    func_02035870(a);
    func_02035870(b);
    na = func_020356dc(a);
    return func_020356dc(b) - na;
}

// Calendar page of the month of `date`: cell -> day (0..29), -1 for empty cells.
#ifdef NONMATCHING
s8 *func_0203564c(GameDay *date)
{
    GameDay first;
    int i;
    int wd;
    s8 day;

    func_02035870(date);
    for (i = 0; i < 36; i++) {
        data_020f57d8[i] = -1;
    }
    first = *date;
    first.day = 0;
    wd = func_02035784(&first);
    day = 0;
    for (i = wd; i < wd + 30; i++) {
        data_020f57d8[i] = day++;
    }
    return data_020f57d8;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm s8 *func_0203564c(GameDay *date) {
    stmdb sp!, {r4, lr}
    sub sp, sp, #0x8
    mov r4, r0
    bl func_02035870
    ldr r2, [pc, #0x74]
    mov r1, #0x0
    mvn r0, #0x0
_02035668:
    add r1, r1, #0x1
    cmp r1, #0x24
    strb r0, [r2], #0x1
    blt _02035668
    ldr r2, [r4, #0x0]
    ldr r1, [r4, #0x4]
    add r0, sp, #0x0
    str r1, [sp, #0x4]
    mov r1, #0x0
    str r2, [sp, #0x0]
    strb r1, [sp, #0x5]
    bl func_02035784
    add ip, r0, #0x1e
    cmp r0, ip
    mov r2, #0x0
    bge _020356cc
    ldr r1, [pc, #0x28]
    add r3, r1, r0
_020356b0:
    add r1, r2, #0x1
    add r0, r0, #0x1
    mov r1, r1, lsl #0x18
    strb r2, [r3], #0x1
    cmp r0, ip
    mov r2, r1, asr #0x18
    blt _020356b0
_020356cc:
    ldr r0, [pc, #0x4]
    add sp, sp, #0x8
    ldmia sp!, {r4, pc}
    dcd data_020f57d8
}
#endif

// Day number: 4 seasons of 30 days per year.
int func_020356dc(GameDay *date)
{
    func_02035870(date);
    return date->year * 120 + date->season * 30 + date->day;
}

#ifdef NONMATCHING
void func_0203570c(GameDay *out, u32 n)
{
    ZeroDay d;

    d.year = n / 120;
    d.season = n % 120 / 30;
    d.day = n % 120 % 30;
    *(Words2 *)out = *(Words2 *)&d;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_0203570c(GameDay *out, u32 n) {
    stmdb sp!, {r3, r4, r5, lr}
    sub sp, sp, #0x8
    ldr lr, [pc, #0x64]
    mov ip, #0x78
    umull r3, r2, r1, lr
    mov r2, r2, lsr #0x6
    umull r2, r3, ip, r2
    sub r2, r1, r2
    umull r3, r4, r2, lr
    umull ip, r3, r1, lr
    umull r1, r5, r2, lr
    mov r4, r4, lsr #0x4
    mov lr, #0x1e
    umull r1, ip, lr, r4
    add lr, sp, #0x0
    mov ip, #0x0
    str ip, [lr, #0x4]
    mov r5, r5, lsr #0x4
    sub r4, r2, r1
    strb r5, [sp, #0x4]
    strb r4, [sp, #0x5]
    str ip, [lr, #0x0]
    mov r3, r3, lsr #0x6
    ldr r1, [sp, #0x4]
    str r3, [sp, #0x0]
    str r3, [r0, #0x0]
    str r1, [r0, #0x4]
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, pc}
    dcd 0x88888889
}
#endif

// Day of the week (0..6).
int func_02035784(GameDay *date)
{
    u32 n;

    func_02035870(date);
    n = (u32)(date->year * 4 + date->season) % 7 * 30 + date->day;
    return n % 7;
}

// `n + add`, kept inside 0..0xffffffff.
u32 func_020357e8(u32 n, int add)
{
    double v = (double)n + (double)add;

    if (v < 0.0) {
        v = 0.0;
    } else if (4294967295.0 < v) {
        v = 4294967295.0;
    }
    return (u32)v;
}

void func_02035870(GameDay *date)
{
}

u16 func_02035874(void)
{
    data_020bc948 = data_020bc948 * 0x41c64e6d + 0x3039;
    return data_020bc948 >> 16;
}

void func_020358a8(u32 seed)
{
    data_020bc948 = seed;
}

u32 func_020358b8(void)
{
    u32 hi = func_02035874();

    return func_02035874() | hi << 16;
}

// Random number 0..max.
u32 func_020358d0(u32 max)
{
    return func_020358b8() % (max + 1);
}

}

// Random number min..max.
u32 RandRange(u32 min, u32 max)
{
    if (min > max) {
        func_02035934(&min, &max);
    }
    return min + func_020358d0(max - min);
}

extern "C" {

void func_02035934(u32 *a, u32 *b)
{
    u32 t = *a;

    *a = *b;
    *b = t;
}

// TRUE with `percent` % chance.
BOOL func_02035948(u32 percent)
{
    return percent > func_020358d0(99);
}

}

