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
