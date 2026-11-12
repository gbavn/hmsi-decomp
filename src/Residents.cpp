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

typedef struct FieldVars {
    u8 unk0[0xb4];
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

void func_02040328(Residents *self);
void func_02040344(Residents *self);
void func_02038558(ScheduleLoader *self);
void func_02038564(ScheduleLoader *self);
void func_02038594(ScheduleLoader *self);
int func_02031b18(void *self, int i);
void func_02035524(GameDay *out);
int func_02055800(int place);
int func_02055898(int a);
void *func_020788d4(u32 size);
void *func_02077a08(void *self, int who, ScheduleMove *m, void *pos);
void func_02078620(int a);
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
int func_0203e5dc(Residents *self, int who);
void func_0203e760(Residents *self);
int func_0203e820(Residents *self, int who);
ScheduleMove *func_0203e830(Residents *self, int who);
BOOL _ZN9Residents10IsResidentEi(Residents *self, int who);
BOOL func_0203f264(Residents *self, int who);
u8 *func_0203f674(Residents *self);
FieldObjV *func_0203f68c(Residents *self);
BOOL func_0203f6a4(Residents *self);
BOOL func_0203f6cc(Residents *self, int who, ScheduleEntry *e, int i);
void func_0203f988(Residents *self, void *npc);
void *func_0203fa10(Residents *self, int who);
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
            func_0203f988(self, npc);
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
            func_0203f988(self, npc);
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

}
