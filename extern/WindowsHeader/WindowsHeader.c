#include "WindowsHeader.h"

#define W32(T) __declspec(dllimport) T __stdcall

W32(int) SetConsoleOutputCP(uint32_t);
bool WinH_SetConsoleOutputCP(uint32_t wCodePageID) {
    return SetConsoleOutputCP(wCodePageID); //
}

W32(int) CopyFileA(const char *, const char *, int);
bool WinH_CopyFileA(const char *sourceFile, const char *destFile, bool failIfExists) {
    return CopyFileA(sourceFile, destFile, failIfExists);
}

W32(int) CopyFileW(const wchar_t *, const wchar_t *, int);
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

typedef void *HKEY;

#define HKEY_CLASSES_ROOT ((HKEY)(uintptr_t)((int32_t)0x80000000))
#define HKEY_CURRENT_USER ((HKEY)(uintptr_t)((int32_t)0x80000001))
#define HKEY_LOCAL_MACHINE ((HKEY)(uintptr_t)((int32_t)0x80000002))
#define HKEY_USERS ((HKEY)(uintptr_t)((int32_t)0x80000003))
#define HKEY_PERFORMANCE_DATA ((HKEY)(uintptr_t)((int32_t)0x80000004))
#define HKEY_PERFORMANCE_TEXT ((HKEY)(uintptr_t)((int32_t)0x80000050))
#define HKEY_PERFORMANCE_NLSTEXT ((HKEY)(uintptr_t)((int32_t)0x80000060))
#define HKEY_CURRENT_CONFIG ((HKEY)(uintptr_t)((int32_t)0x80000005))
#define HKEY_DYN_DATA ((HKEY)(uintptr_t)((int32_t)0x80000006))
#define HKEY_CURRENT_USER_LOCAL_SETTINGS ((HKEY)(uintptr_t)((int32_t)0x80000007))

static HKEY HKArr[] = {
    HKEY_CLASSES_ROOT,                //
    HKEY_CURRENT_USER,                //
    HKEY_LOCAL_MACHINE,               //
    HKEY_USERS,                       //
    HKEY_PERFORMANCE_DATA,            //
    HKEY_PERFORMANCE_TEXT,            //
    HKEY_PERFORMANCE_NLSTEXT,         //
    HKEY_CURRENT_CONFIG,              //
    HKEY_DYN_DATA,                    //
    HKEY_CURRENT_USER_LOCAL_SETTINGS, //
};

W32(int32_t) RegGetValueA(HKEY, const char *, const char *, uint32_t, uint32_t *, char *, uint32_t *);
int32_t WinH_RegGetValueA(int hKey, const char *subKey, const char *value, uint32_t flags, uint32_t *valueType, char *regValue,
                          uint32_t *regValueSize) {
    if (hKey < 0 || hKey > (int)(sizeof(HKArr) / sizeof(HKArr[0])))
        return -1;

    return RegGetValueA(HKArr[hKey], subKey, value, flags, valueType, regValue, regValueSize);
}

W32(int32_t) RegGetValueW(HKEY, const wchar_t *, const wchar_t *, uint32_t, uint32_t *, wchar_t *, uint32_t *);
int32_t WinH_RegGetValueW(int hKey, const wchar_t *subKey, const wchar_t *value, uint32_t flags, uint32_t *valueType, wchar_t *regValue,
                          uint32_t *regValueSize) {
    if (hKey < 0 || hKey > (int)(sizeof(HKArr) / sizeof(HKArr[0])))
        return -1;

    return RegGetValueW(HKArr[hKey], subKey, value, flags, valueType, regValue, regValueSize);
}

#define SHARED_USER_DATA (uint8_t *)0x7FFE0000
WinVer GetWinVer(void) {
    WinVer result = {
        .major = *(uint32_t *)(SHARED_USER_DATA + 0x26c), // major version offset
        .minor = *(uint32_t *)(SHARED_USER_DATA + 0x270), // minor version offset
    };

    if (result.major >= 10UL)
        result.build = *(uint32_t *)(SHARED_USER_DATA + 0x260); // build number offset

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
__declspec(dllimport) int __cdecl isspace(int _C);

#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#define ERROR_MR_MID_NOT_FOUND 317l

#ifndef WinH_WIN32_ERR_MSG_SIZE
#define WinH_WIN32_ERR_MSG_SIZE (4 * 1024)
#endif // WinH_WIN32_ERR_MSG_SIZE

W32(uint32_t) FormatMessageA(uint32_t, void *, uint32_t, uint32_t, char *, uint32_t, void *);
W32(uint32_t) GetLastError(void);

char *WinH_win32_error_message(uint32_t err) {
    static char win32ErrMsg[WinH_WIN32_ERR_MSG_SIZE] = { 0 };
    uint32_t errMsgSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, win32ErrMsg,
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
