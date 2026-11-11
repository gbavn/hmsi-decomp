// Save memory (card backup) access (ARM9 0x0203ab28..0x0203b07c). Names are ours.
//
// The game identifies the backup chip as 2 Mbit flash, then reads/writes it through the NitroSDK CARD
// inline functions (one call per device type: EEPROM, flash, FRAM). The time of the last access is
// measured with the OS tick.
#include <nitro.h>

extern "C" {

typedef struct BackupVars {
    u16 lockId;
    int result;
    OSTick tick;
} BackupVars;

extern BackupVars data_0214f758;
extern const CARDBackupType data_020b2a30[7];   // EEPROM 4K/64K/512K, flash 2M/4M/8M, FRAM 256K

#define gBackup data_0214f758

void func_0203ab28(void)
{
    CARD_Init();
    s32 lockId = OS_GetLockID();
    if (lockId == OS_LOCK_ID_ERROR) {
        OS_Terminate();
    }
    gBackup.lockId = lockId;
    CARD_LockBackup(gBackup.lockId);
    CARD_IdentifyBackup(data_020b2a30[3]);
    CARD_UnlockBackup(gBackup.lockId);
    CARD_GetBackupPageSize();
    CARD_GetBackupTotalSize();
    if (CARD_IsBackupEeprom()) {
    } else if (CARD_IsBackupFlash()) {
    } else if (CARD_IsBackupFram()) {
    }
}

// Writes and verifies `len` bytes at `dst` in the backup; returns FALSE on error.
BOOL func_0203abc0(u32 dst, const void *src, u32 len)
{
    BOOL ok = TRUE;
    CARD_LockBackup(gBackup.lockId);
    gBackup.tick = OS_GetTick();
    if (CARD_IsBackupEeprom()) {
        CARD_WriteAndVerifyEepromAsync(dst, src, len, NULL, NULL);
    } else if (CARD_IsBackupFlash()) {
        CARD_WriteAndVerifyFlashAsync(dst, src, len, NULL, NULL);
    } else if (CARD_IsBackupFram()) {
        CARD_WriteAndVerifyFramAsync(dst, src, len, NULL, NULL);
    }
    CARD_WaitBackupAsync();
    gBackup.result = CARD_GetResultCode();
    if (gBackup.result != 0) {
        ok = FALSE;
    } else {
        gBackup.tick = OS_GetTick() - gBackup.tick;
    }
    CARD_UnlockBackup(gBackup.lockId);
    return ok;
}

// Reads `len` bytes at `src` in the backup; returns FALSE on error.
BOOL func_0203ad50(u32 src, void *dst, u32 len)
{
    BOOL ok = TRUE;
    CARD_LockBackup(gBackup.lockId);
    gBackup.tick = OS_GetTick();
    if (CARD_IsBackupEeprom()) {
        CARD_ReadEepromAsync(src, dst, len, NULL, NULL);
    } else if (CARD_IsBackupFlash()) {
        CARD_ReadFlashAsync(src, dst, len, NULL, NULL);
    } else if (CARD_IsBackupFram()) {
        CARD_ReadFramAsync(src, dst, len, NULL, NULL);
    }
    CARD_WaitBackupAsync();
    gBackup.result = CARD_GetResultCode();
    if (gBackup.result != 0) {
        ok = FALSE;
    } else {
        gBackup.tick = OS_GetTick() - gBackup.tick;
    }
    CARD_UnlockBackup(gBackup.lockId);
    return ok;
}

// Starts writing in the background (see func_0203b014).
void func_0203aec8(u32 dst, const void *src, u32 len)
{
    CARD_LockBackup(gBackup.lockId);
    gBackup.tick = OS_GetTick();
    if (CARD_IsBackupEeprom()) {
        CARD_WriteAndVerifyEepromAsync(dst, src, len, NULL, NULL);
    } else if (CARD_IsBackupFlash()) {
        CARD_WriteAndVerifyFlashAsync(dst, src, len, NULL, NULL);
    } else if (CARD_IsBackupFram()) {
        CARD_WriteAndVerifyFramAsync(dst, src, len, NULL, NULL);
    }
}

// Background write finished? 0 = not yet, 1 = done, -1 = error.
int func_0203b014(void)
{
    int ret = 0;
    if (CARD_TryWaitBackupAsync()) {
        gBackup.result = CARD_GetResultCode();
        if (gBackup.result != 0) {
            ret = -1;
        } else {
            gBackup.tick = OS_GetTick() - gBackup.tick;
            ret = 1;
        }
        CARD_UnlockBackup(gBackup.lockId);
    }
    return ret;
}

}
