#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <nitro/types.h>

// A packed file archive loaded in memory (ARM9 0x02036f80..): look files up by name.
// Names are ours; vtable at 0x020bc954.
class Archive {
public:
    Archive(void *data);
    virtual ~Archive();
    void *GetFile(const char *name);
    u32 GetSize(const char *name);

    void *mData;
};

#endif
