// Overlay 9: only a big work buffer (0x4C000 bytes of .bss) and its getter. Names are ours.
#include <nitro/types.h>

static u8 sWork[0x4C000];

void *Ov009_GetWork(void)
{
    return sWork;
}
