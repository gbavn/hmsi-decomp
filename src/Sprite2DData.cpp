// Globals of Sprite2D.cpp. In the original they come before the vblank_count of <nitro/gx/gx.h>;
// our mwcc puts vblank_count first, so they live in this unit. Layout workaround.
class Renderer2D;

int gSpriteDepthStep;
Renderer2D *gRenderer2D;
