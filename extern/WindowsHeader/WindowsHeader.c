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
WinVer GetWinVer(void) {
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
WVResp GetWindowsVersion(void) {
    const WinVer ver = GetWinVer();
    if (ver.build == 0) {
        if (ver.major < 5UL || (ver.major == 5UL && ver.minor < 1UL)) return OLDER_WIN;
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
        fflush(stderr);
        abort();
    }
    return ver.build >= 21996UL ? WIN_11 : WIN_10; // if build >= 21996 = Win 11 else Win 10
}
// clang-format on

#ifndef WinH_WIN32_ERR_MSG_SIZE
#define WinH_WIN32_ERR_MSG_SIZE (4 * 1024)
#endif // WinH_WIN32_ERR_MSG_SIZE

char *WinH_win32_error_message(uint32_t err) {
    static char win32ErrMsg[WinH_WIN32_ERR_MSG_SIZE] = {0};
    DWORD errMsgSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, LANG_USER_DEFAULT, win32ErrMsg,
                                      WinH_WIN32_ERR_MSG_SIZE, NULL);

    if (errMsgSize == 0) {
        char *newErrMsg = NULL;
        if (GetLastError() == ERROR_MR_MID_NOT_FOUND) {
            newErrMsg = "Invalid Win32 error code";
        } else {
            newErrMsg = "Could not get error message";
        }

        if (snprintf(win32ErrMsg, sizeof(win32ErrMsg), "%s for 0x%X", newErrMsg, err) > 0) {
            return (char *)&win32ErrMsg;
        } else {
            return newErrMsg;
        }
    }

    while (errMsgSize > 1 && isspace(win32ErrMsg[errMsgSize - 1])) {
        win32ErrMsg[--errMsgSize] = '\0';
    }

    return win32ErrMsg;
}