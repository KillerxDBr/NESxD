#pragma once
#ifndef KXD_WIN_H_ISOLATION
#define KXD_WIN_H_ISOLATION

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
    WIN_H_REG_NONE = 0x00000001,
    WIN_H_REG_SZ = 0x00000002,
    WIN_H_REG_EXPAND_SZ = 0x00000004,
    WIN_H_REG_BINARY = 0x00000008,
    WIN_H_REG_DWORD = 0x00000010,
    WIN_H_REG_MULTI_SZ = 0x00000020,
    WIN_H_REG_QWORD = 0x00000040,
    WIN_H_DWORD = (WIN_H_REG_BINARY | WIN_H_REG_DWORD),
    WIN_H_QWORD = (WIN_H_REG_BINARY | WIN_H_REG_QWORD),
    WIN_H_ANY = 0x0000ffff,
} WIN_H_REG_TYPE;

typedef struct WinVer {
    unsigned long major;
    unsigned long minor;
    unsigned long build;
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

WinVer GetWindowsVersion(void);
WVResp GetWinVer(void);

bool WinH_SetConsoleOutputCP(uint32_t wCodePageID);
bool WinH_CopyFileA(const char *sourceFile, const char *destFile, bool failIfExists);
bool WinH_CopyFileW(const wchar_t *sourceFile, const wchar_t *destFile, bool failIfExists);
long WinH_RegGetValueA(int hKey, const char *subKey, const char *value, uint32_t flags, unsigned long *valueType, char *regValue,
                       unsigned long *regValueSize);
long WinH_RegGetValueW(int hKey, const wchar_t *subKey, const wchar_t *value, uint32_t flags, unsigned long *valueType, wchar_t *regValue,
                       unsigned long *regValueSize);

#endif /* KXD_WIN_H_ISOLATION */
