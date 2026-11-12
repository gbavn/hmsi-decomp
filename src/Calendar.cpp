// Festivals, birthdays and the tables of the game variables (ARM9 0x02040ab8..0x020411c4). Names are ours.
//
// Festivals: 0x17 (season, day) pairs in data_020b2ca8; func_02040bec adds the conditions of some of
// them. Birthdays: 0x22 (season, day) pairs in data_020b2cd8 (one per resident; the player's is in the
// variables 0x23e/0x23f, the spouse's in 0x24c/0x24d). The game variables (flags, bytes, u16, u32, see
// GameData.cpp and docs/MODDING.md) have their maximum and initial values in tables, by kind.
#include <nitro.h>

extern "C" {

typedef struct GameDay {
    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
} GameDay;

typedef struct DayPair {
    s8 season;
    s8 day;                 // 1-30
} DayPair;

extern void *data_020f57c0;         // the game data
extern void *gResidents;
extern DayPair data_020b2ca8[0x17]; // festivals
extern DayPair data_020b2cd8[0x22]; // birthdays of the residents
extern s8 data_020b2cd9[];          // data_020b2cd8[i].day
extern const u32 data_020bdb7c;     // 0xfe
extern u8 data_020b2fac[];          // maximum of the flags
extern u8 data_020b2ec8[];          // maximum of the byte variables
extern u16 data_020b2d3c[];         // maximum of the u16 variables
extern u32 data_020b2d98[];         // maximum of the u32 variables
extern u8 data_020b31ec[];          // initial values...
extern u8 data_020b2de4[];
extern u16 data_020b2d1c[];
extern u32 data_020b2d5c[];

int func_0203366c(void *self, int id);
void func_0203357c(void *self, int id, u32 value);
void func_0203375c(void *self, int id, int value);
int func_02031b18(void *self, int i);
void func_02035524(GameDay *out);
BOOL func_0203f248(void *self, int who);
const u32 *func_0202f140(const u32 *a, const u32 *b);
int func_020908b8(int item);

int func_02040ba8(GameDay *day);
int func_02040bec(GameDay *day);
int func_02041028(int id);

// Is there a festival (that closes the shops) today?
BOOL func_02040ab8(void)
{
    if (!func_0203366c(data_020f57c0, 0x189)) {
        return FALSE;
    }
    GameDay today;
    func_02035524(&today);
    int f = func_02040bec(&today);
    if (f == -1 || f == 2 || f == 0x11 || f == 0x13 || f == 0x15) {
        return FALSE;
    }
    return TRUE;
}

BOOL func_02040b20(void)
{
    if (!func_0203366c(data_020f57c0, 0x189)) {
        return FALSE;
    }
    GameDay today;
    func_02035524(&today);
    return func_02040bec(&today) != -1;
}

BOOL func_02040b78(void)
{
    GameDay today;
    func_02035524(&today);
    return func_02040ba8(&today) != -1;
}

// Festival on `day` (-1: none).
#ifdef NONMATCHING
int func_02040ba8(GameDay *day)
{
    int d = day->day + 1;
    s8 season = day->season;
    DayPair *p = data_020b2ca8;
    for (u32 i = 0; i < 0x17; i++, p++) {
        if (season == p->season && d == p->day) {
            return i;
        }
    }
    return -1;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_02040ba8(GameDay *day) {
    ldrsb r1, [r0, #0x5]
    ldrsb ip, [r0, #0x4]
    ldr r2, [pc, #0x30]
    add r3, r1, #0x1
    mov r0, #0x0
_02040bbc:
    ldrsb r1, [r2, #0x0]
    cmp ip, r1
    ldreqsb r1, [r2, #0x1]
    cmpeq r3, r1
    bxeq lr
    add r0, r0, #0x1
    cmp r0, #0x17
    add r2, r2, #0x2
    blo _02040bbc
    mvn r0, #0x0
    bx lr
    dcd data_020b2ca8
}
#endif

// The same, with the conditions of some festivals.
#ifdef NONMATCHING
int func_02040bec(GameDay *day)
{
    int f = func_02040ba8(day);
    if (f == -1) {
        return -1;
    }
    void *gd = data_020f57c0;
    switch (f) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 16:
    case 17:
    case 18:
    case 19:
    case 21:
    case 22:
        return f;
    case 0:
        if (day->year != 0) {
            return f;
        }
        break;
    case 1:
        if (func_02031b18(gd, 0xe) || func_02031b18(gd, 6)) {
            return f;
        }
        break;
    case 20:
        if (func_0203366c(gd, 0x65)) {
            return f;
        }
        break;
    case 15: {
        int n = (u32)func_0203366c(gd, 0x322) < (u32)func_0203366c(gd, 0x323) ? func_0203366c(gd, 0x322)
                                                                                : func_0203366c(gd, 0x323);
        if (n + func_0203366c(gd, 0x324) != 0) {
            return f;
        }
        break;
    }
    }
    return -1;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_02040bec(GameDay *day) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r4, r0
    bl func_02040ba8
    mov r5, r0
    mvn r1, #0x0
    cmp r5, r1
    moveq r0, r1
    ldmeqia sp!, {r4, r5, r6, pc}
    ldr r1, [pc, #0x12c]
    cmp r5, #0x16
    ldr r6, [r1, #0x0]
    addls pc, pc, r5, lsl #0x2
    b _02040d38
_02040c20:
    b _02040c80
    b _02040c90
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040cdc
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040c7c
    b _02040cc0
    b _02040c7c
    b _02040c7c
_02040c7c:
    ldmia sp!, {r4, r5, r6, pc}
_02040c80:
    ldr r1, [r4, #0x0]
    cmp r1, #0x0
    beq _02040d38
    ldmia sp!, {r4, r5, r6, pc}
_02040c90:
    mov r0, r6
    mov r1, #0xe
    bl func_02031b18
    cmp r0, #0x0
    bne _02040cb8
    mov r0, r6
    mov r1, #0x6
    bl func_02031b18
    cmp r0, #0x0
    beq _02040d38
_02040cb8:
    mov r0, r5
    ldmia sp!, {r4, r5, r6, pc}
_02040cc0:
    mov r0, r6
    mov r1, #0x65
    bl func_0203366c
    cmp r0, #0x0
    beq _02040d38
    mov r0, r5
    ldmia sp!, {r4, r5, r6, pc}
_02040cdc:
    ldr r1, [pc, #0x60]
    mov r0, r6
    bl func_0203366c
    mov r4, r0
    ldr r1, [pc, #0x54]
    mov r0, r6
    bl func_0203366c
    cmp r4, r0
    bhs _02040d10
    ldr r1, [pc, #0x3c]
    mov r0, r6
    bl func_0203366c
    b _02040d1c
_02040d10:
    ldr r1, [pc, #0x30]
    mov r0, r6
    bl func_0203366c
_02040d1c:
    mov r4, r0
    mov r0, r6
    mov r1, #0x324
    bl func_0203366c
    adds r0, r0, r4
    movne r0, r5
    ldmneia sp!, {r4, r5, r6, pc}
_02040d38:
    mvn r0, #0x0
    ldmia sp!, {r4, r5, r6, pc}
    dcd data_020f57c0
    dcd 0x322
    dcd 0x323
}
#endif

// Birthday season of resident `who` (0 = the player, 0x1c/0x1d = the spouse).
s8 func_02040d4c(int who)
{
    if (func_0203f248(gResidents, who)) {
        return -1;
    }
    if (who == 0) {
        return func_0203366c(data_020f57c0, 0x23e);
    }
    if ((u32)(who - 0x1c) <= 1) {
        return func_0203366c(data_020f57c0, 0x24c);
    }
    return data_020b2cd8[who].season;
}

// Birthday day (0-29).
s8 func_02040dd8(int who)
{
    if (func_0203f248(gResidents, who)) {
        return -1;
    }
    if (who == 0) {
        return func_0203366c(data_020f57c0, 0x23f);
    }
    if ((u32)(who - 0x1c) <= 1) {
        return func_0203366c(data_020f57c0, 0x24d);
    }
    return data_020b2cd9[who * 2] - 1;
}

// Is `day` the player's birthday?
BOOL func_02040e78(GameDay *day)
{
    void *gd = data_020f57c0;
    if (day->season == func_0203366c(gd, 0x23e) && day->day == func_0203366c(gd, 0x23f)) {
        return TRUE;
    }
    return FALSE;
}

// Is `day` the spouse's birthday?
BOOL func_02040ed0(GameDay *day)
{
    void *gd = data_020f57c0;
    if (func_0203366c(gd, 1) && day->season == func_0203366c(gd, 0x24c) && day->day == func_0203366c(gd, 0x24d)) {
        return TRUE;
    }
    return FALSE;
}

// Resident whose birthday is `day` (0xff: none).
int func_02040f38(GameDay *day)
{
    int d = day->day + 1;
    s8 season = day->season;
    for (u32 i = 0; i < 0x22; i++) {
        if (season == data_020b2cd8[i].season && d == data_020b2cd8[i].day) {
            return i;
        }
    }
    return 0xff;
}

// Sets the spouse's birthday.
void func_02040f84(u32 id, int season, int day)
{
    void *gd = data_020f57c0;
    func_0203357c(gd, 0x24b, *func_0202f140(&id, &data_020bdb7c));
    func_0203357c(gd, 0x24c, season);
    func_0203357c(gd, 0x24d, day);
}

void func_02040ff4(int id, int item, int n)
{
    void *gd = data_020f57c0;
    func_0203375c(gd, id, n * func_020908b8(item));
}

// Kind of game variable `id`: 0 flag, 1 byte, 2 u16, 3 u32, -1 invalid.
int func_02041028(int id)
{
    if (id >= 0 && id < 0x23e) {
        return 0;
    }
    if (id >= 0x23e && id < 0x321) {
        return 1;
    }
    if (id >= 0x321 && id < 0x331) {
        return 2;
    }
    if (id >= 0x331 && id < 0x340) {
        return 3;
    }
    return -1;
}

// Maximum value of game variable `id`.
u32 func_020410a4(int id)
{
    switch (func_02041028(id)) {
    case 0:
        return data_020b2fac[id];
    case 1:
        return data_020b2ec8[id - 0x23e];
    case 2:
        return data_020b2d3c[id - 0x321];
    case 3:
        return data_020b2d98[id - 0x331];
    }
    return 0;
}

// Initial value of game variable `id`.
u32 func_02041134(int id)
{
    switch (func_02041028(id)) {
    case 0:
        return data_020b31ec[id];
    case 1:
        return data_020b2de4[id - 0x23e];
    case 2:
        return data_020b2d1c[id - 0x321];
    case 3:
        return data_020b2d5c[id - 0x331];
    }
    return 0;
}

}
