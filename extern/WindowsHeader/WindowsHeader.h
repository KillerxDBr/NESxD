#pragma once
#ifndef KXD_WIN_H_ISOLATION
#define KXD_WIN_H_ISOLATION

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    WIN_H_HKEY_CLASSES_ROOT,
    WIN_H_HKEY_CURRENT_USER,
    WIN_H_HKEY_LOCAL_MACHINE,
    WIN_H_HKEY_USERS,
    WIN_H_HKEY_PERFORMANCE_DATA,
    WIN_H_HKEY_PERFORMANCE_TEXT,
    WIN_H_HKEY_PERFORMANCE_NLSTEXT,
    WIN_H_HKEY_CURRENT_CONFIG,
    WIN_H_HKEY_DYN_DATA,
    WIN_H_HKEY_CURRENT_USER_LOCAL_SETTINGS,
} WIN_H_ENUM_HKEY;

typedef enum {
    WIN_H_REG_NONE      = 0x00000001,
    WIN_H_REG_SZ        = 0x00000002,
    WIN_H_REG_EXPAND_SZ = 0x00000004,
    WIN_H_REG_BINARY    = 0x00000008,
    WIN_H_REG_DWORD     = 0x00000010,
    WIN_H_REG_MULTI_SZ  = 0x00000020,
    WIN_H_REG_QWORD     = 0x00000040,
    WIN_H_DWORD         = (WIN_H_REG_BINARY | WIN_H_REG_DWORD),
    WIN_H_QWORD         = (WIN_H_REG_BINARY | WIN_H_REG_QWORD),
    WIN_H_ANY           = 0x0000ffff,
} WIN_H_REG_TYPE;

typedef struct WinVer {
    uint32_t major;
    uint32_t minor;
    uint32_t build;
} WinVer;

typedef enum {
    OLDER_WIN = -1,
    WIN_XP,
    WIN_VISTA,
    WIN_7,
    WIN_8,
    WIN_81,
    WIN_10,
    WIN_11,
} WVResp;

WVResp GetWindowsVersion(void);
WinVer GetWinVer(void);

bool WinH_SetConsoleOutputCP(uint32_t wCodePageID);
bool WinH_CopyFile(const char *sourceFile, const char *destFile);

int32_t WinH_RegGetValueA(int hKey, const char *subKey, const char *value, uint32_t flags,
                          uint32_t *valueType, char *regValue, uint32_t *regValueSize);

int32_t WinH_RegGetValueW(int hKey, const wchar_t *subKey, const wchar_t *value, uint32_t flags,
                          uint32_t *valueType, wchar_t *regValue, uint32_t *regValueSize);

const char *WinH_win32_error_message(uint32_t err);
bool WinH_win32_uft8_cmdline_args(int *argc, char ***argv_ptr);
#endif /* KXD_WIN_H_ISOLATION */
