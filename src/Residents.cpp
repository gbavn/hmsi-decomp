// The residents (ARM9 0x0203cc24..): today's schedule of each of the 0x89 residents (see Schedule.cpp),
// spawning them on the current map, finding the one the player faces or touches, friendship changes
// after events... The object is gResidents (0x020f57c8). Names are ours.
//
// C++ class with a vtable at 0x020bdad0 (in the asm gap) deriving from a base class (0x02040328..).
// Methods are extern "C" functions; the classes below are "views" to make virtual calls.
#include <nitro.h>
#include "Task.h"
#include "input.h"

// Field objects (see GroundObject.cpp): slot 7 is GetPos.
class FieldObjV {
public:
    virtual void V0();
    virtual void V1();
    virtual BOOL V2();
    virtual void V3();
    virtual BOOL V4();
    virtual void V5();
    virtual void V6();
    virtual VecFx32 *GetPos();
};

typedef struct Npc Npc;

struct Npc {
    u8 unk0[0xc];
    Npc *next;
    u8 unk10[0x68 - 0x10];
    int id;                 // 0x68
    u8 unk6C[0x84 - 0x6c];
    FieldObjV *obj;         // 0x84
};

typedef struct NpcList {
    u8 unk0[4];
    Npc *head;              // sentinel
} NpcList;

typedef struct ObjList {
    u8 unk0[0x10];
    int unk10;
    int unk14;
} ObjList;

typedef struct FieldVars {
    u8 unk0[0x98];
    ObjList *objs;          // 0x98
    u8 unk9C[0xb4 - 0x9c];
    NpcList *npcs;          // 0xb4
} FieldVars;

extern "C" {

typedef struct ScheduleMove {
    s8 b0;
    s8 b1;
    s8 b2;
    s8 b3;
    int place;
    u32 w8;
    u32 wc;
} ScheduleMove;

typedef struct ScheduleEntry {
    s8 day;
    s8 weather;
    s8 season;
    s8 numConds;
    s8 numMoves;
    u32 *conds;
    ScheduleMove *moves;
} ScheduleEntry;

typedef struct ScheduleLoader {
    u8 keep;
} ScheduleLoader;

typedef struct Words2 {
    u32 a;
    u32 b;
} Words2;

typedef struct GameDay {
    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
} GameDay;

typedef struct Residents {
    void *vtable;
    ScheduleEntry today[0x89];      // 0x004
    ScheduleEntry special[0x89];    // 0x894
    int places[0x21];               // 0x1124 (indexed by resident - 1)
    Npc *groups[6];                 // 0x11a8 residents 0x26-0x4f, by group of 7
} Residents;

typedef struct Found {
    Npc *npc;
    fx32 dist;
} Found;

extern void *data_020bdad0[];       // vtable
extern void *data_020f57c0;         // the game data
extern void *data_020f57c4;
extern Residents *gResidents;
extern FieldVars data_020ec6ac;

Residents *func_02040328(Residents *self);
Residents *func_02040344(Residents *self);
void func_02038558(ScheduleLoader *self);
void func_02038564(ScheduleLoader *self);
void func_02038594(ScheduleLoader *self);
int func_02031b18(void *self, int i);
void func_02035524(GameDay *out);
int func_02055800(int place);
int func_02055898(int a);
void *func_020788d4(u32 size);
void *func_02077a08(void *self, int who, ScheduleMove *m, void *pos);
void func_02078620(Npc *npc);
ScheduleMove *func_02080de4(void *self, int who, int a);
BOOL func_02053ef4(void *a);
void func_ov002_021d0f8c(void *a, int msg, int b, int c, int d, int e, int f, int g, int h);
BOOL func_ov002_021d11f4(void *a);
BOOL func_ov002_0215b098(void *a);
int func_02093008(void *a);
void func_02078648(Npc *npc, int *x0, int *x1, int *y0, int *y1);

void func_0203d1fc(Residents *self);
Npc *func_0203d324(Residents *self);
Npc *func_0203d5cc(Residents *self);
BOOL func_0203d824(Residents *self, VecFx32 *a, VecFx32 *b);
BOOL func_0203e40c(Residents *self, int item);
BOOL func_0203f1b4(Residents *self, int who);
void func_020319ac(void *self, int who, int amount);
void func_02080d20(void *self, int who, int amount);
void func_02031af0(void *self, int who, int v);
void func_02031b70(void *self, int who, int v);
void func_02031ce0(void *self, int who, int v);
void func_02031d6c(void *self, int who, int v);
void func_02031df8(void *self, int who, int v);
int func_0203366c(void *self, int id);
BOOL func_02031c18(void *self, int who);
BOOL func_02031ebc(void *self, int who);
void func_02031b98(void *self, int who);
int func_02031bd4(void *self, int who);
int func_02031d50(void *self, int who);
int func_02031ddc(void *self, int who);
void func_02031e20(void *self, int who);
void func_02031bf0(void *self, int who, int v);
void func_02031c40(void *self, int who, int v);
void func_02031c90(void *self, int who, int v);
void func_02031e94(void *self, int who, int v);
void func_0203189c(void *self);
BOOL func_02035948(int percent);
int func_02080be0(void *self, int who);
BOOL func_02081160(void *self, int who);
BOOL func_02081178(void *self, int who);
void func_02081494(void *self);
void func_0203fb6c(Residents *self);
typedef struct EventObj EventObj;
void func_02044ed8(EventObj *self);
void func_02044ee4(EventObj *self);
void func_02044ef8(EventObj *self);
BOOL func_02045044(EventObj *self, int who);
BOOL func_020450ac(EventObj *self, int who);
BOOL func_0204514c(EventObj *self, int who);
void func_0203f7d4(Residents *self, EventObj *ev, int who);
void func_0203f76c(Residents *self, int who);
int func_0204ed14(int a);
void *func_02077dec(void *self, int a, int b, int c, s8 d);
void func_02038568(ScheduleLoader *self);
void func_02038580(ScheduleLoader *self, int who);
const void *func_020351b0(void);
void func_02033cf4(void *self, int v);
BOOL func_02031f7c(void *self, int who);
int func_02032080(void *self, int who);
BOOL func_020816b8(void *self, int who);
int func_02082450(void *self, int place);
void func_0206fb5c(int a);
BOOL func_0203f248(Residents *self, int who);
extern void *data_020bdae0[];
extern int data_020b2a90[6];
extern int data_020b2aa8[6];
extern int data_020b2ac0[];
extern int data_020b2b38[];
extern int data_020b2bb0[];
extern ScheduleMove data_020b2c28[6];
Npc *func_0203e5dc(Residents *self, int who);
void func_0203e760(Residents *self);
int func_0203e820(Residents *self, int who);
ScheduleMove *func_0203e830(Residents *self, int who);
BOOL _ZN9Residents10IsResidentEi(Residents *self, int who);
BOOL func_0203f264(Residents *self, int who);
u8 *func_0203f674(Residents *self);
FieldObjV *func_0203f68c(Residents *self);
BOOL func_0203f6a4(Residents *self);
BOOL func_0203f6cc(Residents *self, int who, ScheduleEntry *e, int i);
void func_0203f988(Residents *self, Npc *npc);
Npc *func_0203fa10(Residents *self, int who);
ScheduleMove *func_0203fa74(Residents *self, int a);

#ifdef NONMATCHING
Residents *func_0203cc24(Residents *self)
{
    func_02040328(self);
    self->vtable = data_020bdad0;
    for (int i = 0; i < 0x89; i++) {
        MI_CpuFill8(&self->today[i], 0, sizeof(ScheduleEntry));
        MI_CpuFill8(&self->special[i], 0, sizeof(ScheduleEntry));
    }
    for (int i = 0; i < 0x21; i++) {
        self->places[i] = -1;
    }
    return self;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm Residents *func_0203cc24(Residents *self) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r10, r0
    bl func_02040328
    mov r8, #0x0
    ldr r0, [pc, #0x74]
    mov r9, r8
    str r0, [r10, #0x0]
    add r0, r10, #0x94
    add r7, r10, #0x4
    add r4, r0, #0x800
    mov r6, r8
    mov r5, #0x10
    mov r11, r8
_0203cc58:
    mov r1, r6
    mov r2, r5
    add r0, r7, r9
    bl MI_CpuFill8
    mov r1, r11
    mov r2, #0x10
    add r0, r4, r9
    bl MI_CpuFill8
    add r8, r8, #0x1
    cmp r8, #0x89
    add r9, r9, #0x10
    blt _0203cc58
    mov r2, #0x0
    mvn r1, #0x0
_0203cc90:
    add r0, r10, r2, lsl #0x2
    add r0, r0, #0x1000
    add r2, r2, #0x1
    str r1, [r0, #0x124]
    cmp r2, #0x21
    blt _0203cc90
    mov r0, r10
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_020bdad0
}
#endif

Residents *func_0203ccb4(Residents *self)
{
    self->vtable = data_020bdad0;
    func_0203d1fc(self);
    func_0203e760(self);
    func_02040344(self);
    return self;
}

Residents *func_0203cce4(Residents *self)
{
    self->vtable = data_020bdad0;
    func_0203d1fc(self);
    func_0203e760(self);
    func_02040344(self);
    operator delete(self);
    return self;
}

// Chooses today's schedules again (keeping the ones already chosen).
void func_0203cd1c(Residents *self)
{
    func_0203e760(self);
    ScheduleLoader loader;
    func_02038558(&loader);
    func_02038594(&loader);
    func_02038564(&loader);
}

// Spawns the residents who are on map `map` now.
#ifdef NONMATCHING
void func_0203cd40(Residents *self, int map)
{
    void *gd = data_020f57c0;
    GameDay today;
    func_02035524(&today);
    for (int who = 0; who < 0x89; who++) {
        if (!func_0203f264(self, who)) {
            continue;
        }
        if (func_02031b18(gd, who) == 3) {
            continue;
        }
        if (func_02031b18(gd, who) == 4) {
            if (func_0203e820(gResidents, who) != func_02055898(map)) {
                continue;
            }
            if (_ZN9Residents10IsResidentEi(gResidents, who)) {
                self->places[who - 1] = func_02055800(func_0203e820(gResidents, who));
            }
            void *npc = func_020788d4(0x90);
            if (npc != NULL) {
                npc = func_02077a08(npc, who, func_0203e830(self, who), func_0203fa10(self, who));
            }
            func_0203f988(self, (Npc *)npc);
            continue;
        }
        ScheduleEntry *e = &self->today[who];
        BOOL flag = FALSE;
        for (int i = 0; i < e->numMoves; i++) {
            ScheduleMove *m = &e->moves[i];
            if (!func_0203f6cc(self, who, e, i)) {
                continue;
            }
            if (_ZN9Residents10IsResidentEi(gResidents, who)) {
                self->places[who - 1] = func_02055800(m->place);
            }
            if (m->place == 0x2b) {
                if (func_02055898(map) != 0x2a) {
                    continue;
                }
                flag = TRUE;
            } else if (m->place == 0x2c) {
                if (map != 0x3c) {
                    continue;
                }
            } else if (who == 0x74) {
                m = func_0203fa74(self, map);
                if (m == NULL) {
                    continue;
                }
            } else if (who >= 0x26 && who <= 0x4f) {
                m = func_02080de4(data_020f57c4, who, map);
                if (m == NULL) {
                    continue;
                }
            } else if (m->place != func_02055898(map)) {
                continue;
            }
            void *npc = func_020788d4(0x90);
            if (npc != NULL) {
                npc = func_02077a08(npc, who, m, func_0203fa10(self, who));
            }
            func_0203f988(self, (Npc *)npc);
            if (flag) {
                func_02078620(func_0203e5dc(gResidents, who));
            }
            break;
        }
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_0203cd40(Residents *self, int map) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0x10
    ldr r2, [pc, #0x2a0]
    mov r9, r1
    ldr r1, [r2, #0x0]
    mov r10, r0
    add r0, sp, #0x8
    str r1, [sp, #0x4]
    bl func_02035524
    mov r4, #0x0
_0203cd68:
    mov r0, r10
    mov r1, r4
    bl func_0203f264
    cmp r0, #0x0
    beq _0203cfdc
    ldr r0, [sp, #0x4]
    mov r1, r4
    bl func_02031b18
    cmp r0, #0x3
    beq _0203cfdc
    ldr r0, [sp, #0x4]
    mov r1, r4
    bl func_02031b18
    cmp r0, #0x4
    bne _0203ce54
    ldr r0, [pc, #0x248]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl func_0203e820
    mov r5, r0
    mov r0, r9
    bl func_02055898
    cmp r5, r0
    bne _0203cfdc
    ldr r0, [pc, #0x224]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl _ZN9Residents10IsResidentEi
    cmp r0, #0x0
    beq _0203ce00
    ldr r0, [pc, #0x20c]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl func_0203e820
    bl func_02055800
    add r1, r10, r4, lsl #0x2
    add r1, r1, #0x1000
    str r0, [r1, #0x120]
_0203ce00:
    mov r0, #0x90
    bl func_020788d4
    movs r6, r0
    beq _0203ce44
    mov r0, r10
    mov r1, r4
    bl func_0203e830
    mov r5, r0
    mov r0, r10
    mov r1, r4
    bl func_0203fa10
    mov r2, r5
    mov r3, r0
    mov r0, r6
    mov r1, r4
    bl func_02077a08
    mov r6, r0
_0203ce44:
    mov r0, r10
    mov r1, r6
    bl func_0203f988
    b _0203cfdc
_0203ce54:
    add r0, r10, #0x4
    add r5, r0, r4, lsl #0x4
    ldrsb r0, [r5, #0x4]
    mov r6, #0x0
    str r6, [sp, #0x0]
    cmp r0, #0x0
    ble _0203cfdc
    add r0, r10, r4, lsl #0x2
    ldr r8, [sp, #0x0]
    add r11, r0, #0x124
_0203ce7c:
    ldr r2, [r5, #0xc]
    mov r0, r10
    mov r1, r4
    add r7, r2, r8
    mov r2, r5
    mov r3, r6
    bl func_0203f6cc
    cmp r0, #0x0
    beq _0203cfc8
    ldr r0, [pc, #0x14c]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl _ZN9Residents10IsResidentEi
    cmp r0, #0x0
    beq _0203cec4
    ldr r0, [r7, #0x4]
    bl func_02055800
    str r0, [r11, #0xffc]
_0203cec4:
    ldr r0, [r7, #0x4]
    cmp r0, #0x2b
    bne _0203ceec
    mov r0, r9
    bl func_02055898
    cmp r0, #0x2a
    bne _0203cfc8
    mov r0, #0x1
    str r0, [sp, #0x0]
    b _0203cf64
_0203ceec:
    cmp r0, #0x2c
    bne _0203cf00
    cmp r9, #0x3c
    bne _0203cfc8
    b _0203cf64
_0203cf00:
    cmp r4, #0x74
    bne _0203cf20
    mov r0, r10
    mov r1, r9
    bl func_0203fa74
    movs r7, r0
    beq _0203cfc8
    b _0203cf64
_0203cf20:
    cmp r4, #0x26
    blt _0203cf50
    cmp r4, #0x4f
    bgt _0203cf50
    ldr r0, [pc, #0xc0]
    mov r1, r4
    ldr r0, [r0, #0x0]
    mov r2, r9
    bl func_02080de4
    movs r7, r0
    beq _0203cfc8
    b _0203cf64
_0203cf50:
    mov r0, r9
    bl func_02055898
    ldr r1, [r7, #0x4]
    cmp r1, r0
    bne _0203cfc8
_0203cf64:
    mov r0, #0x90
    bl func_020788d4
    movs r5, r0
    beq _0203cf98
    mov r0, r10
    mov r1, r4
    bl func_0203fa10
    mov r3, r0
    mov r0, r5
    mov r1, r4
    mov r2, r7
    bl func_02077a08
    mov r5, r0
_0203cf98:
    mov r0, r10
    mov r1, r5
    bl func_0203f988
    ldr r0, [sp, #0x0]
    cmp r0, #0x0
    beq _0203cfdc
    ldr r0, [pc, #0x3c]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl func_0203e5dc
    bl func_02078620
    b _0203cfdc
_0203cfc8:
    ldrsb r0, [r5, #0x4]
    add r6, r6, #0x1
    add r8, r8, #0x10
    cmp r6, r0
    blt _0203ce7c
_0203cfdc:
    add r4, r4, #0x1
    cmp r4, #0x89
    blt _0203cd68
    add sp, sp, #0x10
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_020f57c0
    dcd gResidents
    dcd data_020f57c4
}
#endif

// Is resident `who` on map `map` now? Gives its position.
BOOL func_0203cffc(Residents *self, int map, int who, u32 *x, u32 *y)
{
    void *gd = data_020f57c0;
    GameDay today;
    func_02035524(&today);
    if (!func_0203f264(self, who)) {
        return FALSE;
    }
    if (func_02031b18(gd, who) == 3) {
        return FALSE;
    }
    if (func_02031b18(gd, who) == 4) {
        return func_0203e820(gResidents, who) == func_02055898(map);
    }
    ScheduleEntry *e = &self->today[who];
    BOOL flag = FALSE;
    for (int i = 0; i < e->numMoves; i++) {
        ScheduleMove *m = &e->moves[i];
        if (!func_0203f6cc(self, who, e, i)) {
            continue;
        }
        if (_ZN9Residents10IsResidentEi(gResidents, who)) {
            self->places[who - 1] = func_02055800(m->place);
        }
        if (m->place == 0x2b) {
            if (func_02055898(map) != 0x2a) {
                continue;
            }
            flag = TRUE;
        } else if (m->place == 0x2c) {
            if (map != 0x3c) {
                continue;
            }
        } else if (who == 0x74) {
            m = func_0203fa74(self, map);
            if (m == NULL) {
                continue;
            }
        } else if (who >= 0x26 && who <= 0x4f) {
            m = func_02080de4(data_020f57c4, who, map);
            if (m == NULL) {
                continue;
            }
        } else if (m->place != func_02055898(map)) {
            continue;
        }
        *x = m->w8;
        *y = m->wc;
        return TRUE;
    }
    return FALSE;
}

void func_0203d1fc(Residents *self)
{
    if (data_020ec6ac.npcs != NULL) {
        ((TaskManager *)data_020ec6ac.npcs)->KillAll();
    }
}

// Talks to the resident touched on the touch screen.
void func_0203d21c(Residents *self)
{
    if (((s8 *)data_020f57c0)[0x106b8] == 1) {
        return;
    }
    if (func_02053ef4(*(void **)(func_0203f674(self) + 0x5c0))) {
        return;
    }
    Npc *npc = func_0203d324(self);
    if (npc == NULL) {
        return;
    }
    if (!npc->obj->V4()) {
        return;
    }
    if (*(int *)(func_0203f674(self) + 0x380) != -1) {
        func_ov002_021d0f8c(func_0203f674(self) + 0x14, npc->id + 0x175, 0, 0, 0, 0, 0, 0, 0);
    } else {
        func_ov002_021d0f8c(func_0203f674(self) + 0x14, npc->id + 0xee, 0, 0, 0, 0, 0, 0, 0);
    }
}

// The resident under the stylus (the nearest one if several).
#ifdef NONMATCHING
Npc *func_0203d324(Residents *self)
{
    Npc *hit[0x2b];
    int n;
    if (!(gInput.touchFlags & INPUT_TOUCH_PRESSED)) {
        return NULL;
    }
    if (((s8 *)data_020f57c0)[0x106b9] == 1) {
        return NULL;
    }
    if (func_ov002_021d11f4(func_0203f674(self) + 0x14)) {
        return NULL;
    }
    if (!func_0203f6a4(self)) {
        return NULL;
    }
    if (func_02093008((u8 *)func_0203f68c(self) + 0x388) >= 0) {
        return NULL;
    }
    if (func_ov002_0215b098(func_0203f68c(self))) {
        return NULL;
    }
    n = 0;
    NpcList *list = data_020ec6ac.npcs;
    for (Npc *npc = list->head->next; npc != list->head; npc = npc->next) {
        if (npc->obj->V4()) {
            int x0, x1, y0, y1;
            func_02078648(npc, &x0, &x1, &y0, &y1);
            if (x0 <= gInput.touchX && gInput.touchX <= x1 && y0 <= gInput.touchY && gInput.touchY <= y1) {
                hit[n++] = npc;
            }
        }
    }
    Npc *best = NULL;
    if (n != 0) {
        fx32 maxY = 0x80000000;
        for (int i = 0; i < n; i++) {
            Npc *npc = hit[i];
            if (npc->obj->GetPos()->z > maxY) {
                maxY = npc->obj->GetPos()->z;
                best = npc;
            }
        }
    }
    return best;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm Npc *func_0203d324(Residents *self) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0xc0
    ldr r1, [pc, #0x1b4]
    mov r4, r0
    ldrb r1, [r1, #0x0]
    tst r1, #0x8
    addeq sp, sp, #0xc0
    moveq r0, #0x0
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldr r2, [pc, #0x19c]
    ldr r1, [pc, #0x19c]
    ldr r2, [r2, #0x0]
    ldrsb r1, [r2, r1]
    cmp r1, #0x1
    addeq sp, sp, #0xc0
    moveq r0, #0x0
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    bl func_0203f674
    add r0, r0, #0x14
    bl func_ov002_021d11f4
    cmp r0, #0x0
    addne sp, sp, #0xc0
    movne r0, #0x0
    ldmneia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r0, r4
    bl func_0203f6a4
    cmp r0, #0x0
    addeq sp, sp, #0xc0
    moveq r0, #0x0
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r0, r4
    bl func_0203f68c
    add r0, r0, #0x388
    bl func_02093008
    cmp r0, #0x0
    addge sp, sp, #0xc0
    movge r0, #0x0
    ldmgeia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r0, r4
    bl func_0203f68c
    bl func_ov002_0215b098
    cmp r0, #0x0
    addne sp, sp, #0xc0
    movne r0, #0x0
    ldmneia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldr r0, [pc, #0x114]
    mov r9, #0x0
    ldr r8, [r0, #0xb4]
    ldr r0, [r8, #0x4]
    ldr r10, [r0, #0xc]
    cmp r10, r0
    beq _0203d478
    ldr r4, [pc, #0xfc]
    ldr r5, [pc, #0xfc]
    add r6, sp, #0x14
    add r7, sp, #0x4
    add r11, sp, #0x10
_0203d408:
    ldr r0, [r10, #0x84]
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x10]
    blx r1
    cmp r0, #0x0
    beq _0203d468
    mov r0, r10
    mov r1, r11
    add r2, sp, #0xc
    add r3, sp, #0x8
    str r7, [sp, #0x0]
    bl func_02078648
    ldrh r1, [r5, #0x0]
    ldr r0, [sp, #0x10]
    cmp r0, r1
    ldrle r0, [sp, #0xc]
    cmple r1, r0
    ldrleh r1, [r4, #0x0]
    ldrle r0, [sp, #0x8]
    cmple r0, r1
    ldrle r0, [sp, #0x4]
    cmple r1, r0
    strle r10, [r6, r9, lsl #0x2]
    addle r9, r9, #0x1
_0203d468:
    ldr r10, [r10, #0xc]
    ldr r0, [r8, #0x4]
    cmp r10, r0
    bne _0203d408
_0203d478:
    cmp r9, #0x0
    mov r8, #0x0
    beq _0203d4dc
    mov r6, r8
    cmp r9, #0x0
    mov r5, #0x80000000
    ble _0203d4dc
    add r4, sp, #0x14
_0203d498:
    ldr r7, [r4, r6, lsl #0x2]
    mov r0, r7
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    ldr r0, [r0, #0x8]
    cmp r0, r5
    ble _0203d4d0
    mov r0, r7
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    ldr r5, [r0, #0x8]
    mov r8, r7
_0203d4d0:
    add r6, r6, #0x1
    cmp r6, r9
    blt _0203d498
_0203d4dc:
    mov r0, r8
    add sp, sp, #0xc0
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd gInput
    dcd data_020f57c0
    dcd 0x106b9
    dcd data_020ec6ac
    dcd gInput+0x8
    dcd gInput+0xc
}
#endif

// Talks to the resident the player faces.
BOOL func_0203d500(Residents *self)
{
    Npc *npc = func_0203d5cc(self);
    if (npc != NULL && npc->obj->V4()) {
        if (*(int *)(func_0203f674(self) + 0x380) != -1) {
            func_ov002_021d0f8c(func_0203f674(self) + 0x14, npc->id + 0x175, 0, 0, 0, 0, 0, 0, 0);
        } else {
            func_ov002_021d0f8c(func_0203f674(self) + 0x14, npc->id + 0xee, 0, 0, 0, 0, 0, 0, 0);
        }
        return TRUE;
    }
    return FALSE;
}

// The nearest resident in front of the player (up to 3 steps of 4 units).
#ifdef NONMATCHING
Npc *func_0203d5cc(Residents *self)
{
    Found found[0x2b];
    VecFx32 dir;
    VecFx32 p;
    VecFx32 diff;
    int n;
    if (func_ov002_021d11f4(func_0203f674(self) + 0x14)) {
        return NULL;
    }
    if (((s8 *)data_020f57c0)[0x106b8] == 1) {
        return NULL;
    }
    n = 0;
    VEC_Normalize((VecFx32 *)((u8 *)func_0203f68c(self) + 0x224), &dir);
    NpcList *list = data_020ec6ac.npcs;
    for (Npc *npc = list->head->next; npc != list->head; npc = npc->next) {
        for (int i = 0; i < 3; i++) {
            fx32 d = FX_F32_TO_FX32(4.0f * (i + 1));
            VEC_MultAdd(d, &dir, func_0203f68c(self)->GetPos(), &p);
            if (func_0203d824(self, &p, ((FieldObjV *)npc)->GetPos())) {
                VEC_Subtract(((FieldObjV *)npc)->GetPos(), func_0203f68c(self)->GetPos(), &diff);
                found[n].npc = npc;
                found[n++].dist = VEC_Mag(&diff);
                break;
            }
        }
    }
    if (n == 0) {
        return NULL;
    }
    Npc *best = NULL;
    fx32 min = 0x7fffffff;
    for (int i = 0; i < n; i++) {
        if (found[i].dist < min) {
            min = found[i].dist;
            best = found[i].npc;
        }
    }
    return best;
}
#else
// Linker names used by the asm below.
extern "C" {
void _fadd(void);
void _ffix(void);
void _fflt(void);
void _fgr(void);
void _fmul(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm Npc *func_0203d5cc(Residents *self) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0x184
    mov r10, r0
    bl func_0203f674
    add r0, r0, #0x14
    bl func_ov002_021d11f4
    cmp r0, #0x0
    addne sp, sp, #0x184
    movne r0, #0x0
    ldmneia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldr r1, [pc, #0x218]
    ldr r0, [pc, #0x218]
    ldr r1, [r1, #0x0]
    ldrsb r0, [r1, r0]
    cmp r0, #0x1
    addeq sp, sp, #0x184
    moveq r0, #0x0
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    mov r1, #0x0
    mov r0, r10
    str r1, [sp, #0x0]
    bl func_0203f68c
    add r1, sp, #0x20
    add r0, r0, #0x224
    bl VEC_Normalize
    ldr r0, [pc, #0x1e4]
    ldr r0, [r0, #0xb4]
    str r0, [sp, #0x4]
    ldr r0, [r0, #0x4]
    ldr r8, [r0, #0xc]
    cmp r8, r0
    beq _0203d7b8
_0203d64c:
    ldr r6, [pc, #0x1cc]
    mov r7, #0x0
    sub r5, r6, #0xfb000000
    add r11, sp, #0x14
_0203d65c:
    add r0, r7, #0x1
    bl _fflt
    mov r1, r0
    mov r0, r6
    bl _fmul
    mov r1, #0x0
    bl _fgr
    add r0, r7, #0x1
    bls _0203d6ac
    bl _fflt
    mov r1, r0
    mov r0, r6
    bl _fmul
    mov r1, r0
    mov r0, r5
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _0203d6d0
_0203d6ac:
    bl _fflt
    mov r1, r0
    mov r0, r6
    bl _fmul
    mov r1, r0
    mov r0, r5
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_0203d6d0:
    mov r9, r0
    mov r0, r10
    bl func_0203f68c
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    mov r4, r0
    mov r0, r9
    bl _ffix
    mov r2, r4
    add r1, sp, #0x20
    mov r3, r11
    bl VEC_MultAdd
    mov r0, r8
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    mov r2, r0
    mov r0, r10
    mov r1, r11
    bl func_0203d824
    cmp r0, #0x0
    beq _0203d798
    mov r0, r8
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    mov r4, r0
    mov r0, r10
    bl func_0203f68c
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x1c]
    blx r1
    mov r1, r0
    mov r0, r4
    add r2, sp, #0x8
    bl VEC_Subtract
    ldr r1, [sp, #0x0]
    add r2, sp, #0x2c
    add r0, sp, #0x8
    str r8, [r2, r1, lsl #0x3]
    bl VEC_Mag
    ldr r1, [sp, #0x0]
    mov r2, r1, lsl #0x3
    add r1, r1, #0x1
    str r1, [sp, #0x0]
    add r1, sp, #0x2c
    add r1, r1, r2
    str r0, [r1, #0x4]
    b _0203d7a4
_0203d798:
    add r7, r7, #0x1
    cmp r7, #0x3
    blt _0203d65c
_0203d7a4:
    ldr r0, [sp, #0x4]
    ldr r8, [r8, #0xc]
    ldr r0, [r0, #0x4]
    cmp r8, r0
    bne _0203d64c
_0203d7b8:
    ldr r0, [sp, #0x0]
    cmp r0, #0x0
    addeq sp, sp, #0x184
    mov r0, #0x0
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldr r1, [sp, #0x0]
    mov r4, r0
    cmp r1, #0x0
    addle sp, sp, #0x184
    sub r3, r0, #0x80000001
    ldmleia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    add r2, sp, #0x2c
_0203d7e8:
    add r1, r2, r4, lsl #0x3
    ldr r1, [r1, #0x4]
    cmp r1, r3
    movlt r3, r1
    ldrlt r0, [r2, r4, lsl #0x3]
    ldr r1, [sp, #0x0]
    add r4, r4, #0x1
    cmp r4, r1
    blt _0203d7e8
    add sp, sp, #0x184
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_020f57c0
    dcd 0x106b8
    dcd data_020ec6ac
    dcd 0x40800000
}
#endif

BOOL func_0203d824(Residents *self, VecFx32 *a, VecFx32 *b)
{
    return VEC_Distance(a, b) <= 0x4800;
}


typedef struct Pet {
    u8 unk0[4];
    s16 love;
    u8 unk6[0x14 - 6];
} Pet;

// 6 entries at +0xe168 of the game data (pets?), friendship 0-1000.
#define HORSES ((Pet *)((u8 *)data_020f57c0 + 0xe168))

static inline void AddClamped(s16 *p, int n)
{
    *p += n;
    if (*p > 1000) {
        *p = 1000;
    }
    if (*p < 0) {
        *p = 0;
    }
}

// After event `kind` (a festival missed, ...), the residents concerned lose friendship (-100 or -300).
void func_0203d844(Residents *self, int kind, int item)
{
    int who;
    void *gd = data_020f57c0;
    for (who = 0; who < 0x89; who++) {
        if (!func_0203f1b4(gResidents, who)) {
            continue;
        }
        if (func_02031b18(gd, who) != 1) {
            continue;
        }
        switch (kind) {
        case 2:
            if (who < 0x22 || who > 0x4f) {
                func_020319ac(gd, who, -100);
            }
            break;
        case 1:
            if (who == 0x9) {
                func_020319ac(gd, 0x9, -100);
            }
            if (who == 0x1) {
                func_020319ac(gd, 0x1, -100);
            }
            if (who == 0x10) {
                func_020319ac(gd, 0x10, -100);
            }
            if (who == 0x11) {
                func_020319ac(gd, 0x11, -100);
            }
            if (who == 0x2) {
                func_020319ac(gd, 0x2, -100);
            }
            if (who == 0xa) {
                func_020319ac(gd, 0xa, -100);
            }
            if (who == 0x12) {
                func_020319ac(gd, 0x12, -100);
            }
            if (who == 0x13) {
                func_020319ac(gd, 0x13, -100);
            }
            if (who == 0x14) {
                func_020319ac(gd, 0x14, -100);
            }
            if (who == 0xc) {
                func_020319ac(gd, 0xc, -100);
            }
            if (who == 0x5e) {
                func_020319ac(gd, 0x5e, -100);
            }
            if (who == 0x5f) {
                func_020319ac(gd, 0x5f, -100);
            }
            if (who == 0x65) {
                func_020319ac(gd, 0x65, -100);
            }
            break;
        case 3:
            if (who == 0x15) {
                func_020319ac(gd, 0x15, -100);
            }
            if (who == 0x16) {
                func_020319ac(gd, 0x16, -100);
            }
            if (who == 0x4) {
                func_020319ac(gd, 0x4, -100);
            }
            if (who == 0x17) {
                func_020319ac(gd, 0x17, -100);
            }
            if (who == 0x3) {
                func_020319ac(gd, 0x3, -100);
            }
            if (who == 0xb) {
                func_020319ac(gd, 0xb, -100);
            }
            if (who == 0x5) {
                func_020319ac(gd, 0x5, -100);
            }
            if (who == 0xe) {
                func_020319ac(gd, 0xe, -100);
            }
            if (who == 0x6) {
                func_020319ac(gd, 0x6, -100);
            }
            if (who == 0x20) {
                func_020319ac(gd, 0x20, -100);
            }
            if (who == 0x21) {
                func_020319ac(gd, 0x21, -100);
            }
            if (who == 0x1f) {
                func_020319ac(gd, 0x1f, -100);
            }
            if (who == 0x1e) {
                func_020319ac(gd, 0x1e, -100);
            }
            if (who == 0x66) {
                func_020319ac(gd, 0x66, -100);
            }
            if (who == 0x6c) {
                func_020319ac(gd, 0x6c, -100);
            }
            if (who == 0x73) {
                func_020319ac(gd, 0x73, -100);
            }
            if (who == 0x74) {
                func_020319ac(gd, 0x74, -100);
            }
            if (who == 0x81) {
                func_020319ac(gd, 0x81, -100);
            }
            break;
        case 6:
            if (who == 0xf) {
                func_020319ac(gd, 0xf, -100);
            }
            if (who == 0x19) {
                func_020319ac(gd, 0x19, -100);
            }
            break;
        case 7:
            if (who == 0x17) {
                func_020319ac(gd, 0x17, -100);
            }
            if (who == 0x4) {
                func_020319ac(gd, 0x4, -100);
            }
            if (who == 0x65) {
                func_020319ac(gd, 0x65, -100);
            }
            if (who == 0x66) {
                func_020319ac(gd, 0x66, -100);
            }
            if (who == 0xd) {
                func_020319ac(gd, 0xd, -100);
            }
            break;
        case 5:
            if (who == 0x7) {
                func_020319ac(gd, 0x7, -100);
            }
            if (who == 0x1a) {
                func_020319ac(gd, 0x1a, -100);
            }
            if (who == 0x8) {
                func_020319ac(gd, 0x8, -100);
            }
            if (who == 0x18) {
                func_020319ac(gd, 0x18, -100);
            }
            if (who == 0x1b) {
                func_020319ac(gd, 0x1b, -100);
            }
            break;
        case 8:
            if (who == 0xd) {
                func_020319ac(gd, 0xd, -100);
            }
            break;
        case 11:
            func_02080d20(data_020f57c4, who, -100);
            break;
        case 21:
            if (who == 0x9) {
                func_020319ac(gd, 0x9, -300);
            }
            if (who == 0x1) {
                func_020319ac(gd, 0x1, -300);
            }
            if (who == 0x10) {
                func_020319ac(gd, 0x10, -300);
            }
            if (who == 0x11) {
                func_020319ac(gd, 0x11, -300);
            }
            break;
        case 23:
            if (who == 0x2) {
                func_020319ac(gd, 0x2, -300);
            }
            if (who == 0xa) {
                func_020319ac(gd, 0xa, -300);
            }
            if (who == 0x12) {
                func_020319ac(gd, 0x12, -300);
            }
            break;
        case 20:
            if (who == 0x13) {
                func_020319ac(gd, 0x13, -300);
            }
            if (who == 0x14) {
                func_020319ac(gd, 0x14, -300);
            }
            break;
        case 22:
            if (who == 0x15) {
                func_020319ac(gd, 0x15, -300);
            }
            if (who == 0x16) {
                func_020319ac(gd, 0x16, -300);
            }
            break;
        case 24:
        case 25:
            if (who == 0x17) {
                func_020319ac(gd, 0x17, -300);
            }
            if (who == 0x4) {
                func_020319ac(gd, 0x4, -300);
            }
            break;
        case 26:
            if (who == 0xc) {
                func_020319ac(gd, 0xc, -300);
            }
            break;
        case 27:
            if (who == 0x3) {
                func_020319ac(gd, 0x3, -300);
            }
            break;
        case 37:
            if (who == 0xb) {
                func_020319ac(gd, 0xb, -300);
            }
            break;
        case 12:
            if (who == 0x8) {
                func_020319ac(gd, 0x8, -300);
            }
            if (who == 0x18) {
                func_020319ac(gd, 0x18, -300);
            }
            break;
        case 38:
            if (who == 0xf) {
                func_020319ac(gd, 0xf, -300);
            }
            if (who == 0x19) {
                func_020319ac(gd, 0x19, -300);
            }
            break;
        case 39:
            if (who == 0xd) {
                func_020319ac(gd, 0xd, -300);
            }
            break;
        case 13:
            if (who == 0x7) {
                func_020319ac(gd, 0x7, -300);
            }
            if (who == 0x1a) {
                func_020319ac(gd, 0x1a, -300);
            }
            break;
        case 31:
            if (who == 0x5) {
                func_020319ac(gd, 0x5, -300);
            }
            if (who == 0xe) {
                func_020319ac(gd, 0xe, -300);
            }
            if (who == 0x6) {
                func_020319ac(gd, 0x6, -300);
            }
            if (who == 0x20) {
                func_020319ac(gd, 0x20, -300);
            }
            if (who == 0x21) {
                func_020319ac(gd, 0x21, -300);
            }
            if (who == 0x1f) {
                func_020319ac(gd, 0x1f, -300);
            }
            if (who == 0x1e) {
                func_020319ac(gd, 0x1e, -300);
            }
            if (who == 0x6c) {
                func_020319ac(gd, 0x6c, -300);
            }
            break;
        case 32:
            if (who == 0x1f) {
                func_020319ac(gd, 0x1f, -300);
            }
            if (who == 0x1e) {
                func_020319ac(gd, 0x1e, -300);
            }
            break;
        case 33:
            if (who == 0x20) {
                func_020319ac(gd, 0x20, -300);
            }
            if (who == 0x21) {
                func_020319ac(gd, 0x21, -300);
            }
            break;
        case 34:
            if (who == 0x5) {
                func_020319ac(gd, 0x5, -300);
            }
            break;
        case 35:
            if (who == 0xe) {
                func_020319ac(gd, 0xe, -300);
            }
            if (who == 0x6) {
                func_020319ac(gd, 0x6, -300);
            }
            break;
        case 36:
            if (who == 0x6c) {
                func_020319ac(gd, 0x6c, -300);
            }
            break;
        case 28:
            if (who == 0x73) {
                func_020319ac(gd, 0x73, -300);
            }
            break;
        case 29:
            if (who == 0x81) {
                func_020319ac(gd, 0x81, -300);
            }
            break;
        case 40:
            if (who == 0x65) {
                func_020319ac(gd, 0x65, -300);
            }
            break;
        case 41:
            if (who == 0x5f) {
                func_020319ac(gd, 0x5f, -300);
            }
            break;
        case 42:
            if (who == 0x5e) {
                func_020319ac(gd, 0x5e, -300);
            }
            break;
        case 43:
            if (who == 0x66) {
                func_020319ac(gd, 0x66, -300);
            }
            break;
        }
    }
    if (kind != 10) {
        return;
    }
    if (!func_0203e40c(self, item)) {
        return;
    }
    AddClamped(&HORSES[0].love, -5);
    AddClamped(&HORSES[1].love, -5);
    AddClamped(&HORSES[2].love, -5);
    AddClamped(&HORSES[3].love, -5);
    AddClamped(&HORSES[4].love, -5);
    AddClamped(&HORSES[5].love, -5);
}

// Does giving `item` count? (FALSE for 0x28-0x5c, 0x67-0x69, 0x14e-0x151, 0x186-0x18b)
BOOL func_0203e40c(Residents *self, int item)
{
    BOOL ret = TRUE;
    switch (item) {
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x14e:
    case 0x14f:
    case 0x150:
    case 0x151:
    case 0x186:
    case 0x187:
    case 0x188:
    case 0x189:
    case 0x18a:
    case 0x18b:
        ret = FALSE;
        break;
    }
    return ret;
}

Npc *func_0203e5dc(Residents *self, int who)
{
    NpcList *list = data_020ec6ac.npcs;
    if (list == NULL) {
        return NULL;
    }
    for (Npc *npc = list->head->next; npc != list->head; npc = npc->next) {
        if (who == npc->id) {
            return npc;
        }
    }
    return NULL;
}

// Sets today's schedule of resident `who`.
void func_0203e624(Residents *self, int who, ScheduleEntry *src)
{
    ScheduleEntry *t = self->today;
    t[who].day = src->day;
    ScheduleEntry *e = &t[who];
    e->weather = src->weather;
    e->season = src->season;
    e->numConds = src->numConds;
    e->numMoves = src->numMoves;
    if (src->numConds > 0) {
        e->conds = new u32[src->numConds];
        MI_CpuCopy8(src->conds, e->conds, src->numConds * sizeof(u32));
    } else {
        e->conds = NULL;
    }
    e->moves = new ScheduleMove[src->numMoves];
    MI_CpuCopy8(src->moves, e->moves, src->numMoves * sizeof(ScheduleMove));
}

void func_0203e6b8(Residents *self, int who)
{
    if (!func_0203f1b4(gResidents, who)) {
        return;
    }
    if (self->today[who].conds != NULL) {
        delete[] self->today[who].conds;
        self->today[who].conds = NULL;
    }
    if (self->today[who].moves != NULL) {
        delete[] self->today[who].moves;
        self->today[who].moves = NULL;
    }
    if (self->special[who].conds != NULL) {
        delete[] self->special[who].conds;
        self->special[who].conds = NULL;
    }
    if (self->special[who].moves != NULL) {
        delete[] self->special[who].moves;
        self->special[who].moves = NULL;
    }
}

void func_0203e760(Residents *self)
{
    for (int who = 0; who < 0x89; who++) {
        func_0203e6b8(self, who);
    }
}

// Sets the special schedule (day kind 10) of resident `who`.
void func_0203e788(Residents *self, int who, ScheduleEntry *src)
{
    ScheduleEntry *t = self->special;
    t[who].day = src->day;
    ScheduleEntry *e = &t[who];
    e->weather = src->weather;
    e->season = src->season;
    e->numConds = src->numConds;
    e->numMoves = src->numMoves;
    if (src->numConds > 0) {
        e->conds = new u32[src->numConds];
        MI_CpuCopy8(src->conds, e->conds, src->numConds * sizeof(u32));
    } else {
        e->conds = NULL;
    }
    e->moves = new ScheduleMove[src->numMoves];
    MI_CpuCopy8(src->moves, e->moves, src->numMoves * sizeof(ScheduleMove));
}

int func_0203e820(Residents *self, int who)
{
    return self->special[who].moves->place;
}

ScheduleMove *func_0203e830(Residents *self, int who)
{
    return self->special[who].moves;
}

void func_0203e83c(Residents *self, int who)
{
    void *gd = data_020f57c0;
    func_02031af0(gd, who, 1);
    func_02031b70(gd, who, 0);
    func_02031ce0(gd, who, 10);
    func_02031df8(gd, who, 1);
}

void func_0203e890(Residents *self, int who)
{
    void *gd = data_020f57c0;
    func_02031af0(gd, who, 2);
    func_02031d6c(gd, who, 0x1e);
}

void func_0203e8c4(Residents *self, int who)
{
    void *gd = data_020f57c0;
    func_02031af0(gd, who, 1);
    func_02031b70(gd, who, 0);
    func_02031ce0(gd, who, 10);
    func_02031d6c(gd, who, 0x1e);
}


// New day: friendship changes of every resident (gifts, talks, absences...).
void func_0203e918(Residents *self)
{
    GameDay day;
    GameDay today;
    int who;
    void *gd;
    s8 weekday;
    gd = data_020f57c0;
    func_02035524(&today);
    *(Words2 *)&day = *(Words2 *)&today;
    weekday = day.weekday;
    for (who = 0; who < 0x89; who++) {
        if (!func_0203f1b4(gResidents, who)) {
            continue;
        }
        if (who == 10 && who != func_0203366c(gd, 0x242)) {
            if (func_0203366c(gd, 0x2bb) != 4 && weekday != 1 && weekday != 2) {
                continue;
            }
        }
        if (who == 0x19 && !func_0203366c(gd, 0x196)) {
            if (weekday != 3 && weekday != 4) {
                continue;
            }
        }
        if (func_02031c18(gd, who) || func_02031ebc(gd, who)) {
            func_02031b70(gd, who, 0);
        } else {
            func_02031b98(gd, who);
        }
        if (func_02031c18(gd, who)) {
            if (who >= 0x22 && who <= 0x4f) {
                int w = func_02080be0(data_020f57c4, who);
                if (w == who) {
                    func_020319ac(gd, w, 100);
                }
            } else {
                func_020319ac(gd, who, 100);
            }
        }
        if (who == func_0203366c(gd, 0x242)) {
            if (func_02031bd4(gd, who) >= 5 && func_02035948(0x1e)) {
                func_020319ac(gd, who, -1000);
            }
        } else if (who != 10 || who != 0x19) {
            if (who >= 0x22 && who <= 0x4f) {
                switch (who) {
                case 0x22:
                    if (func_02035948(0x1e)) {
                        int i;
                        int w;
                        for (w = who, i = 0; i < 4; i++, w++) {
                            if (func_02031b18(data_020f57c0, w) && func_02031bd4(gd, w) >= 1) {
                                func_020319ac(gd, who, -100);
                                break;
                            }
                        }
                    }
                    break;
                case 0x26:
                case 0x2d:
                case 0x34:
                case 0x3b:
                case 0x42:
                case 0x49:
                    if (!func_02081178(data_020f57c4, who) && func_02035948(0x1e)) {
                        int i;
                        int w;
                        for (w = who, i = 0; i < 7; i++, w++) {
                            if (func_02031b18(data_020f57c0, w) && func_02031bd4(gd, w) >= 1) {
                                func_020319ac(gd, who, -100);
                                break;
                            }
                        }
                    }
                    break;
                }
            } else if (func_02031bd4(gd, who) >= 1 && func_02035948(0x1e)) {
                if ((u32)(who - 0x1c) > 1 || func_0203366c(gd, 0x24a)) {
                    func_020319ac(gd, who, -100);
                }
            }
        }
        if (func_02031bd4(gd, who) == 0) {
            func_02031ce0(gd, who, func_02031d50(gd, who) - 1);
        } else if (who >= 0x26 && who <= 0x4f) {
            if (!func_02081160(data_020f57c4, who)) {
                func_02031ce0(gd, who, func_02031d50(gd, who) + 1);
            }
        } else {
            func_02031ce0(gd, who, func_02031d50(gd, who) + 1);
        }
        if (func_02031b18(gd, who) == 2 && func_02031ddc(gd, who) >= 0) {
            func_02031d6c(gd, who, func_02031ddc(gd, who) - 1);
        }
        if (func_02031b18(gd, who) == 1) {
            func_02031e20(gd, who);
        }
        if (func_02031b18(gd, who) == 3 || func_02031b18(gd, who) == 4) {
            func_02031af0(gd, who, 1);
        }
        func_02031bf0(gd, who, 0);
        func_02031c40(gd, who, 0);
        func_02031c90(gd, who, 0);
        func_02031e94(gd, who, 0);
    }
    if (func_0203366c(gd, 1)) {
        func_0203189c(gd);
    }
    func_02081494(data_020f57c4);
    func_0203fb6c(self);
}


struct EventObj {
    u32 unk0;
};

void func_0203ee5c(Residents *self)
{
    EventObj ev;
    func_02044ed8(&ev);
    func_02044ef8(&ev);
    for (int who = 0; who < 0x89; who++) {
        if (func_0203f1b4(gResidents, who)) {
            func_0203f7d4(self, &ev, who);
        }
    }
    func_02044ee4(&ev);
}

// Chooses today's schedules of all residents.
void func_0203eec0(Residents *self)
{
    func_0203e760(self);
    ScheduleLoader loader;
    func_02038558(&loader);
    func_02038568(&loader);
    func_02038564(&loader);
}

void func_0203eee4(Residents *self, int who)
{
    func_0203e6b8(self, who);
    ScheduleLoader loader;
    func_02038558(&loader);
    func_02038580(&loader, who);
    func_02038564(&loader);
}

void func_0203ef18(Residents *self, int a, int b, int c, s8 d)
{
    int who = func_0204ed14(a);
    if (func_0203e5dc(gResidents, who) != NULL) {
        func_0203f76c(self, who);
    }
    void *npc = func_020788d4(0x90);
    if (npc != NULL) {
        func_02077dec(npc, a, b, c, d);
    }
}

void func_0203ef98(Residents *self, int who)
{
    if (func_0203e5dc(gResidents, who) != NULL) {
        func_0203f76c(self, who);
    }
}

// Is `who` the spouse, or one of the 6 marriage candidates?
BOOL func_0203efcc(Residents *self, int who)
{
    void *gd = data_020f57c0;
    if (func_0203366c(gd, 2)) {
        if (who == func_0203366c(gd, 0x242)) {
            return TRUE;
        }
    } else {
        if (func_0203366c(gd, 0)) {
            for (u32 i = 0; i < 6; i++) {
                if (who == data_020b2aa8[i]) {
                    return TRUE;
                }
            }
        } else {
            for (u32 i = 0; i < 6; i++) {
                if (who == data_020b2a90[i]) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

// Where is resident `who` now? (-1: nowhere)
#ifdef NONMATCHING
int func_0203f080(Residents *self, int who)
{
    if (func_02031b18(data_020f57c0, who) != 1) {
        return -1;
    }
    if (!func_0203f264(self, who)) {
        return -1;
    }
    if (func_02031b18(data_020f57c0, who) == 4) {
        return func_02055800(func_0203e820(self, who));
    }
    const s8 *t1 = (const s8 *)func_020351b0();
    const s8 *t2 = (const s8 *)func_020351b0();
    ScheduleEntry *e = &self->today[who];
    int n = e->numMoves;
    int now = t2[0] * 60 + t1[1];
    for (int i = 0; i < n; i++) {
        ScheduleMove *m = &e->moves[i];
        int start = m->b0 * 60 + m->b1;
        int end;
        if (i >= n - 1) {
            end = 30 * 60;
        } else {
            end = m[1].b0 * 60 + m[1].b1;
        }
        if (start <= now && now < end) {
            return func_02055800(e->moves[i].place);
        }
    }
    return func_02055800(e->moves[0].place);
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_0203f080(Residents *self, int who) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    ldr r2, [pc, #0x10c]
    mov r6, r0
    ldr r0, [r2, #0x0]
    mov r4, r1
    bl func_02031b18
    cmp r0, #0x1
    mvnne r0, #0x0
    ldmneia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    mov r0, r6
    mov r1, r4
    bl func_0203f264
    cmp r0, #0x0
    mvneq r0, #0x0
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    ldr r0, [pc, #0xd4]
    mov r1, r4
    ldr r0, [r0, #0x0]
    bl func_02031b18
    cmp r0, #0x4
    bne _0203f0e8
    mov r0, r6
    mov r1, r4
    bl func_0203e820
    bl func_02055800
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_0203f0e8:
    bl func_020351b0
    mov r5, r0
    bl func_020351b0
    add r1, r6, #0x4
    add r2, r1, r4, lsl #0x4
    ldrsb r4, [r2, #0x4]
    ldrsb r1, [r5, #0x1]
    ldrsb r3, [r0, #0x0]
    mov r0, #0x3c
    cmp r4, #0x0
    mla r1, r3, r0, r1
    mov ip, #0x0
    ble _0203f188
    ldr r5, [r2, #0xc]
    ldr r9, [pc, #0x74]
    sub lr, r4, #0x1
    mov r6, r0
    mov r7, r0
_0203f130:
    mov r8, ip, lsl #0x4
    add r0, r5, ip, lsl #0x4
    ldrsb r3, [r0, #0x1]
    ldrsb r8, [r5, r8]
    cmp ip, lr
    mla r3, r8, r7, r3
    movge r8, r9
    ldrltsb r8, [r0, #0x11]
    ldrltsb r0, [r0, #0x10]
    mlalt r8, r0, r6, r8
    cmp r3, r1
    bgt _0203f17c
    cmp r1, r8
    bge _0203f17c
    ldr r0, [r2, #0xc]
    add r0, r0, ip, lsl #0x4
    ldr r0, [r0, #0x4]
    bl func_02055800
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_0203f17c:
    add ip, ip, #0x1
    cmp ip, r4
    blt _0203f130
_0203f188:
    ldr r0, [r2, #0xc]
    ldr r0, [r0, #0x4]
    bl func_02055800
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    dcd data_020f57c0
    dcd 0x708
}
#endif

int func_0203f1a0(Residents *self, int who)
{
    return self->places[who - 1];
}

BOOL func_0203f1b4(Residents *self, int who)
{
    if (_ZN9Residents10IsResidentEi(self, who) || func_0203f248(self, who)) {
        return TRUE;
    }
    return FALSE;
}

BOOL _ZN9Residents10IsResidentEi(Residents *self, int who)
{
    if (who >= 1 && who < 0x22) {
        return TRUE;
    }
    if (who >= 0x89 && who <= 0xa0) {
        return TRUE;
    }
    if (who >= 0xc6 && who <= 0xd3) {
        return TRUE;
    }
    if (who >= 0xd4 && who <= 0xd6) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203f248(Residents *self, int who)
{
    if (who >= 0x22 && who < 0x88) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203f264(Residents *self, int who)
{
    if (!func_0203f1b4(self, who)) {
        return FALSE;
    }
    if (who == 0x1b) {
        return FALSE;
    }
    if (who >= 0x89 && who <= 0xa0) {
        return FALSE;
    }
    return TRUE;
}

BOOL func_0203f2a4(Residents *self, int who)
{
    switch (who) {
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x9:
    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x89:
    case 0x8a:
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f:
    case 0x90:
    case 0x91:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9a:
    case 0x9b:
    case 0x9c:
    case 0x9d:
    case 0x9e:
    case 0x9f:
    case 0xa0:
    case 0xc6:
    case 0xc7:
    case 0xc8:
    case 0xc9:
    case 0xca:
    case 0xcb:
    case 0xcc:
    case 0xcd:
    case 0xce:
    case 0xcf:
    case 0xd0:
    case 0xd1:
    case 0xd2:
    case 0xd3:
        return TRUE;
    }
    return FALSE;
}

int _ZN9Residents10GetPicDataEi(Residents *self, int who)
{
    int pic = 0;
    if (who >= 0x22 && who < 0x88) {
        if (who >= 0x22 && who <= 0x4f) {
            if (who <= 0x25) {
            } else if (who <= 0x2c) {
                pic = 1;
            } else if (who <= 0x33) {
                pic = 2;
            } else if (who <= 0x3a) {
                pic = 3;
            } else if (who <= 0x41) {
                pic = 4;
            } else if (who <= 0x48) {
                pic = 5;
            } else {
                pic = 6;
            }
        } else {
            pic = (u32)(who - 0x50) % 7;
        }
    } else if (who >= 0xd4 && who < 0xd6) {
        switch (*(int *)(func_0203f674(self) + 0x540)) {
        case 0x2d2:
        case 0x3c5:
            pic = 1;
            break;
        case 0x2d3:
        case 0x3c7:
            pic = 2;
            break;
        case 0x2cc:
        case 0x3c3:
            pic = 0;
            break;
        default:
            if (func_0203366c(data_020f57c0, 0x141)) {
                pic = 1;
            } else if (func_0203366c(data_020f57c0, 0x14b)) {
                pic = 0;
            } else if (func_0203366c(data_020f57c0, 0x146)) {
                pic = 2;
            } else {
                pic = 0;
            }
            break;
        }
    }
    return pic;
}

void func_0203f638(Residents *self, int a)
{
    func_0206fb5c(a);
}

BOOL func_0203f648(Residents *self, int who)
{
    if (who == 0x1c || who == 0x1d || (who >= 0x22 && who <= 0x4f)) {
        return FALSE;
    }
    return TRUE;
}

// The player (first task of the field scene).
u8 *func_0203f674(Residents *self)
{
    return (u8 *)(*(Task **)((u8 *)gSceneTaskManagers[1] + 4))->mNext;
}

FieldObjV *func_0203f68c(Residents *self)
{
    return (FieldObjV *)(*(Task **)((u8 *)data_020ec6ac.objs + 4))->mNext;
}

BOOL func_0203f6a4(Residents *self)
{
    return data_020ec6ac.objs->unk10 - data_020ec6ac.objs->unk14 > 0;
}

// Is move i of schedule e running now?
#ifdef NONMATCHING
BOOL func_0203f6cc(Residents *self, int who, ScheduleEntry *e, int i)
{
    const s8 *t1 = (const s8 *)func_020351b0();
    const s8 *t2 = (const s8 *)func_020351b0();
    ScheduleMove *moves = e->moves;
    int n = e->numMoves;
    ScheduleMove *m = &moves[i];
    int now = t2[0] * 60 + t1[1];
    int start = moves[i].b0 * 60 + m->b1;
    int end;
    if (i >= n - 1) {
        end = 6 * 60;
    } else {
        end = m[1].b0 * 60 + m[1].b1;
    }
    if (start < end) {
        if (now >= start && end > now) {
            return TRUE;
        }
        return FALSE;
    }
    if (start > end && now < start && end <= now) {
        return FALSE;
    }
    return TRUE;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL func_0203f6cc(Residents *self, int who, ScheduleEntry *e, int i) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r2
    mov r4, r3
    bl func_020351b0
    mov r5, r0
    bl func_020351b0
    ldr lr, [r6, #0xc]
    ldrsb r1, [r6, #0x4]
    mov r2, r4, lsl #0x4
    add ip, lr, r4, lsl #0x4
    ldrsb r6, [r5, #0x1]
    ldrsb r5, [r0, #0x0]
    mov r0, #0x3c
    sub r1, r1, #0x1
    ldrsb r3, [ip, #0x1]
    ldrsb r2, [lr, r2]
    mla r6, r5, r0, r6
    mla r3, r2, r0, r3
    cmp r4, r1
    movge r0, #0x168
    bge _0203f72c
    ldrsb r2, [ip, #0x11]
    ldrsb r1, [ip, #0x10]
    mla r0, r1, r0, r2
_0203f72c:
    cmp r3, r0
    bge _0203f74c
    cmp r6, r3
    blt _0203f744
    cmp r0, r6
    bgt _0203f764
_0203f744:
    mov r0, #0x0
    ldmia sp!, {r4, r5, r6, pc}
_0203f74c:
    ble _0203f764
    cmp r6, r3
    bge _0203f764
    cmp r0, r6
    movle r0, #0x0
    ldmleia sp!, {r4, r5, r6, pc}
_0203f764:
    mov r0, #0x1
    ldmia sp!, {r4, r5, r6, pc}
}
#endif

// Removes the spawned resident `who`.
void func_0203f76c(Residents *self, int who)
{
    Npc *npc = func_0203e5dc(gResidents, who);
    NpcList *list = data_020ec6ac.npcs;
    for (Npc *t = list->head->next; t != list->head; t = t->next) {
        if (npc == t) {
            Task *task = ((Task *)t)->mPrev->mNext;
            if (task != NULL) {
                delete task;
            }
            return;
        }
    }
}

void func_0203f7d4(Residents *self, EventObj *ev, int who)
{
    void *gd = data_020f57c0;
    switch (func_02031b18(gd, who)) {
    case 0:
        if (!func_02045044(ev, who)) {
            return;
        }
        if (_ZN9Residents10IsResidentEi(gResidents, who)) {
            int v = data_020b2ac0[who - 0xe];
            if (v != -1) {
                func_02033cf4(gd, v);
                return;
            }
        }
        func_0203e83c(self, who);
        break;
    case 1:
        if (_ZN9Residents10IsResidentEi(gResidents, who)) {
            if (func_02031f7c(gd, who) || func_02032080(gd, who)) {
                return;
            }
        }
        if (!func_020450ac(ev, who)) {
            return;
        }
        if (_ZN9Residents10IsResidentEi(gResidents, who)) {
            int v = data_020b2b38[who - 0xe];
            if (v != -1) {
                func_02033cf4(gd, v);
                return;
            }
        }
        func_0203e890(self, who);
        break;
    case 2:
        if (!func_0204514c(ev, who)) {
            return;
        }
        if (_ZN9Residents10IsResidentEi(gResidents, who)) {
            int v = data_020b2bb0[who - 0xe];
            if (v != -1) {
                func_02033cf4(gd, v);
                return;
            }
        }
        func_0203e8c4(self, who);
        break;
    }
}

static inline int GroupOf(int who)
{
    int g;
    if (who <= 0x2c) {
        g = 0;
    } else if (who <= 0x33) {
        g = 1;
    } else if (who <= 0x3a) {
        g = 2;
    } else if (who <= 0x41) {
        g = 3;
    } else if (who <= 0x48) {
        g = 4;
    } else {
        g = 5;
    }
    return g;
}

void func_0203f988(Residents *self, Npc *npc)
{
    int who = npc->id;
    if (who < 0x26) {
        return;
    }
    if (who > 0x4f) {
        return;
    }
    if (!func_020816b8(data_020f57c4, who)) {
        return;
    }
    self->groups[GroupOf(who)] = npc;
}

Npc *func_0203fa10(Residents *self, int who)
{
    if (who >= 0x26 && who <= 0x4f) {
        return self->groups[GroupOf(who)];
    }
    return NULL;
}

ScheduleMove *func_0203fa74(Residents *self, int map)
{
    for (u32 i = 0; i < 6; i++) {
        if (data_020b2c28[i].place == func_02055898(map)) {
            return &data_020b2c28[i];
        }
    }
    return NULL;
}

// Spawns resident 0x74 (who has places of his own, see func_0203fa74).
#ifdef NONMATCHING
void func_0203fab8(Residents *self, int map)
{
    int who = 0x74;
    ScheduleEntry *e = &self->today[who];
    for (int i = 0; i < e->numMoves; i++) {
        if (!func_0203f6cc(self, who, e, i)) {
            continue;
        }
        ScheduleMove *m = func_0203fa74(self, map);
        if (m == NULL) {
            continue;
        }
        void *npc = func_020788d4(0x90);
        if (npc != NULL) {
            npc = func_02077a08(npc, 0x74, m, func_0203fa10(self, 0x74));
        }
        func_0203f988(self, (Npc *)npc);
        return;
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_0203fab8(Residents *self, int map) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    mov r7, r0
    add r0, r7, #0x4
    mov r2, #0x74
    add r4, r0, r2, lsl #0x4
    ldrsb r0, [r4, #0x4]
    mov r6, r1
    mov r5, #0x0
    cmp r0, #0x0
    ldmleia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    mov r9, r2
_0203fae4:
    mov r0, r7
    mov r1, r9
    mov r2, r4
    mov r3, r5
    bl func_0203f6cc
    cmp r0, #0x0
    beq _0203fb58
    mov r0, r7
    mov r1, r6
    bl func_0203fa74
    movs r8, r0
    beq _0203fb58
    mov r0, #0x90
    bl func_020788d4
    movs r4, r0
    beq _0203fb48
    mov r0, r7
    mov r1, #0x74
    bl func_0203fa10
    mov r3, r0
    mov r0, r4
    mov r2, r8
    mov r1, #0x74
    bl func_02077a08
    mov r4, r0
_0203fb48:
    mov r0, r7
    mov r1, r4
    bl func_0203f988
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
_0203fb58:
    ldrsb r0, [r4, #0x4]
    add r5, r5, #0x1
    cmp r5, r0
    blt _0203fae4
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
}
#endif

// Friendship gained every day from the places visited (func_02082450).
void func_0203fb6c(Residents *self)
{
    GameDay day;
    GameDay today;
    int v;
    func_02035524(&today);
    *(Words2 *)&day = *(Words2 *)&today;
    v = func_02082450(data_020f57c4, 0x2a);
    if (func_0203366c(data_020f57c0, 2)) {
        func_020319ac(data_020f57c0, func_0203366c(data_020f57c0, 0x242), v);
    }
    if (func_0203366c(data_020f57c0, 1)) {
        func_020319ac(data_020f57c0, func_0203366c(data_020f57c0, 4) == 0 ? 0x1c : 0x1d, v);
    }
    v = func_02082450(data_020f57c4, 0x16);
    func_020319ac(data_020f57c0, 0x12, v);
    if (func_02032080(data_020f57c0, 0x2) == 0) {
        func_020319ac(data_020f57c0, 0x2, v);
    }
    if (func_02031b18(data_020f57c0, 10) && (u8)(s8)(day.weekday - 1) <= 1 &&
        func_02032080(data_020f57c0, 10) == 0) {
        func_020319ac(data_020f57c0, 10, v);
    }
    v = func_02082450(data_020f57c4, 0x13);
    func_020319ac(data_020f57c0, 0x13, v);
    func_020319ac(data_020f57c0, 0x14, v);
    v = func_02082450(data_020f57c4, 0x14);
    func_020319ac(data_020f57c0, 0x11, v);
    func_020319ac(data_020f57c0, 0x10, v);
    if (func_02032080(data_020f57c0, 0x1) == 0) {
        func_020319ac(data_020f57c0, 0x1, v);
    }
    if (func_02032080(data_020f57c0, 0x9) != 1) {
        func_020319ac(data_020f57c0, 0x9, v);
    }
    if (func_02032080(data_020f57c0, 0x2) == 2) {
        func_020319ac(data_020f57c0, 0x2, v);
    }
    v = func_02082450(data_020f57c4, 0x19);
    if (func_02032080(data_020f57c0, 0xc) != 1) {
        func_020319ac(data_020f57c0, 0xc, v);
    }
    if (func_02032080(data_020f57c0, 0x1) == 2) {
        func_020319ac(data_020f57c0, 0x1, v);
    }
    func_020319ac(data_020f57c0, 0x65, func_02082450(data_020f57c4, 0x26));
    func_020319ac(data_020f57c0, 0x5f, func_02082450(data_020f57c4, 0x27));
    func_020319ac(data_020f57c0, 0x5e, func_02082450(data_020f57c4, 0x28));
    func_020319ac(data_020f57c0, 0x66, func_02082450(data_020f57c4, 0x29));
    v = func_02082450(data_020f57c4, 0x18);
    func_020319ac(data_020f57c0, 0x17, v);
    if (func_02032080(data_020f57c0, 0x4) != 1) {
        func_020319ac(data_020f57c0, 0x4, v);
    }
    if (func_02032080(data_020f57c0, 0xa) == 2) {
        func_020319ac(data_020f57c0, 0xa, v);
    }
    v = func_02082450(data_020f57c4, 0x23);
    if (func_02032080(data_020f57c0, 0xb) == 0) {
        func_020319ac(data_020f57c0, 0xb, v);
    }
    v = func_02082450(data_020f57c4, 0x1a);
    if (func_02032080(data_020f57c0, 0x3) != 1) {
        func_020319ac(data_020f57c0, 0x3, v);
    }
    if (func_02032080(data_020f57c0, 0xb) == 2) {
        func_020319ac(data_020f57c0, 0xb, v);
    }
    v = func_02082450(data_020f57c4, 0x15);
    func_020319ac(data_020f57c0, 0x15, v);
    func_020319ac(data_020f57c0, 0x16, v);
    v = func_02082450(data_020f57c4, 0x1d);
    func_020319ac(data_020f57c0, 0x6c, v);
    if (func_02031b18(data_020f57c0, 0x21) == 1) {
        func_020319ac(data_020f57c0, 0x21, v);
    }
    if (func_02031b18(data_020f57c0, 0x20) == 1) {
        func_020319ac(data_020f57c0, 0x20, v);
    }
    if (func_02031b18(data_020f57c0, 0x1f) == 1) {
        func_020319ac(data_020f57c0, 0x1f, v);
    }
    if (func_02031b18(data_020f57c0, 0x1e) == 1) {
        func_020319ac(data_020f57c0, 0x1e, v);
    }
    if (func_02032080(data_020f57c0, 0xe) == 0) {
        func_020319ac(data_020f57c0, 0xe, v);
    }
    if (func_02032080(data_020f57c0, 0x6) == 0) {
        func_020319ac(data_020f57c0, 0x6, v);
    }
    if (func_02032080(data_020f57c0, 0x5) == 0) {
        func_020319ac(data_020f57c0, 0x5, v);
    }
    v = func_02082450(data_020f57c4, 0x25);
    if (func_02032080(data_020f57c0, 0xd) != 1) {
        func_020319ac(data_020f57c0, 0xd, v);
    }
    if (func_02032080(data_020f57c0, 0x5) == 2) {
        func_020319ac(data_020f57c0, 0x5, v);
    }
    v = func_02082450(data_020f57c4, 0x24);
    if (func_02032080(data_020f57c0, 0xf) == 0) {
        func_020319ac(data_020f57c0, 0xf, v);
    }
    if (func_02031b18(data_020f57c0, 0x19) && (u8)(s8)(day.weekday - 3) <= 1) {
        func_020319ac(data_020f57c0, 0x19, v);
    }
    v = func_02082450(data_020f57c4, 0xd);
    func_020319ac(data_020f57c0, 0x1a, v);
    if (func_02032080(data_020f57c0, 0x7) == 0) {
        func_020319ac(data_020f57c0, 0x7, v);
    }
    v = func_02082450(data_020f57c4, 0xc);
    func_020319ac(data_020f57c0, 0x18, v);
    func_020319ac(data_020f57c0, 0x8, v);
}

Residents *func_02040328(Residents *self)
{
    self->vtable = data_020bdae0;
    gResidents = self;
    return self;
}

Residents *func_02040344(Residents *self)
{
    gResidents = NULL;
    return self;
}

Residents *func_02040358(Residents *self)
{
    gResidents = NULL;
    return self;
}

Residents *func_0204036c(Residents *self)
{
    gResidents = NULL;
    operator delete(self);
    return self;
}

}
