// Sound (ARM9 0x02035d54..0x02036404): sound heap, sound archive, 16 sequence handles and one stream
// (NitroSystem NNS_Snd*). Names are ours.
//
// The globals are file statics in the original (in the asm gap); mwcc addresses some of them relative to
// the first one (data_020f57fc), so field reads go through the `gSound` view and addresses use the
// separate symbols.
#include <nitro.h>
#include <nnsys/snd.h>

extern "C" {

typedef struct SoundState {
    const NNSSndArcStrmInfo *strmInfo;
    const NNSSndArcStrmPlayerInfo *strmPlayerInfo;
    u32 unk8;
    int bgm;                    // sequence/stream playing on channel 0
    NNSSndStrmHandle strm;
    int heapState;
    NNSSndHeapHandle heap;
} SoundState;

extern SoundState data_020f57fc;
extern NNSSndStrmHandle data_020f580c;     // gSound.strm
extern NNSSndHandle data_020f5818[16];
extern NNSSndArc data_020f5858;
extern u8 data_020f58f0[0x58000];          // sound heap memory

#define gSound data_020f57fc

void func_02035d54(void)
{
    NNSSndHeapHandle heap;

    NNS_SndInit();
    heap = NNS_SndHeapCreate(data_020f58f0, sizeof(data_020f58f0));
    gSound.heap = heap;
    gSound.heapState = 0;
    gSound.bgm = 0;
}

void func_02035d88(void)
{
    NNS_SndMain();
}

BOOL func_02035d94(const char *path)
{
    NNS_SndArcInit(&data_020f5858, path, gSound.heap, FALSE);
    if (!NNS_SndArcPlayerSetup(gSound.heap)) {
        return FALSE;
    }
    NNS_SndArcGetPlayerInfo(0);
    NNS_SndArcGetPlayerInfo(1);
    return TRUE;
}

void func_02035de4(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        NNS_SndHandleInit(&data_020f5818[i]);
    }
}

BOOL func_02035e10(int ch)
{
    return NNS_SndHandleIsValid(&data_020f5818[ch]) ? TRUE : FALSE;
}

void func_02035e38(int volume)
{
    NNS_SndSetMasterVolume(volume);
}

BOOL func_02035e44(int seqNo)
{
    return NNS_SndArcLoadSeq(seqNo, gSound.heap) != FALSE;
}

BOOL func_02035e68(int groupNo)
{
    return NNS_SndArcLoadGroup(groupNo, gSound.heap) != FALSE;
}

BOOL func_02035e8c(int seqNo, int ch)
{
    if (ch >= 16) {
        return FALSE;
    }
    if (ch == 0) {
        gSound.bgm = seqNo;
    }
    if (!NNS_SndArcPlayerStartSeq(&data_020f5818[ch], seqNo)) {
        return FALSE;
    }
    return TRUE;
}

void func_02035ed4(int fadeFrame, int ch)
{
    if (ch >= 16) {
        return;
    }
    if (ch == 0) {
        gSound.bgm = 0;
    }
    NNS_SndPlayerStopSeq(&data_020f5818[ch], fadeFrame);
}

void func_02035f10(void)
{
    gSound.heapState = NNS_SndHeapSaveState(gSound.heap);
}

void func_02035f30(void)
{
    if ((u32)(gSound.heapState + 1) <= 1) {
        return;
    }
    NNS_SndHeapLoadState(gSound.heap, gSound.heapState);
}

void func_02035f58(int level)
{
    if ((u32)(gSound.heapState + 1) <= 1) {
        return;
    }
    NNS_SndHeapLoadState(gSound.heap, level);
    gSound.heapState = level;
}

void func_02035f90(void)
{
    NNS_SndArcStrmInit(0, gSound.heap);
}

void func_02035fac(void)
{
    NNS_SndStrmHandleInit(&data_020f580c);
}

#ifdef NONMATCHING
BOOL func_02035fc0(int strmNo, int playerNo)
{
    NNS_SndArcStrmMoveVolume(&data_020f580c, 127, 0);
    gSound.strmInfo = NNS_SndArcGetStrmInfo(playerNo);
    gSound.strmPlayerInfo = NNS_SndArcGetStrmPlayerInfo(playerNo);
    if (playerNo == 0) {
        gSound.bgm = strmNo;
    }
    if (!NNS_SndArcStrmStartEx(&data_020f580c, playerNo, 0, strmNo, 0)) {
        return FALSE;
    }
    return NNS_SndStrmHandleIsValid(&gSound.strm) ? TRUE : FALSE;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL func_02035fc0(int strmNo, int playerNo) {
    stmdb sp!, {r3, r4, r5, lr}
    mov r5, r0
    mov r4, r1
    ldr r0, [pc, #0x6c]
    mov r1, #0x7f
    mov r2, #0x0
    bl NNS_SndArcStrmMoveVolume
    mov r0, r4
    bl NNS_SndArcGetStrmInfo
    ldr r1, [pc, #0x58]
    str r0, [r1, #0x0]
    mov r0, r4
    bl NNS_SndArcGetStrmPlayerInfo
    ldr r1, [pc, #0x48]
    cmp r4, #0x0
    str r0, [r1, #0x4]
    streq r5, [r1, #0xc]
    mov r2, #0x0
    ldr r0, [pc, #0x30]
    mov r1, r4
    mov r3, r5
    str r2, [sp, #0x0]
    bl NNS_SndArcStrmStartEx
    cmp r0, #0x0
    ldmeqia sp!, {r3, r4, r5, pc}
    ldr r0, [pc, #0x18]
    ldr r0, [r0, #0x10]
    cmp r0, #0x0
    movne r0, #0x1
    moveq r0, #0x0
    cmp r0, #0x0
    ldmia sp!, {r3, r4, r5, pc}
    dcd data_020f580c
    dcd data_020f57fc
}
#endif

void func_02036048(int fadeFrame)
{
    gSound.bgm = 0;
    NNS_SndArcStrmStop(&data_020f580c, fadeFrame);
}

void func_02036070(int fadeFrame)
{
    gSound.bgm = 0;
    NNS_SndArcStrmStopAll(fadeFrame);
}

int func_0203608c(void)
{
    return gSound.bgm;
}

// 1 for the sound ids in the second group (see the table), 0 for the others.
int func_0203609c(int id)
{
    switch (id) {
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0xa3:
    case 0xa4:
    case 0xa5:
    case 0xa6:
    case 0xa7:
    case 0xa8:
    case 0xa9:
    case 0xaa:
    case 0xab:
    case 0xfa:
    case 0xfb:
    case 0xfc:
    case 0xfd:
    case 0xfe:
    case 0xff:
    case 0x100:
    case 0x101:
    case 0x102:
    case 0x103:
    case 0x104:
    case 0x105:
    case 0x106:
    case 0x107:
    case 0x108:
    case 0x109:
    case 0x10a:
    case 0x10b:
    case 0x10c:
    case 0x10d:
    case 0x10e:
    case 0x10f:
    case 0x110:
        return 0;
    case 0x6f:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
    case 0x7f:
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0xa0:
    case 0xa1:
    case 0xa2:
    case 0xac:
    case 0xad:
    case 0xae:
    case 0xaf:
    case 0xb0:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
    case 0xbb:
    case 0xbc:
    case 0xbd:
    case 0xbe:
    case 0xbf:
    case 0xc0:
    case 0xc1:
    case 0xc2:
    case 0xc3:
    case 0xc4:
    case 0xc5:
    case 0xc6:
    case 0xc7:
    case 0xc8:
    case 0xc9:
    case 0xca:
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xdf:
    case 0xe0:
    case 0xe1:
    case 0xe2:
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
    case 0xe8:
    case 0xe9:
    case 0xea:
    case 0xeb:
    case 0xec:
    case 0xed:
    case 0xee:
    case 0xef:
    case 0xf0:
    case 0xf1:
    case 0x12c:
    case 0x12d:
    case 0x12e:
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
        return 1;
    }
    return 0;
}
}
