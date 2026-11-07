#ifndef OV005_MENUDATA_H
#define OV005_MENUDATA_H

// TitleMenu's constants and file names, kept in TitleMenuData*.cpp as structs so that they come out
// in the original order (see there).
#include <nitro/types.h>
#include <nitro/fx/fx.h>

struct TitleMenuNames {
    const char *ncbr;
    const char *nclr;
    const char *ncer;
    const char *nanr;
};

struct TitleMenuIds {
    int ids[47];
};

// .rodata 0x0215b1e0 (TitleMenuData.cpp)
struct TitleMenuRodata {
    VecFx32 shipStart;
    TitleMenuNames choice;
    TitleMenuNames button;
};

// .rodata 0x0215b228 (TitleMenuData2.cpp)
struct TitleMenuRodata2 {
    VecFx32 shipMove;
    TitleMenuIds personIds;
    TitleMenuIds animalIds;
};

struct TitleMenuStrings {
    char choiceNcbr[20];
    char buttonNcbr[20];
    char choiceNanr[20];
    char choiceNclr[20];
    char buttonNanr[20];
    char buttonNcer[20];
    char buttonNclr[20];
    char choiceNcer[20];
    char mentalNcbr[28];
    char mentalNclr[28];
    char mentalNcer[28];
    char mentalNanr[28];
};

extern const TitleMenuRodata gTitleMenuRodata;
extern const int gTitleMenuPriorities[7];   // .rodata 0x0215b20c (TitleMenuPriorities.cpp)
extern const TitleMenuRodata2 gTitleMenuRodata2;
extern TitleMenuStrings gTitleMenuStrings;

#endif
