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
s8 func_02035784(GameDay *date);
BOOL func_0205ec34(void *a);
BOOL func_0205ecf8(void *a);
void func_0205fb74(void *a, GameTime t);
extern const GameTime data_020b2728[4][4];

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

}
