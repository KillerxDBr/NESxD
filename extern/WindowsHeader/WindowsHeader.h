#ifndef KXD_WIN_H_ISOLATION
#define KXD_WIN_H_ISOLATION

#include <stdbool.h>
#include <stdint.h>

bool WinH_SetConsoleOutputCP(uint32_t wCodePageID);
bool WinH_CopyFileA(const char *sourceFile, const char *destFile, bool failIfExists);
bool WinH_CopyFileW(const wchar_t *sourceFile, const wchar_t *destFile, bool failIfExists);

#endif /* KXD_WIN_H_ISOLATION */
