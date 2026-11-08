#ifndef SPRITE2D_H
#define SPRITE2D_H

// The game's 2D sprites (ARM9 0x0202b008..): a cell animation with its character/palette/cell/animation
// resources, drawn by the 2D renderer (gRenderer2D). Names are ours.
#include <nnsys/g2d.h>

struct Sprite2D {
    u16 numCells;
    s16 rotation;
    fx32 scaleX;
    fx32 scaleY;
    fx32 x;
    fx32 y;
    fx32 z;
    fx32 priority;
    u16 animSeq;
    u8 alpha;
    u8 unk1F;
    u8 nanrLoaded;
    u8 ncgrLoaded;
    u8 ncerLoaded;
    u8 unk23;
    void *ncgr;
    void *nclr;
    void *ncer;
    void *nanr;
    void *unk34;
    u32 unk38;
    void *unk3C;
    u32 transferHandle;
    void *unk44;
    const NNSG2dCellDataBank *cellBank;
    const NNSG2dAnimBankData *animBank;
    NNSG2dCellAnimation *cellAnim;
    void *unk54;
    u8 unk58[0x7C - 0x58];
    u32 unk7C;
    u8 unk80[0x94 - 0x80];
    void *unk94;
    u8 unk98[0x178 - 0x98];
    u16 flags;
    u16 unk17A;
};

#ifdef __cplusplus
extern "C" {
#endif
void func_0202b008(fx32 *a, fx32 *b, fx32 *c, fx32 *d, BOOL decA, BOOL decB);
void func_0202b054(Sprite2D *sprite);                 // stop the animation
void func_0202b064(Sprite2D *sprite, u16 seq);        // set the animation sequence
void func_0202b088(Sprite2D *sprite, fx32 speed);     // set the animation speed
BOOL func_0202b094(Sprite2D *sprite);                 // animation finished?
BOOL func_0202b0bc(Sprite2D *sprite, int seq);        // sequence plays in a loop?
u16 func_0202b0dc(Sprite2D *sprite);                  // current animation frame
void func_0202b0ec(Sprite2D *sprite, int priority);
void func_0202b0f8(Sprite2D *sprite, fx32 priority);
void func_0202b100(Sprite2D *sprite, int x, int y);
void func_0202b114(Sprite2D *sprite, int x);
void func_0202b120(Sprite2D *sprite, int y);
int func_0202b12c(Sprite2D *sprite);
int func_0202b138(Sprite2D *sprite);
void func_0202b144(Sprite2D *sprite, int dy);
BOOL func_0202b154(Sprite2D *sprite, const VecFx32 *pos);          // place at a 3D position
void func_0202b1a4(Sprite2D *sprite, fx32 x, fx32 y, fx32 z);
void func_0202b1d0(Sprite2D *sprite, u16 flags);
void func_0202b1e4(Sprite2D *sprite, u16 flags);
void func_0202b1f0(Sprite2D *sprite, u16 flags);
Sprite2D *func_0202b208(void *arc, const char *ncbr, const char *nclr, const char *ncer, const char *nanr, int x,
                       int y, int priority, int seq);
Sprite2D *func_0202b31c(void *ncbr, void *nclr, void *ncer, const char *ncbrKey, const char *nclrKey,
                        const char *ncerKey, s16 x, s16 y);
Sprite2D *func_0202b41c(void *ncbr, void *nclr, void *ncer, void *nanr, const char *ncbrKey, const char *nclrKey,
                        const char *ncerKey, const char *nanrKey, s16 x, s16 y);
Sprite2D *func_0202b56c(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, s16 x, s16 y);
Sprite2D *func_0202b718(const char *ncbr, const char *nclr, const char *ncer, int x, s16 y);
Sprite2D *func_0202b824(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, s16 x, s16 y,
                        BOOL keep);
Sprite2D *func_0202b970(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, int x, int y,
                        u8 keep);
void func_0202ba04(Sprite2D *sprite, BOOL keepData);                 // delete
void func_0202bb04(Sprite2D **sprites, int count, BOOL keepData);
#ifdef __cplusplus
}

// The list of sprites to draw this frame (an MSL std::vector<Sprite2D *> in the original).
struct SpriteVec {
    SpriteVec();
    ~SpriteVec();
    Sprite2D *&operator[](u32 i) { return data[i]; }
    u32 Size() { return size; }

    Sprite2D **data;
    u32 size;
    u32 capacity;
    u32 unkC;
};

// Registers itself as gRenderer2D (0x020ec87c).
class Renderer2DBase {
public:
    Renderer2DBase();
    virtual ~Renderer2DBase();
};

// The 2D sprite renderer (ARM9 0x0202bb48..).
class Renderer2D : public Renderer2DBase {
public:
    Renderer2D();
    virtual ~Renderer2D();

    u8 unk4;
    u32 unk8;
    u8 unkC[8];
    SpriteVec mSprites;
};

extern Renderer2D *gRenderer2D;
extern int gSpriteDepthStep;

extern "C" {
void func_0202bbd4(Renderer2D *renderer);                     // reserve the list
void func_0202bbf4(Renderer2D *renderer);                     // clear the list
void func_0202bc20(Renderer2D *renderer, Sprite2D *sprite);   // add a sprite (if on screen)
void func_0202bd2c(Renderer2D *renderer, Sprite2D **sprites, int count);
void func_0202bd70(Renderer2D *renderer);                     // advance the animations
void func_0202bdd4(Renderer2D *renderer);                     // draw
}
#endif

#endif
