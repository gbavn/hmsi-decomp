#ifndef XBBSPRITE_H
#define XBBSPRITE_H

#include <nitro/types.h>

// A character picture loaded from an .xbb file (ARM9 0x0203c0f0..). Names are ours; vtable at
// 0x020bda58 (only the destructor). Contains a file object at 0x08 (vtable 0x020bda70).
class XbbSprite {
public:
    XbbSprite();
    virtual ~XbbSprite();
    void Load(const char *path, int a, int b, int c, int d);   // 0x0203c254
    void UpdateAnim();                                          // 0x0203c564
    void SetAnim(int anim, int unk);                            // 0x0203c5d8

    u8 unk4[0x14 - 4];
    struct Sprite2D *mSprite;
    u8 unk18[0x2C - 0x18];
    fx32 mX;
    fx32 mY;
    fx32 mZ;
};

#endif
