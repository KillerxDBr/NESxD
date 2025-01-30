#include "WindowsHeader.h"
#include <windows.h>

bool WinH_SetConsoleOutputCP(uint32_t wCodePageID) {
    //
    return SetConsoleOutputCP(wCodePageID);
}

bool WinH_CopyFileA(const char *sourceFile, const char *destFile, bool failIfExists) {
    return CopyFileA(sourceFile, destFile, failIfExists);
}

bool WinH_CopyFileW(const wchar_t *sourceFile, const wchar_t *destFile, bool failIfExists) {
    return CopyFileW(sourceFile, destFile, failIfExists);
}
/*
LSTATUS RegGetValueA(
 [in]                HKEY    hkey,
 [in, optional]      LPCSTR  lpSubKey,
 [in, optional]      LPCSTR  lpValue,
 [in, optional]      DWORD   dwFlags,
 [out, optional]     LPDWORD pdwType,
 [out, optional]     PVOID   pvData,
 [in, out, optional] LPDWORD pcbData
);
*/
/*
RegGetValueW(
    HKEY_CURRENT_USER,
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
    L"AppsUseLightTheme",
    RRF_RT_REG_DWORD, // expected value type
    nullptr,
    buffer.data(),
    &cbData);
*/

long WinH_RegGetValueA(int hKey, const char *subKey, const char *value, uint32_t flags, unsigned long *valueType, char *regValue,
                       unsigned long *regValueSize) {
    HKEY key;

    switch (hKey) {
    case WIN_H_HKEY_CLASSES_ROOT:
        key = HKEY_CLASSES_ROOT;
        break;
    case WIN_H_HKEY_CURRENT_USER:
        key = HKEY_CURRENT_USER;
        break;
    case WIN_H_HKEY_LOCAL_MACHINE:
        key = HKEY_LOCAL_MACHINE;
        break;
    case WIN_H_HKEY_USERS:
        key = HKEY_USERS;
        break;
    case WIN_H_HKEY_PERFORMANCE_DATA:
        key = HKEY_PERFORMANCE_DATA;
        break;
    case WIN_H_HKEY_PERFORMANCE_TEXT:
        key = HKEY_PERFORMANCE_TEXT;
        break;
    case WIN_H_HKEY_PERFORMANCE_NLSTEXT:
        key = HKEY_PERFORMANCE_NLSTEXT;
        break;
    case WIN_H_HKEY_CURRENT_CONFIG:
        key = HKEY_CURRENT_CONFIG;
        break;
    case WIN_H_HKEY_DYN_DATA:
        key = HKEY_DYN_DATA;
        break;
    case WIN_H_HKEY_CURRENT_USER_LOCAL_SETTINGS:
        key = HKEY_CURRENT_USER_LOCAL_SETTINGS;
        break;
    default:
        key = NULL;
        break;
    }

    if (key == NULL)
        return -1;

    return RegGetValueA(key, subKey, value, flags, valueType, regValue, regValueSize);
}

long WinH_RegGetValueW(int hKey, const wchar_t *subKey, const wchar_t *value, uint32_t flags, unsigned long *valueType, wchar_t *regValue,
                       unsigned long *regValueSize) {
    HKEY key;

    switch (hKey) {
    case WIN_H_HKEY_CLASSES_ROOT:
        key = HKEY_CLASSES_ROOT;
        break;
    case WIN_H_HKEY_CURRENT_USER:
        key = HKEY_CURRENT_USER;
        break;
    case WIN_H_HKEY_LOCAL_MACHINE:
        key = HKEY_LOCAL_MACHINE;
        break;
    case WIN_H_HKEY_USERS:
        key = HKEY_USERS;
        break;
    case WIN_H_HKEY_PERFORMANCE_DATA:
        key = HKEY_PERFORMANCE_DATA;
        break;
    case WIN_H_HKEY_PERFORMANCE_TEXT:
        key = HKEY_PERFORMANCE_TEXT;
        break;
    case WIN_H_HKEY_PERFORMANCE_NLSTEXT:
        key = HKEY_PERFORMANCE_NLSTEXT;
        break;
    case WIN_H_HKEY_CURRENT_CONFIG:
        key = HKEY_CURRENT_CONFIG;
        break;
    case WIN_H_HKEY_DYN_DATA:
        key = HKEY_DYN_DATA;
        break;
    case WIN_H_HKEY_CURRENT_USER_LOCAL_SETTINGS:
        key = HKEY_CURRENT_USER_LOCAL_SETTINGS;
        break;
    default:
        key = NULL;
        break;
    }

    if (key == NULL)
        return -1;

    return RegGetValueW(key, subKey, value, flags, valueType, regValue, regValueSize);
}

#define SHARED_USER_DATA (BYTE *)0x7FFE0000
WinVer GetWindowsVersion(void) {
    WinVer result = {
        .major = *(ULONG *)(SHARED_USER_DATA + 0x26c), // major version offset
        .minor = *(ULONG *)(SHARED_USER_DATA + 0x270), // minor version offset
    };

    if (result.major >= 10UL)
        result.build = *(ULONG *)(SHARED_USER_DATA + 0x260); // build number offset

    return result;
}

// clang-format off

/*
    OLDER_WIN - Older Windows
    WIN_XP    - Windows XP
    WIN_VISTA - Windows Vista
    WIN_7     - Windows 7
    WIN_8     - Windows 8
    WIN_81    - Windows 8.1
    WIN_10    - Windows 10
    WIN_11    - Windows 11
*/
WVResp GetWinVer(void) {
    const WinVer ver = GetWindowsVersion();
    if (ver.build == 0) {
        if (ver.major < 5UL) return OLDER_WIN;
        if (ver.major < 6UL) return WIN_XP;
        if (ver.major == 6UL) {
            switch (ver.minor) {
                case 0UL: return WIN_VISTA;
                case 1UL: return WIN_7;
                case 2UL: return WIN_8;
                case 3UL: return WIN_81;
            }
        }
        fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, "GetWinVer");
        abort();
    }
    return ver.build >= 21996UL ? WIN_11 : WIN_10; // if build >= 21996 = Win 11 else Win 10
}
// clang-format on
