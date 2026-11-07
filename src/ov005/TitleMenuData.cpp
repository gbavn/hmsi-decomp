// TitleMenu's constants (.rodata) and file names (.data). In the original these are TitleMenu.cpp's
// local array templates, constants and string literals; our mwcc sorts them differently (by size over
// the whole file, string literals in another order), so they are spelled out here as two structs in
// the original order, in units linked with TitleMenu.cpp's sections (TitleMenuData.cpp,
// TitleMenuPriorities.cpp, TitleMenuData2.cpp: the original's order is not a single size order). Layout workaround.
#include "ov005_menudata.h"

TitleMenuStrings gTitleMenuStrings = {
    "title_choice.NCBR", "title_button.NCBR", "title_choice.NANR", "title_choice.NCLR",
    "title_button.NANR", "title_button.NCER", "title_button.NCLR", "title_choice.NCER",
    "rom/effect/MENTALICON.NCBR", "rom/effect/MENTALICON.NCLR", "rom/effect/MENTALICON.NCER",
    "rom/effect/MENTALICON.NANR",
};

const TitleMenuRodata gTitleMenuRodata = {
    {0x20000, 0, 0x6000},
    {gTitleMenuStrings.choiceNcbr, gTitleMenuStrings.choiceNclr, gTitleMenuStrings.choiceNcer,
     gTitleMenuStrings.choiceNanr},
    {gTitleMenuStrings.buttonNcbr, gTitleMenuStrings.buttonNclr, gTitleMenuStrings.buttonNcer,
     gTitleMenuStrings.buttonNanr},
};
