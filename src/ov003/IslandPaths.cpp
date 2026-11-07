// The island model paths used by IslandView (indexed by island number).
// In the original these are the string literals of IslandView.cpp's path table, but our mwcc orders
// those literals differently, so they are a struct here (fields keep their order), in a unit linked
// between IslandView.cpp and IslandViewTail.cpp.
#include "ov003_strings.h"

IslandPathStrings gIslandPathStrings = {
    "rom/island/sun", "rom/island/sun", "rom/island/vol", "rom/island/rice", "rom/island/fruit",
    "rom/island/hinata", "rom/island/goddes", "rom/island/wild_a", "rom/island/mushroom",
};
