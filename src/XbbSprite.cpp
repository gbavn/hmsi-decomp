// Character pictures loaded from .xbb archives (ARM9 0x0203c04c..0x0203cc24). Names are ours; the class
// is declared in include/game/XbbSprite.h for the overlays.
//
// An .xbb archive holds, for each of its sprites, 3 files (characters, cells, animations) and then the
// palettes. All sprites share the VRAM of the one with the most characters (`biggest`).
//
// The methods are extern "C" functions with their linker names (see Archive.cpp). The sprites are kept
// in an array class (vtable 0x020bda70, like U32Array in PackFile.cpp) whose methods are emitted here;
// the classes below are "views" with declared-only virtuals to make the virtual calls.
#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/gfd.h>
#include "Gfx2D.h"
#include "Sprite2D.h"

class SpriteArray {
public:
    virtual SpriteArray *Dtor();
    virtual SpriteArray *DtorDelete();
    virtual Sprite2D **Begin();
    virtual Sprite2D **Begin2();
    virtual Sprite2D **End();
    virtual Sprite2D **End2();
    virtual u32 GetCount();
    virtual Sprite2D **At(u32 i);
    virtual Sprite2D **At2(u32 i);
    virtual BOOL IsEmpty();

    u32 count;
    Sprite2D **data;
};

struct XbbSprite {
    void *vtable;
    void **xbb;             // 0x04 resource handle: *xbb = the archive
    SpriteArray sprites;    // 0x08
    Sprite2D *cur;          // 0x14
    int anim;               // 0x18
    u32 biggest;            // 0x1c
    fx32 dx;                // 0x20
    fx32 dy;                // 0x24
    u8 hidden;              // 0x28
};

extern "C" {

typedef struct ArchiveView {
    void *vtable;
    void *data;
} ArchiveView;

extern void *data_020bda60[];       // XbbSprite vtable
extern void *data_020bda70[];       // SpriteArray vtable

ArchiveView *_ZN7ArchiveC1EPv(ArchiveView *self, void *data);
void *func_02036fb0(ArchiveView *self, int i);
void *func_02037058(ArchiveView *self, int i);
void **func_02043cdc(const char *path);
void func_02043d50(void ***handle);
void func_0202c9e8(void *proxy, void *chr, void *ncgr, u32 *vram, const char *key, void *crc, u8 *index,
                   u32 transferSize);
void func_0202cca8(void *proxy, void *chr, void *ncgr, u32 *transferKey, u32 *vram, const char *key, void *crc,
                   u8 *index);
void func_0202ce30(void *proxy, void *nclr, void *vram, const char *key);
void func_0202cff4(const NNSG2dCellDataBank **bank, void *ncer, u32 *a, const char *key, void *b, u8 *loaded);
void func_0202d150(const NNSG2dAnimBankData **bank, void *nanr, const char *key, void *a, u8 *loaded);
BOOL func_0202d284(void *crc, u8 index, u32 vram);
BOOL func_0202d33c(void *crc, u8 index);
BOOL func_0202d370(void *a);
BOOL func_0202d3d8(void *a, u8 loaded);
BOOL func_0202d40c(void *a, u8 loaded);

void func_0203c43c(XbbSprite *self);
void _ZN9XbbSprite7SetAnimEii(XbbSprite *self, int anim, int seq);
void func_0203c624(XbbSprite *self, ArchiveView *arc);
void func_0203c714(XbbSprite *self, ArchiveView *arc);
void func_0203c874(XbbSprite *self, ArchiveView *arc);
void func_0203c92c(XbbSprite *self, ArchiveView *arc, u32 i, int pal);
void func_0203c9a8(XbbSprite *self);
void *func_0203cac8(XbbSprite *self, ArchiveView *arc, int i, int type);
int func_0203cb1c(XbbSprite *self, ArchiveView *arc, int i, int type);
u32 func_0203cb68(XbbSprite *self);
SpriteArray *func_0203cbcc(SpriteArray *self);

static inline void ClearSprites(SpriteArray *a)
{
    if (!a->IsEmpty()) {
        delete[] a->data;
        a->count = 0;
    }
}

XbbSprite *func_0203c04c(XbbSprite *self)
{
    self->vtable = data_020bda60;
    self->xbb = NULL;
    *(void ***)&self->sprites = data_020bda70;
    self->sprites.count = 0;
    self->sprites.data = NULL;
    self->cur = NULL;
    self->anim = 0;
    self->biggest = 0;
    self->dx = 0;
    self->dy = 0;
    self->hidden = 1;
    return self;
}

SpriteArray *func_0203c094(SpriteArray *self)
{
    *(void ***)self = data_020bda70;
    ClearSprites(self);
    func_0203cbcc(self);
    return self;
}

BOOL func_0203c0dc(SpriteArray *self)
{
    return self->data == NULL;
}

XbbSprite *_ZN9XbbSpriteC2Ev(XbbSprite *self)
{
    self->vtable = data_020bda60;
    self->xbb = NULL;
    *(void ***)&self->sprites = data_020bda70;
    self->sprites.count = 0;
    self->sprites.data = NULL;
    self->cur = NULL;
    self->anim = 0;
    self->biggest = 0;
    self->dx = 0;
    self->dy = 0;
    self->hidden = 1;
    return self;
}

XbbSprite *_ZN9XbbSpriteD1Ev(XbbSprite *self)
{
    self->vtable = data_020bda60;
    func_0203c43c(self);
    *(void ***)&self->sprites = data_020bda70;
    ClearSprites(&self->sprites);
    func_0203cbcc(&self->sprites);
    return self;
}

XbbSprite *_ZN9XbbSpriteD0Ev(XbbSprite *self)
{
    self->vtable = data_020bda60;
    func_0203c43c(self);
    *(void ***)&self->sprites = data_020bda70;
    ClearSprites(&self->sprites);
    func_0203cbcc(&self->sprites);
    operator delete(self);
    return self;
}

XbbSprite *_ZN9XbbSpriteD2Ev(XbbSprite *self)
{
    self->vtable = data_020bda60;
    func_0203c43c(self);
    *(void ***)&self->sprites = data_020bda70;
    ClearSprites(&self->sprites);
    func_0203cbcc(&self->sprites);
    return self;
}

// Loads `count` sprites from an .xbb archive, all with palette `pal`, and shows animation `anim`/`seq`.
void _ZN9XbbSprite4LoadEPKciiii(XbbSprite *self, const char *path, int count, int pal, int anim, int seq)
{
    func_0203c43c(self);
    self->xbb = func_02043cdc(path);
    ClearSprites(&self->sprites);
    self->sprites.count = count;
    self->sprites.data = new Sprite2D *[count];
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, *self->xbb);
    func_0203c624(self, &arc);
    func_0203c714(self, &arc);
    func_0203c874(self, &arc);
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        func_0203c92c(self, &arc, i, pal);
    }
    func_0203c9a8(self);
    _ZN9XbbSprite7SetAnimEii(self, anim, seq);
}

// The same with one palette per sprite.
void func_0203c348(XbbSprite *self, const char *path, int count, int *pals, int anim, int seq)
{
    func_0203c43c(self);
    self->xbb = func_02043cdc(path);
    ClearSprites(&self->sprites);
    self->sprites.count = count;
    self->sprites.data = new Sprite2D *[count];
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, *self->xbb);
    func_0203c624(self, &arc);
    func_0203c714(self, &arc);
    func_0203c874(self, &arc);
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        func_0203c92c(self, &arc, i, pals[i]);
    }
    func_0203c9a8(self);
    _ZN9XbbSprite7SetAnimEii(self, anim, seq);
}

void func_0203c43c(XbbSprite *self)
{
    if (func_0203cb68(self) == 0) {
        return;
    }
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        Sprite2D *s = *self->sprites.At(i);
        if (s->transferHandle != (u32)-1) {
            gGfx2D->FreeTransferHandle(s->transferHandle);
            s->transferHandle = -1;
        }
        delete s->cellAnim;
        if (i == self->biggest) {
            func_0202d284(s->unk34, s->ncgrLoaded, s->unk7C);
        } else {
            func_0202d33c(s->unk34, s->ncgrLoaded);
        }
        func_0202d370(s->unk94);
        func_0202d3d8(s->unk3C, s->ncerLoaded);
        func_0202d40c((void *)s->unk44, s->nanrLoaded);
        delete s;
        *self->sprites.At(i) = NULL;
    }
    ClearSprites(&self->sprites);
    func_02043d50(&self->xbb);
}

Sprite2D **func_0203c558(SpriteArray *self, u32 i)
{
    return &self->data[i];
}

void _ZN9XbbSprite10UpdateAnimEv(XbbSprite *self)
{
    func_0202b1d0(self->cur, 0x100);
    self->cur->x += self->dx;
    self->cur->y += self->dy;
    if (self->hidden) {
        func_0202b1f0(self->cur, 1);
    } else {
        func_0202b1d0(self->cur, 1);
    }
    func_0202bc20(gRenderer2D, self->cur);
}

void _ZN9XbbSprite7SetAnimEii(XbbSprite *self, int anim, int seq)
{
    self->cur = *self->sprites.At(anim);
    self->anim = anim;
    func_0202b064(self->cur, seq);
    func_0202b054(self->cur);
    self->anim = anim;
}

void func_0203c624(XbbSprite *self, ArchiveView *arc)
{
    u32 big = 0;
    Sprite2D *bigSprite = NULL;
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        Sprite2D *s = new Sprite2D;
        void *ncer = func_0203cac8(self, arc, i, 1);
        func_0202cff4(&s->cellBank, ncer, &s->unk38, (const char *)func_02037058(arc, func_0203cb1c(self, arc, i, 1)),
                      &s->unk3C, &s->ncerLoaded);
        s->ncer = ncer;
        if (bigSprite == NULL || bigSprite->unk38 < s->unk38) {
            bigSprite = s;
            big = i;
        }
        *self->sprites.At(i) = s;
    }
    self->biggest = big;
}

void func_0203c714(XbbSprite *self, ArchiveView *arc)
{
    Sprite2D *big = *self->sprites.At(self->biggest);
    void *ncgr = func_0203cac8(self, arc, self->biggest, 0);
    func_0202c9e8(big->unk58, &big->unk54, ncgr, &big->unk7C,
                  (const char *)func_02037058(arc, func_0203cb1c(self, arc, self->biggest, 0)), &big->unk34,
                  &big->ncgrLoaded, big->unk38);
    big->ncgr = ncgr;
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        if (i == self->biggest) {
            continue;
        }
        Sprite2D *s = *self->sprites.At(i);
        void *chr = func_0203cac8(self, arc, i, 0);
        func_0202cca8(s->unk58, &s->unk54, chr, &big->unk7C, &s->unk7C,
                      (const char *)func_02037058(arc, func_0203cb1c(self, arc, i, 0)), &s->unk34, &s->ncgrLoaded);
        s->ncgr = chr;
    }
}

void func_0203c874(XbbSprite *self, ArchiveView *arc)
{
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        Sprite2D *s = *self->sprites.At(i);
        void *nanr = func_0203cac8(self, arc, i, 2);
        func_0202d150(&s->animBank, nanr, (const char *)func_02037058(arc, func_0203cb1c(self, arc, i, 2)),
                      &s->unk44, &s->nanrLoaded);
        s->nanr = nanr;
    }
}

void func_0203c92c(XbbSprite *self, ArchiveView *arc, u32 i, int pal)
{
    Sprite2D *s = *self->sprites.At(i);
    void *nclr = func_0203cac8(self, arc, pal, -1);
    func_0202ce30(s->unk80, nclr, &s->unk94, (const char *)func_02037058(arc, func_0203cb1c(self, arc, pal, -1)));
    s->nclr = nclr;
}

void func_0203c9a8(XbbSprite *self)
{
    for (u32 i = 0; i < func_0203cb68(self); i++) {
        Sprite2D *s = *self->sprites.At(i);
        s->transferHandle = gGfx2D->GetNewTransferHandle();
        s->cellAnim = NULL;
        s->cellAnim = new NNSG2dCellAnimation;
        Gfx2D *gfx = gGfx2D;
        NNSG2dCharacterData *chr = (NNSG2dCharacterData *)s->unk54;
        gfx->InitCellAnim(s->cellAnim, NNS_G2dGetAnimSequenceByIdx(s->animBank, 0), s->cellBank,
                          s->transferHandle, NNS_GfdGetTexKeyAddr(s->unk7C), NNS_G2D_VRAM_ADDR_NONE,
                          NNS_G2D_VRAM_ADDR_NONE, NULL, chr->pRawData, chr->szByte);
        s->z = 0;
        s->priority = 0;
        s->numCells = 0;
        s->rotation = 0;
        s->scaleX = FX32_ONE;
        s->scaleY = FX32_ONE;
        s->x = 0x80000;
        s->y = 0x60000;
        s->alpha = 31;
        func_0202b1e4(s, 2);
    }
}

// File `type` (0 characters, 1 cells, 2 animations, -1 palette i) of sprite i.
void *func_0203cac8(XbbSprite *self, ArchiveView *arc, int i, int type)
{
    int idx = 0;
    switch (type) {
    case 0:
    case 1:
    case 2:
        idx = type + i * 3;
        break;
    case -1:
        idx = i + func_0203cb68(self) * 3;
        break;
    }
    return func_02036fb0(arc, idx);
}

int func_0203cb1c(XbbSprite *self, ArchiveView *arc, int i, int type)
{
    int idx = 0;
    switch (type) {
    case 0:
    case 1:
    case 2:
        idx = type + i * 3;
        break;
    case -1:
        idx = i + func_0203cb68(self) * 3;
        break;
    }
    return idx;
}

u32 func_0203cb68(XbbSprite *self)
{
    return self->sprites.GetCount();
}

u32 func_0203cb7c(SpriteArray *self)
{
    return self->count;
}

Sprite2D **func_0203cb84(SpriteArray *self)
{
    return self->data;
}

Sprite2D **func_0203cb8c(SpriteArray *self)
{
    return self->data;
}

Sprite2D **func_0203cb94(SpriteArray *self)
{
    return self->data + self->count;
}

Sprite2D **func_0203cba4(SpriteArray *self)
{
    return self->data + self->count;
}

SpriteArray *func_0203cbb4(SpriteArray *self)
{
    return self;
}

SpriteArray *func_0203cbb8(SpriteArray *self)
{
    operator delete(self);
    return self;
}

SpriteArray *func_0203cbcc(SpriteArray *self)
{
    return self;
}

SpriteArray *func_0203cbd0(SpriteArray *self)
{
    *(void ***)self = data_020bda70;
    ClearSprites(self);
    operator delete(self);
    return self;
}

Sprite2D **func_0203cc18(SpriteArray *self, u32 i)
{
    return &self->data[i];
}

}
