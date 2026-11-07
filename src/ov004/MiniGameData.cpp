// The current minigame. In the original a global of MiniGame.cpp placed before the vblank_count of
// <nitro/gx/gx.h>; our mwcc puts vblank_count first, so it lives in this unit. Layout workaround.
class MiniGame;

MiniGame *gMiniGame;
