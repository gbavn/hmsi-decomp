#ifndef GAME_FILE_H
#define GAME_FILE_H

#include <nitro/fs.h>

typedef struct GFile {
    u32 unk0;
    FSFile file;
} GFile;

typedef s32 (*GFileReadFunc)(FSFile *file, void *dst, s32 len);

void GFile_InitFileSystem(u32 dmaNo);
FSFileID GFile_GetFileID(const char *path);
void *GFile_UncompressLZ(void *src);
BOOL GFile_IsClosed(GFile *file);
void GFile_Init(GFile *file);
BOOL GFile_Open(GFile *file, const char *path);
BOOL GFile_OpenFast(GFile *file, const FSFileID *id);
BOOL GFile_Close(GFile *file);
u32 GFile_GetLength(GFile *file);
BOOL GFile_ReadWith(GFile *file, u32 len, void *dst, GFileReadFunc func);
BOOL GFile_Read(GFile *file, u32 len, void *dst);
BOOL GFile_ReadAsync(GFile *file, u32 len, void *dst);
BOOL GFile_IsBusy(GFile *file);
BOOL GFile_Seek(GFile *file, s32 offset, FSSeekFileMode origin);
BOOL GFile_Rewind(GFile *file);
void *GFile_Load(GFile *file, const char *path, int unused);
void *GFile_LoadEx(GFile *file, const char *path, u32 *outSize, int unused);
void *GFile_LoadAsync(GFile *file, const char *path);
void *GFile_LoadAsyncEx(GFile *file, const char *path, u32 *outSize);

#endif
