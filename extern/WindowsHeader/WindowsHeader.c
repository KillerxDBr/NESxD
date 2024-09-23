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
