// The 3D renderer instance. In the original a global of Model3D.cpp placed before the vblank_count of
// <nitro/gx/gx.h>; our mwcc puts vblank_count first, so it lives in this unit.
class Renderer3D;

Renderer3D *gRenderer3D;
