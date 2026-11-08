// Packed file archive in memory (ARM9 0x02036f80..0x02037238): files are found by the hash of their name
// (binary search in a sorted hash table, or a linear search). Names are ours; see include/game/Archive.h.
//
// The class methods are written as extern "C" functions with their linker names (the original compiler
// emitted only one constructor/destructor variant, ours would emit two).
#include <nitro.h>

extern "C" {

typedef struct ArchiveEntry {
    u32 offset;
    u32 size;
    u32 offset2;
    u32 hash;
} ArchiveEntry;

typedef struct ArchiveHashSlot {
    u32 hash;
    int index;
} ArchiveHashSlot;

// File header: count at +4, table type at +8 (0/1 = sorted hash table, 2/3 = linear), hash seed at +0xc,
// entries at +0x20.
typedef struct ArchiveHeader {
    u32 unk0;
    u32 count;
    u32 type;
    u32 seed;
    u8 unk10[0x10];
    ArchiveEntry entries[1];
} ArchiveHeader;

typedef struct ArchiveObj {
    void **vt;
    ArchiveHeader *data;
} ArchiveObj;

extern void *data_020bc954[];   // vtable
void _ZdlPv(void *ptr);

void func_02036fa8(ArchiveObj *self, void *data);
void *func_02036fb0(ArchiveObj *self, int i);
u32 func_02036ff8(ArchiveObj *self, int i);
int func_0203702c(ArchiveObj *self, const char *name);
ArchiveHeader *func_02037094(ArchiveObj *self);
ArchiveEntry *func_0203709c(ArchiveObj *self, u32 i);
ArchiveHashSlot *func_020370c0(ArchiveObj *self);
int func_020370e8(ArchiveObj *self, u32 hash);
u32 func_020371d4(const char *name, u32 seed);

ArchiveObj *_ZN7ArchiveC1EPv(ArchiveObj *self, void *data)
{
    self->vt = data_020bc954;
    self->data = NULL;
    func_02036fa8(self, data);
    return self;
}

void func_02036fa8(ArchiveObj *self, void *data)
{
    self->data = (ArchiveHeader *)data;
}

// File number `i`.
void *func_02036fb0(ArchiveObj *self, int i)
{
    ArchiveEntry *e = func_0203709c(self, i);

    if (e == NULL) {
        return NULL;
    }
    return (u8 *)func_02037094(self) + e->offset;
}

void *_ZN7Archive7GetFileEPKc(ArchiveObj *self, const char *name)
{
    return func_02036fb0(self, func_0203702c(self, name));
}

u32 func_02036ff8(ArchiveObj *self, int i)
{
    ArchiveEntry *e = func_0203709c(self, i);

    return e != NULL ? e->size : 0;
}

u32 _ZN7Archive7GetSizeEPKc(ArchiveObj *self, const char *name)
{
    return func_02036ff8(self, func_0203702c(self, name));
}

// Number of the file called `name`, -1 if missing.
int func_0203702c(ArchiveObj *self, const char *name)
{
    return func_020370e8(self, func_020371d4(name, func_02037094(self)->seed));
}

void *func_02037058(ArchiveObj *self, int i)
{
    ArchiveEntry *e = func_0203709c(self, i);

    if (e == NULL) {
        return NULL;
    }
    if (e->offset2 == 0) {
        return NULL;
    }
    return (u8 *)func_02037094(self) + e->offset2;
}

ArchiveHeader *func_02037094(ArchiveObj *self)
{
    return self->data;
}

ArchiveEntry *func_0203709c(ArchiveObj *self, u32 i)
{
    ArchiveHeader *h = func_02037094(self);

    if (i >= h->count) {
        return NULL;
    }
    return &h->entries[i];
}

ArchiveHashSlot *func_020370c0(ArchiveObj *self)
{
    ArchiveHeader *h = func_02037094(self);

    if (h->type != 0 && h->type != 1) {
        return NULL;
    }
    return (ArchiveHashSlot *)&h->entries[h->count];
}

#ifdef NONMATCHING
int func_020370e8(ArchiveObj *self, u32 hash)
{
    ArchiveHeader *h = func_02037094(self);
    ArchiveHashSlot *slots;
    ArchiveEntry *e;
    int lo;
    int hi;
    int mid;
    u32 i;

    switch (h->type) {
    case 0:
    case 1:
        hi = h->count - 1;
        lo = 0;
        slots = func_020370c0(self);
        while (TRUE) {
            mid = (lo + hi) / 2;
            if (hash == slots[mid].hash) {
                return slots[mid].index;
            }
            if (lo == hi) {
                break;
            }
            if (slots[mid].hash > hash) {
                hi = mid;
            } else if (lo == mid) {
                lo = hi;
            } else {
                lo = mid;
            }
        }
        break;
    case 2:
    case 3:
        e = func_0203709c(self, 0);
        for (i = 0; i < h->count; i++) {
            if (hash == e[i].hash) {
                return i;
            }
        }
        break;
    }
    return -1;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm int func_020370e8(ArchiveObj *self, u32 hash) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r0
    mov r5, r1
    bl func_02037094
    mov r4, r0
    ldr r0, [r4, #0x8]
    cmp r0, #0x3
    addls pc, pc, r0, lsl #0x2
    b _020371b4
_0203710c:
    b _0203711c
    b _0203711c
    b _02037174
    b _02037174
_0203711c:
    ldr r1, [r4, #0x4]
    mov r0, r6
    sub r6, r1, #0x1
    mov r4, #0x0
    bl func_020370c0
_02037130:
    add r1, r4, r6
    add r1, r1, r1, lsr #0x1f
    mov r2, r1, asr #0x1
    ldr r1, [r0, r2, lsl #0x3]
    cmp r5, r1
    addeq r0, r0, r2, lsl #0x3
    ldreq r0, [r0, #0x4]
    ldmeqia sp!, {r4, r5, r6, pc}
    cmp r4, r6
    beq _020371b4
    cmp r1, r5
    movhi r6, r2
    bhi _02037130
    cmp r4, r2
    moveq r4, r6
    movne r4, r2
    b _02037130
_02037174:
    mov r0, r6
    mov r1, #0x0
    bl func_0203709c
    ldr r1, [r4, #0x4]
    mov r2, #0x0
    cmp r1, #0x0
    bls _020371b4
_02037190:
    add r1, r0, r2, lsl #0x4
    ldr r1, [r1, #0xc]
    cmp r5, r1
    moveq r0, r2
    ldmeqia sp!, {r4, r5, r6, pc}
    ldr r1, [r4, #0x4]
    add r2, r2, #0x1
    cmp r2, r1
    blo _02037190
_020371b4:
    mvn r0, #0x0
    ldmia sp!, {r4, r5, r6, pc}
}
#endif

ArchiveObj *_ZN7ArchiveD1Ev(ArchiveObj *self)
{
    return self;
}

ArchiveObj *_ZN7ArchiveD0Ev(ArchiveObj *self)
{
    _ZdlPv(self);
    return self;
}

// Hash of a file name.
u32 func_020371d4(const char *name, u32 seed)
{
    int sum;
    u32 hash;
    int i;

    hash = 0;
    sum = 0;
    i = 0;

    while (name[i] != 0) {
        int c = name[i++];

        sum += c;
        hash += (c << (sum & 31)) | (c >> (32 - (sum & 31)));
        hash += (seed << (c & 31)) | (seed >> (32 - (c & 31)));
    }
    return hash;
}

}
