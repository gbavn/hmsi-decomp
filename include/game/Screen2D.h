#ifndef SCREEN2D_H
#define SCREEN2D_H

#include <nitro/types.h>

// Base class of the 2D (BG/OBJ) screens (ARM9 0x02049b30..). The constructor sets up the OAM
// managers, the destructor hides every OBJ on both screens. Names are ours.
class Screen2D {
public:
    Screen2D();
    virtual ~Screen2D();
};

#endif
