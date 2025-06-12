#pragma once
#ifndef KXD_ICONTRAY_H
#define KXD_ICONTRAY_H

#include "resource.h"

#include <windows.h>

#include <stdint.h>
#include <stdio.h>

#include "WindowsHeader/WindowsHeader.h"
#include "kxdDebug.h"

void KxD_Create_Tray(void *hWnd);
void KxD_Handle_Tray(void);
void KxD_Destroy_Tray(void);

#ifdef _WIN32
#define KXD_ICONTRAY_MSG  (uint32_t)(WM_APP + 1)
#define KXD_ICONTRAY_MSG2 (uint32_t)(WM_USER + 1)

#define KXD_ICON_TEXT "KxD trayIcon"
#endif // _WIN32

#endif
