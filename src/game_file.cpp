// game_file.cpp: thin wrapper around the SDK file system (FSFile) used by the whole game.
// File and function names are ours; the original names are unknown.
#include <nitro/fs.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include "game_file.h"

void *sFSTable;

void GFile_InitFileSystem(u32 dmaNo)
{
    u32 size;

    FS_Init(dmaNo);
    size = FS_GetTableSize();
    sFSTable = new u8[size];
    (void)FS_LoadTable(sFSTable, size);
}

FSFileID GFile_GetFileID(const char *path)
{
    FSFileID id;

    (void)FS_ConvertPathToFileID(&id, path);
    return id;
}

void *GFile_UncompressLZ(void *src)
{
    void *dst = new u8[*(u32 *)src >> 8];

    MI_UncompressLZ8(src, dst);
    delete[] (u8 *)src;
    return dst;
}

BOOL GFile_IsClosed(GFile *file)
{
    return !FS_IsFile(&file->file);
}

void GFile_Init(GFile *file)
{
    FS_InitFile(&file->file);
}

BOOL GFile_Open(GFile *file, const char *path)
{
    if (!GFile_IsClosed(file)) {
        GFile_Close(file);
    }
    return FS_OpenFile(&file->file, path) ? TRUE : FALSE;
}

BOOL GFile_OpenFast(GFile *file, const FSFileID *id)
{
    if (!GFile_IsClosed(file)) {
        GFile_Close(file);
    }
    return FS_OpenFileFast(&file->file, *id) ? TRUE : FALSE;
}

BOOL GFile_Close(GFile *file)
{
    BOOL result;

    if (!GFile_IsClosed(file)) {
        result = FS_CloseFile(&file->file) ? TRUE : FALSE;
        GFile_Init(file);
        return result;
    }
    return FALSE;
}

u32 GFile_GetLength(GFile *file)
{
    return FS_GetLength(&file->file);
}

BOOL GFile_ReadWith(GFile *file, u32 len, void *dst, GFileReadFunc func)
{
    s32 read;

    if (!GFile_IsClosed(file)) {
        read = func(&file->file, dst, (s32)len);
        if (read >= 0 && read <= len) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL GFile_Read(GFile *file, u32 len, void *dst)
{
    return GFile_ReadWith(file, len, dst, FS_ReadFile);
}

BOOL GFile_ReadAsync(GFile *file, u32 len, void *dst)
{
    return GFile_ReadWith(file, len, dst, FS_ReadFileAsync);
}

BOOL GFile_IsBusy(GFile *file)
{
    return FS_IsBusy(&file->file) ? TRUE : FALSE;
}

BOOL GFile_Seek(GFile *file, s32 offset, FSSeekFileMode origin)
{
    return FS_SeekFile(&file->file, offset, origin) ? TRUE : FALSE;
}

BOOL GFile_Rewind(GFile *file)
{
    return FS_SeekFile(&file->file, 0, FS_SEEK_SET) ? TRUE : FALSE;
}

void *GFile_Load(GFile *file, const char *path, int unused)
{
    u32 size;

    return GFile_LoadEx(file, path, &size, unused);
}

#ifdef NONMATCHING
// Register allocation differs (raw/rawSize swapped).
void *GFile_LoadEx(GFile *file, const char *path, u32 *outSize, int unused)
{
    u32 len;
    u32 header;
    u32 rawSize;
    void *raw;
    void *buf;

    (void)GFile_Open(file, path);
    len = GFile_GetLength(file);
    (void)GFile_Read(file, sizeof(header), &header);
    (void)GFile_Seek(file, 0, FS_SEEK_SET);
    if ((header & 0xFF) == 0x11) {
        rawSize = header >> 8;
        raw = new u8[rawSize];
    }
    buf = new u8[len];
    (void)GFile_Read(file, len, buf);
    DC_FlushRange(buf, len);
    if ((header & 0xFF) != 0x11) {
        *outSize = len;
        return buf;
    }
    MI_UncompressLZ8(buf, raw);
    DC_FlushRange(raw, rawSize);
    delete[] (u8 *)buf;
    *outSize = rawSize;
    return raw;
}
#else
// operator new[] / delete[] under their linker names, for the asm below
extern "C" void *_Znam(unsigned long);
extern "C" void _ZdaPv(void *);
// NONMATCHING: kept as assembly until the C version matches.
asm void *GFile_LoadEx(GFile *file, const char *path, u32 *outSize, int unused) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    mov r9, r0
    mov r8, r2
    bl GFile_Open
    mov r0, r9
    bl GFile_GetLength
    mov r5, r0
    add r2, sp, #0x0
    mov r0, r9
    mov r1, #0x4
    bl GFile_Read
    mov r1, #0x0
    mov r0, r9
    mov r2, r1
    bl GFile_Seek
    ldr r0, [sp, #0x0]
    and r1, r0, #0xff
    cmp r1, #0x11
    bne _02028cfc
    mov r7, r0, lsr #0x8
    mov r0, r7
    bl _Znam
    mov r6, r0
_02028cfc:
    mov r0, r5
    bl _Znam
    mov r4, r0
    mov r0, r9
    mov r1, r5
    mov r2, r4
    bl GFile_Read
    mov r0, r4
    mov r1, r5
    bl DC_FlushRange
    ldr r0, [sp, #0x0]
    and r0, r0, #0xff
    cmp r0, #0x11
    mov r0, r4
    strne r5, [r8, #0x0]
    ldmneia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    mov r1, r6
    bl MI_UncompressLZ8
    mov r0, r6
    mov r1, r7
    bl DC_FlushRange
    mov r0, r4
    bl _ZdaPv
    mov r0, r6
    str r7, [r8, #0x0]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
}
#endif

void *GFile_LoadAsync(GFile *file, const char *path)
{
    u32 size;

    return GFile_LoadAsyncEx(file, path, &size);
}

void *GFile_LoadAsyncEx(GFile *file, const char *path, u32 *outSize)
{
    u32 len;
    void *buf;

    (void)GFile_Open(file, path);
    len = GFile_GetLength(file);
    buf = new u8[len];
    (void)GFile_ReadAsync(file, len, buf);
    *outSize = len;
    return buf;
}
