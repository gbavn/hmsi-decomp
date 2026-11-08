// Packs of files read by index (ARM9 0x0202e810..0x0202f140). Names are ours.
//
// A pack starts with a u32 count followed by count+1 u32 offsets (the last one may be missing: then the
// file length is the end). Three kinds share one interface (vtable slots: dtor, dtor+delete, IsEmpty,
// GetCount, GetSize, Read, GetOffset, IsValid):
// - PackFile (the C++ name the overlays use): read from the file on demand.
// - OffsetPack: the same, with the offset table in memory.
// - MemPack: the whole pack in memory (a Buffer).
// The original is C++ with abstract base classes; the classes are written here as structs with explicit
// vtables (one table for the whole .data of the file) so that functions and data keep the original order.
#include <nitro.h>
#include "game_file.h"

extern "C" void func_0201f668(void *dst, const void *src, u32 size);   // memmove

// The classes are only used to make virtual calls: their virtual functions are declared but never
// defined as members (the real ones are the extern "C" functions below, put in the explicit vtables),
// so mwcc emits no vtables or inline functions for them.
class Buffer {
public:
    virtual Buffer *Dtor();
    virtual Buffer *DtorDelete();
    virtual u8 *GetData();
    virtual u8 *Begin();
    virtual u8 *End();
    virtual u8 *End2();
    virtual u32 GetSize();
    virtual u8 *At(u32 offset);
    virtual u8 *At2(u32 offset);
    virtual BOOL IsEmpty();

    u32 size;
    u8 *data;
};

class U32Array {
public:
    virtual U32Array *Dtor();
    virtual U32Array *DtorDelete();
    virtual u32 *Begin();
    virtual u32 *Begin2();
    virtual u32 *End();
    virtual u32 *End2();
    virtual u32 GetCount();
    virtual u32 *At(u32 i);
    virtual u32 *At2(u32 i);
    virtual BOOL IsEmpty();

    u32 count;
    u32 *data;
};

class Pack {
public:
    virtual Pack *Dtor();
    virtual Pack *DtorDelete();
    virtual BOOL IsEmpty();
    virtual u32 GetCount();
    virtual u32 GetSize(u32 index);
    virtual void Read(u32 index, u32 maxSize, void *dst);
    virtual u32 GetOffset(u32 index);
    virtual BOOL IsValid(u32 index);
};

// Base of the packs read from a file.
class FilePackBase : public Pack {
public:
    FSFileID fileId;
};

class PackFile : public FilePackBase {
public:
    u32 count;
};

class OffsetPack : public FilePackBase {
public:
    U32Array offsets;
};

class MemPack : public Pack {
public:
    Buffer buf;
};

extern "C" {
// The .data of the file: a GFile vtable copy header, then (each after an 8-byte header) the vtables of
// PackFile, OffsetPack, U32Array, U32Array's abstract base, FilePackBase, MemPack, Buffer, Buffer's
// abstract base, the pack interface, and the (unused) GFile vtable copy.
extern void *gPackVtables[0x198 / 4];
void func_0202aec0(void);
void func_0202afe0(void);

Buffer *func_0202ee5c(Buffer *self);
U32Array *func_0202ee78(U32Array *self);
}

#define VT_PACKFILE (&gPackVtables[2])
#define VT_U32ARRAY (&gPackVtables[22])
#define VT_FILEPACKBASE (&gPackVtables[46])
#define VT_BUFFER (&gPackVtables[66])
#define SET_VT(obj, vt) (*(void ***)(obj) = (vt))

static inline void CopyFileId(FSFileID *dst, const FSFileID *src)
{
    *dst = *src;
}

static inline void CopyBytes(const u8 *first, const u8 *last, u8 *result)
{
    func_0201f668(result, first, last - first);
}

extern "C" {

Buffer *func_0202e810(Buffer *self)
{
    SET_VT(self, VT_BUFFER);
    if (!self->IsEmpty()) {
        delete[] self->data;
        self->size = 0;
    }
    func_0202ee5c(self);
    return self;
}

BOOL func_0202e858(Buffer *self)
{
    return self->data == NULL;
}

u8 *func_0202e86c(Buffer *self)
{
    return self->data;
}

u32 func_0202e874(MemPack *self, u32 index)
{
    u32 start = self->GetOffset(index);

    return self->GetOffset(index + 1) - start;
}

u32 func_0202e8ac(MemPack *self, u32 index)
{
    return *(u32 *)self->buf.At2((index + 1) * 4);
}

u8 *func_0202e8cc(Buffer *self, u32 offset)
{
    return self->data + offset;
}

void func_0202e8d8(MemPack *self, u32 index, u32 maxSize, void *dst)
{
    u8 *src = self->buf.GetData() + self->GetOffset(index);

    CopyBytes(src, src + self->GetSize(index), (u8 *)dst);
}

#ifdef NONMATCHING
FilePackBase *func_0202e940(FilePackBase *self, const char *path)
{
    SET_VT(self, VT_FILEPACKBASE);
    CopyFileId(&self->fileId, &GFile_GetFileID(path));
    return self;
}
#else
void _Z15GFile_GetFileIDPKc(void);

// The original loads both words of the FSFileID before storing them.
asm FilePackBase *func_0202e940(FilePackBase *self, const char *path)
{
    stmdb sp!, {r4, lr}
    sub sp, sp, #0x8
    ldr r2, [pc, #0x2c]
    mov r4, r0
    add r0, sp, #0x0
    str r2, [r4, #0x0]
    bl _Z15GFile_GetFileIDPKc
    add r1, sp, #0x0
    ldr r2, [r1, #0x0]
    ldr r1, [r1, #0x4]
    mov r0, r4
    str r2, [r4, #0x4]
    str r1, [r4, #0x8]
    add sp, sp, #0x8
    ldmia sp!, {r4, pc}
    dcd gPackVtables+0xb8
}
#endif

void func_0202e980(FilePackBase *self, u32 index, u32 maxSize, void *dst)
{
    GFileObject file;
    u32 size;

    GFile_OpenFast(file.Get(), &self->fileId);
    GFile_Seek(file.Get(), self->GetOffset(index), FS_SEEK_SET);
    size = self->GetSize(index);
    if (maxSize <= size) {
        size = maxSize;
    }
    GFile_Read(file.Get(), size, dst);
    DC_FlushRange(dst, size);
}

u32 func_0202ea30(OffsetPack *self, u32 index)
{
    u32 *next = self->offsets.At2(index + 1);
    u32 *cur = self->offsets.At2(index);

    return *next - *cur;
}

u32 *func_0202ea78(U32Array *self, u32 i)
{
    return &self->data[i];
}

u32 func_0202ea84(OffsetPack *self, u32 index)
{
    return *self->offsets.At2(index);
}

BOOL func_0202ea9c(OffsetPack *self, u32 index)
{
    if (!self->IsEmpty() && index < self->offsets.GetCount() - 1) {
        return TRUE;
    }
    return FALSE;
}

u32 func_0202eae4(U32Array *self)
{
    return self->count;
}

BOOL func_0202eaec(OffsetPack *self)
{
    return self->offsets.IsEmpty();
}

BOOL func_0202eb00(U32Array *self)
{
    return self->data == NULL;
}

U32Array *func_0202eb14(U32Array *self)
{
    SET_VT(self, VT_U32ARRAY);
    if (!self->IsEmpty()) {
        delete[] self->data;
        self->count = 0;
    }
    func_0202ee78(self);
    return self;
}

PackFile *_ZN8PackFileC1EPKc(PackFile *self, const char *path)
{
    func_0202e940((FilePackBase *)self, path);
    SET_VT(self, VT_PACKFILE);
    self->count = 0;
    {
        GFileObject file;

        GFile_OpenFast(file.Get(), &self->fileId);
        GFile_Read(file.Get(), 4, &self->count);
        DC_FlushRange(&self->count, 4);
    }
    return self;
}

u32 func_0202ebd8(PackFile *self, u32 index)
{
    GFileObject file;
    u32 offsets[2];

    GFile_OpenFast(file.Get(), &self->fileId);
    if (index == 0 && self->count == 1) {
        GFile_Seek(file.Get(), 4, FS_SEEK_SET);
        GFile_Read(file.Get(), 4, offsets);
        return GFile_GetLength(file.Get()) - offsets[0];
    }
    if (index == self->count - 1) {
        GFile_Seek(file.Get(), (index + 1) * 4, FS_SEEK_SET);
        GFile_Read(file.Get(), 4, offsets);
        return GFile_GetLength(file.Get()) - offsets[0];
    }
    GFile_Seek(file.Get(), (index + 1) * 4, FS_SEEK_SET);
    GFile_Read(file.Get(), 8, offsets);
    DC_FlushRange(offsets, 8);
    return offsets[1] - offsets[0];
}

u32 func_0202ed08(PackFile *self, u32 index)
{
    GFileObject file;
    u32 offset;

    GFile_OpenFast(file.Get(), &self->fileId);
    GFile_Seek(file.Get(), (index + 1) * 4, FS_SEEK_SET);
    GFile_Read(file.Get(), 4, &offset);
    DC_FlushRange(&offset, 4);
    return offset;
}

BOOL func_0202ed88(PackFile *self, u32 index)
{
    return index < self->count;
}

PackFile *func_0202ed9c(PackFile *self)
{
    operator delete(self);
    return self;
}

PackFile *func_0202edb0(PackFile *self)
{
    return self;
}

OffsetPack *func_0202edb4(OffsetPack *self)
{
    SET_VT(&self->offsets, VT_U32ARRAY);
    if (!self->offsets.IsEmpty()) {
        delete[] self->offsets.data;
        self->offsets.count = 0;
    }
    operator delete(self);
    return self;
}

OffsetPack *func_0202ee00(OffsetPack *self)
{
    SET_VT(&self->offsets, VT_U32ARRAY);
    if (!self->offsets.IsEmpty()) {
        delete[] self->offsets.data;
        self->offsets.count = 0;
    }
    return self;
}

FilePackBase *func_0202ee44(FilePackBase *self)
{
    operator delete(self);
    return self;
}

FilePackBase *func_0202ee58(FilePackBase *self)
{
    return self;
}

Buffer *func_0202ee5c(Buffer *self)
{
    return self;
}

Buffer *func_0202ee60(Buffer *self)
{
    operator delete(self);
    return self;
}

Buffer *func_0202ee74(Buffer *self)
{
    return self;
}

U32Array *func_0202ee78(U32Array *self)
{
    return self;
}

U32Array *func_0202ee7c(U32Array *self)
{
    operator delete(self);
    return self;
}

U32Array *func_0202ee90(U32Array *self)
{
    return self;
}

u8 *func_0202ee94(Buffer *self, u32 offset)
{
    return self->data + offset;
}

u8 *func_0202eea0(Buffer *self)
{
    return self->data + self->size;
}

u8 *func_0202eeb0(Buffer *self)
{
    return self->data + self->size;
}

u8 *func_0202eec0(Buffer *self)
{
    return self->data;
}

u32 *func_0202eec8(U32Array *self, u32 i)
{
    return &self->data[i];
}

u32 *func_0202eed4(U32Array *self)
{
    return self->data + self->count;
}

u32 *func_0202eee4(U32Array *self)
{
    return self->data + self->count;
}

u32 *func_0202eef4(U32Array *self)
{
    return self->data;
}

u32 *func_0202eefc(U32Array *self)
{
    return self->data;
}

Pack *func_0202ef04(Pack *self)
{
    return self;
}

Pack *func_0202ef08(Pack *self)
{
    operator delete(self);
    return self;
}

Buffer *func_0202ef1c(Buffer *self)
{
    SET_VT(self, VT_BUFFER);
    if (!self->IsEmpty()) {
        delete[] self->data;
        self->size = 0;
    }
    operator delete(self);
    return self;
}

u32 func_0202ef64(Buffer *self)
{
    return self->size;
}

MemPack *func_0202ef6c(MemPack *self)
{
    SET_VT(&self->buf, VT_BUFFER);
    if (!self->buf.IsEmpty()) {
        delete[] self->buf.data;
        self->buf.size = 0;
    }
    return self;
}

BOOL func_0202efb0(MemPack *self)
{
    BOOL empty = TRUE;

    if (!self->buf.IsEmpty()) {
        if (self->buf.GetSize() != 0) {
            empty = FALSE;
        }
    }
    return empty;
}

u32 func_0202eff4(MemPack *self)
{
    if (self->IsEmpty()) {
        return 0;
    }
    return *(u32 *)self->buf.At2(0);
}

BOOL func_0202f030(MemPack *self, u32 index)
{
    return index < self->GetCount() - 1;
}

MemPack *func_0202f058(MemPack *self)
{
    SET_VT(&self->buf, VT_BUFFER);
    if (!self->buf.IsEmpty()) {
        delete[] self->buf.data;
        self->buf.size = 0;
    }
    operator delete(self);
    return self;
}

U32Array *func_0202f0a4(U32Array *self)
{
    SET_VT(self, VT_U32ARRAY);
    if (!self->IsEmpty()) {
        delete[] self->data;
        self->count = 0;
    }
    operator delete(self);
    return self;
}

u32 func_0202f0ec(OffsetPack *self)
{
    if (self->IsEmpty()) {
        return 0;
    }
    return self->offsets.GetCount() - 1;
}

BOOL func_0202f124(PackFile *self)
{
    return self->count == 0;
}

u32 func_0202f138(PackFile *self)
{
    return self->count;
}


void *gPackVtables[0x198 / 4] = {
    NULL, NULL, (void *)func_0202edb0, (void *)func_0202ed9c,
    (void *)func_0202f124, (void *)func_0202f138, (void *)func_0202ebd8, (void *)func_0202e980,
    (void *)func_0202ed08, (void *)func_0202ed88, NULL, NULL,
    (void *)func_0202ee00, (void *)func_0202edb4, (void *)func_0202eaec, (void *)func_0202f0ec,
    (void *)func_0202ea30, (void *)func_0202e980, (void *)func_0202ea84, (void *)func_0202ea9c,
    NULL, NULL, (void *)func_0202eb14, (void *)func_0202f0a4,
    (void *)func_0202eefc, (void *)func_0202eef4, (void *)func_0202eee4, (void *)func_0202eed4,
    (void *)func_0202eae4, (void *)func_0202eec8, (void *)func_0202ea78, (void *)func_0202eb00,
    NULL, NULL, (void *)func_0202ee90, (void *)func_0202ee7c,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, (void *)func_0202ee58, (void *)func_0202ee44,
    NULL, NULL, NULL, (void *)func_0202e980,
    NULL, NULL, NULL, NULL,
    (void *)func_0202ef6c, (void *)func_0202f058, (void *)func_0202efb0, (void *)func_0202eff4,
    (void *)func_0202e874, (void *)func_0202e8d8, (void *)func_0202e8ac, (void *)func_0202f030,
    NULL, NULL, (void *)func_0202e810, (void *)func_0202ef1c,
    (void *)func_0202e86c, (void *)func_0202eec0, (void *)func_0202eeb0, (void *)func_0202eea0,
    (void *)func_0202ef64, (void *)func_0202ee94, (void *)func_0202e8cc, (void *)func_0202e858,
    NULL, NULL, (void *)func_0202ee74, (void *)func_0202ee60,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, (void *)func_0202ef04, (void *)func_0202ef08,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    (void *)func_0202aec0, (void *)func_0202afe0,
};
}
