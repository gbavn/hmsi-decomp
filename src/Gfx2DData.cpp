// The 2D graphics manager instance. In the original a global of Gfx2D.cpp placed before the
// vblank_count of <nitro/gx/gx.h>; our mwcc puts vblank_count first, so it lives in this unit.
class Gfx2D;

Gfx2D *gGfx2D;
