#ifndef OV003_STRINGS_H
#define OV003_STRINGS_H

// Strings that are literals in the original IslandView.cpp, kept as structs in their own units so
// they come out in the original order (see IslandPaths.cpp and IslandViewTail.cpp).

struct IslandPathStrings {
    char sun[16];
    char sun2[16];   // a second copy of "rom/island/sun" (island 3)
    char vol[16];
    char rice[16];
    char fruit[20];
    char hinata[20];
    char goddes[20];
    char wildA[20];
    char mushroom[20];
};

struct IslandViewStrings {
    char nsbmd[8];
    char nsbtp[8];
    char nsbta[8];
    char nsbca[8];
    char nsbma[8];
    char nsbva[8];
    char camera[8];   // the joint name searched by IslandView::LoadResources
};

extern IslandPathStrings gIslandPathStrings;
extern IslandViewStrings gIslandViewStrings;

#endif
