// Schedules of the residents (ARM9 0x02038558..0x02039208). Names are ours.
//
// rom/schedule/schedule.xbb is an archive with one file per resident (0x89 residents; 0x1c and 0x1d
// have variants chosen by a game value). A file is a list of schedules: each has a day kind (weekday
// 0-6, 7, 8/9 resident states, 10 = special), weather and season filters (3 = any), a list of extra
// conditions (see func_02038c9c) and a list of 16-byte moves. The first schedule whose conditions hold
// today is given to the resident (gResidents).
#include <nitro.h>
#include "game_file.h"

extern "C" {

typedef struct ScheduleMove {
    s8 b0;
    s8 b1;
    s8 b2;
    s8 b3;
    u32 w4;
    u32 w8;
    u32 wc;
} ScheduleMove;

typedef struct ScheduleEntry {
    s8 day;                 // 0-6 weekday, 7, 8/9 resident state, 10 = special
    s8 weather;             // 3 = any
    s8 season;              // 3 = any
    s8 numConds;
    s8 numMoves;
    u32 *conds;
    ScheduleMove *moves;
} ScheduleEntry;

typedef struct ScheduleList {
    s8 count;
    ScheduleEntry *entries;
} ScheduleList;

typedef struct ScheduleLoader {
    u8 keep;                // use the schedule already chosen today
} ScheduleLoader;

typedef struct ArchiveView {
    void *vtable;
    void *data;
} ArchiveView;

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

extern void *data_020f57c0;         // the game data
extern void *gResidents;
extern char data_020b28a0[];        // "rom/schedule/schedule.xbb"

ArchiveView *_ZN7ArchiveC1EPv(ArchiveView *self, void *data);
void *func_02036fb0(ArchiveView *self, int i);
u32 func_02036ff8(ArchiveView *self, int i);
int func_020318b4(void *self);
int func_02031b18(void *self, int i);
void func_02031f40(void *self, int i, int value);
int func_02031f64(void *self, int i);
int func_02032080(void *self, int i);
int func_0203366c(void *self, int id);
const void *func_020351b0(void);
void func_02035524(GameDay *out);
int func_02035c5c(int a);
void func_0203e624(void *self, int who, ScheduleEntry *e);
void func_0203e788(void *self, int who, ScheduleEntry *e);
BOOL func_0203f264(void *self, int who);
BOOL func_02040ab8(void);
int func_02040bec(GameDay *day);
int func_020558a8(int a);
int func_020558b8(int a);

void func_020385ac(ScheduleLoader *self, int who, int keep);
void func_02038670(ScheduleLoader *self, ArchiveView *arc, int who, ScheduleList *list);
BOOL func_0203874c(ScheduleLoader *self, ArchiveView *arc, int who, ScheduleList *list);
void func_02038a10(ScheduleLoader *self, int who, ScheduleList *list, int i);
ScheduleEntry *func_02038aa0(ScheduleLoader *self, int who, int *out, ScheduleList *list);
void func_02038c08(ScheduleLoader *self, ScheduleList *list);
BOOL func_02038c9c(ScheduleLoader *self, int who, ScheduleEntry *e);

void func_02038558(ScheduleLoader *self)
{
    self->keep = 0;
}

void func_02038564(ScheduleLoader *self)
{
}

// Chooses the schedules of all residents.
void func_02038568(ScheduleLoader *self)
{
    self->keep = 0;
    func_020385ac(self, 0x89, 0);
}

void func_02038580(ScheduleLoader *self, int who)
{
    self->keep = 0;
    func_020385ac(self, who, 0);
}

void func_02038594(ScheduleLoader *self)
{
    self->keep = 1;
    func_020385ac(self, 0x89, 1);
}

void func_020385ac(ScheduleLoader *self, int who, int keep)
{
    ScheduleList list;
    MI_CpuFill8(&list, 0, sizeof(list));
    GFileObject file;
    u8 *data = (u8 *)GFile_Load(file.Get(), data_020b28a0, 0);
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, data);
    if (who == 0x89) {
        for (int i = 0; i < 0x89; i++) {
            func_02038670(self, &arc, i, &list);
        }
    } else {
        func_02038670(self, &arc, who, &list);
    }
    delete[] data;
}

void func_02038670(ScheduleLoader *self, ArchiveView *arc, int who, ScheduleList *list)
{
    if (!func_0203f264(gResidents, who)) {
        list->count = 0;
        list->entries = NULL;
        return;
    }
    if (func_0203874c(self, arc, who, list)) {
        int i;
        if (self->keep) {
            i = func_02031f64(data_020f57c0, who);
            if (i == -1) {
                func_02038aa0(self, who, &i, list);
            }
        } else {
            func_02038aa0(self, who, &i, list);
        }
        func_02038a10(self, who, list, i);
    }
    func_02038c08(self, list);
}

// Reads the schedules of resident `who`.
BOOL func_0203874c(ScheduleLoader *self, ArchiveView *arc, int who, ScheduleList *list)
{
    u8 *src;
    u32 size;
    int pos;
    u8 *buf;
    if (who == 0x1c) {
        who = func_020318b4(data_020f57c0) + 0x8a;
    } else if (who == 0x1d) {
        who = func_020318b4(data_020f57c0) + 0x8d;
    }
    src = (u8 *)func_02036fb0(arc, who);
    size = func_02036ff8(arc, who);
    buf = new u8[size];
    MI_CpuCopy8(src, buf, size);
    pos = 0;
    MI_CpuCopy8(buf, &list->count, 1);
    pos++;
    list->entries = (ScheduleEntry *)new u8[list->count * sizeof(ScheduleEntry)];
    for (int i = 0; i < list->count; i++) {
        ScheduleEntry *e = &list->entries[i];
        MI_CpuCopy8(&buf[pos], &e->day, 1);
        MI_CpuCopy8(&buf[pos + 1], &e->weather, 1);
        MI_CpuCopy8(&buf[pos + 2], &e->season, 1);
        MI_CpuCopy8(&buf[pos + 3], &e->numConds, 1);
        pos += 4;
        if (e->numConds > 0) {
            e->conds = new u32[e->numConds];
            for (int j = 0; j < e->numConds; j++) {
                MI_CpuCopy8(&buf[pos], &e->conds[j], 4);
                pos += 4;
            }
        } else {
            e->conds = NULL;
        }
        MI_CpuCopy8(&buf[pos], &e->numMoves, 1);
        pos++;
        e->moves = (ScheduleMove *)new u8[e->numMoves * sizeof(ScheduleMove)];
        for (int j = 0; j < e->numMoves; j++) {
            ScheduleMove *m = &e->moves[j];
            MI_CpuCopy8(&buf[pos], &m->b0, 1);
            MI_CpuCopy8(&buf[pos + 1], &m->b1, 1);
            MI_CpuCopy8(&buf[pos + 2], &m->w4, 4);
            MI_CpuCopy8(&buf[pos + 6], &m->w8, 4);
            MI_CpuCopy8(&buf[pos + 10], &m->wc, 4);
            MI_CpuCopy8(&buf[pos + 14], &m->b2, 1);
            MI_CpuCopy8(&buf[pos + 15], &m->b3, 1);
            pos += 16;
        }
    }
    delete[] buf;
    return TRUE;
}

void func_02038a10(ScheduleLoader *self, int who, ScheduleList *list, int i)
{
    func_0203e624(gResidents, who, &list->entries[i]);
    func_02031f40(data_020f57c0, who, i);
    for (int j = 0; j < list->count; j++) {
        ScheduleEntry *e = &list->entries[j];
        if (e->day == 10) {
            func_0203e788(gResidents, who, e);
        }
    }
}

// First schedule of resident `who` that can be used today.
ScheduleEntry *func_02038aa0(ScheduleLoader *self, int who, int *out, ScheduleList *list)
{
    void *gd;
    int x;
    ScheduleEntry *e;
    int state;
    int i;
    gd = data_020f57c0;
    state = func_02031b18(gd, who);
    x = func_02032080(gd, who);
    for (i = 0; i < list->count; i++) {
        e = &list->entries[i];
        s8 day = e->day;
        if (day == 8) {
            if (state != 0) {
                continue;
            }
        } else if (day == 9) {
            if (state != 2) {
                continue;
            }
        } else if (day == 7) {
            if (!func_02040ab8()) {
                continue;
            }
        } else if (day >= 0 && day <= 6) {
            GameDay today;
            func_02035524(&today);
            if (e->day != today.weekday) {
                continue;
            }
        } else if (day == 10) {
            continue;
        }
        if (e->weather != 3 && e->weather != func_020558a8(func_02035c5c(0))) {
            continue;
        }
        if (e->season != 3 && e->season != func_020558b8(x)) {
            continue;
        }
        if (func_02038c9c(self, who, e)) {
            if (out != NULL) {
                *out = i;
            }
            return e;
        }
    }
    return NULL;
}

void func_02038c08(ScheduleLoader *self, ScheduleList *list)
{
    for (int i = 0; i < list->count; i++) {
        if (list->entries[i].moves != NULL) {
            delete[] (u8 *)list->entries[i].moves;
            list->entries[i].moves = NULL;
        }
        if (list->entries[i].conds != NULL) {
            delete[] list->entries[i].conds;
            list->entries[i].conds = NULL;
        }
    }
    if (list->entries != NULL) {
        delete[] (u8 *)list->entries;
        list->entries = NULL;
    }
}

// Extra conditions of a schedule (game variables, flags, the day...).
BOOL func_02038c9c(ScheduleLoader *self, int who, ScheduleEntry *e)
{
    GameDay day;
    GameDay today;
    func_02035524(&today);
    *(Words2 *)&day = *(Words2 *)&today;
    func_020351b0();
    for (int i = 0; i < e->numConds; i++) {
        switch (e->conds[i]) {
        case 0:
            if (!func_0203366c(data_020f57c0, 0x66)) {
                return FALSE;
            }
            break;
        case 1:
            if (!func_0203366c(data_020f57c0, 0x196)) {
                return FALSE;
            }
            break;
        case 2:
            if (!func_02031b18(data_020f57c0, 8) || !func_02031b18(data_020f57c0, 0x18)) {
                return FALSE;
            }
            break;
        case 3:
            if (!func_0203366c(data_020f57c0, 0x99)) {
                return FALSE;
            }
            break;
        case 4:
            if (!func_0203366c(data_020f57c0, 0x89)) {
                return FALSE;
            }
            break;
        case 5:
            if (!func_0203366c(data_020f57c0, 0x18a)) {
                return FALSE;
            }
            break;
        case 6:
            if (!func_0203366c(data_020f57c0, 0x9d)) {
                return FALSE;
            }
            break;
        case 7:
            if (!func_0203366c(data_020f57c0, 0x72)) {
                return FALSE;
            }
            break;
        case 8:
            if (!func_0203366c(data_020f57c0, 0x7e)) {
                return FALSE;
            }
            break;
        case 9:
            if (!func_0203366c(data_020f57c0, 0x7a)) {
                return FALSE;
            }
            break;
        case 10:
            if (!func_0203366c(data_020f57c0, 0x91)) {
                return FALSE;
            }
            break;
        case 11:
            if (!func_0203366c(data_020f57c0, 0xa1)) {
                return FALSE;
            }
            break;
        case 12:
            if (!func_0203366c(data_020f57c0, 0x6e)) {
                return FALSE;
            }
            break;
        case 13:
            if (!func_0203366c(data_020f57c0, 0x95)) {
                return FALSE;
            }
            break;
        case 14:
            if (!func_0203366c(data_020f57c0, 0xa1)) {
                return FALSE;
            }
            break;
        case 15:
            if (!func_0203366c(data_020f57c0, 0x76)) {
                return FALSE;
            }
            break;
        case 16:
            if (!func_0203366c(data_020f57c0, 0x8d)) {
                return FALSE;
            }
            break;
        case 17:
            if (!func_0203366c(data_020f57c0, 0x65)) {
                return FALSE;
            }
            break;
        case 18:
            if (func_0203366c(data_020f57c0, 0x24a) != 2) {
                return FALSE;
            }
            break;
        case 19:
            if (func_0203366c(data_020f57c0, 0x24f) != 2) {
                return FALSE;
            }
            break;
        case 20:
            if (func_0203366c(data_020f57c0, 0x24f) != 3) {
                return FALSE;
            }
            break;
        case 21:
            if (!func_02031b18(data_020f57c0, 0x1b)) {
                return FALSE;
            }
            break;
        case 22:
            if (!func_0203366c(data_020f57c0, 0x82)) {
                return FALSE;
            }
            break;
        case 23:
            if (func_02040bec(&day) != 1) {
                return FALSE;
            }
            break;
        case 24:
            switch (func_02040bec(&day)) {
            case 0:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 18:
            case 20:
                break;
            default:
                return FALSE;
            }
            break;
        case 25: {
            int kind = func_02040bec(&day);
            if (kind != 10 && kind != 0x16) {
                return FALSE;
            }
            break;
        }
        case 26:
            if (!func_0203366c(data_020f57c0, 0x72) && func_0203366c(data_020f57c0, 0x2ba) != 4) {
                return FALSE;
            }
            break;
        case 27:
            if ((u8)(s8)(day.weekday - 1) <= 1) {
                return FALSE;
            }
            break;
        case 28:
            if (day.weekday != 1) {
                return FALSE;
            }
            break;
        case 29:
            if (day.weekday != 2) {
                return FALSE;
            }
            break;
        case 30:
            if (who != func_0203366c(data_020f57c0, 0x2e6)) {
                return FALSE;
            }
            break;
        }
    }
    return TRUE;
}

}
