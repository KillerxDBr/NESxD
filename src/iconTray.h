#ifndef KXD_ICONTRAY_H
#define KXD_ICONTRAY_H

#include "resource.h"

#ifdef _WIN32
void KxD_Create_Tray(void *hWnd);
void KxD_Handle_Tray(void);
void KxD_Destroy_Tray(void);
#endif

#define KXD_ICONTRAY_MSG (UINT)(WM_APP + 1)
#define KXD_ICONTRAY_MSG2 (UINT)(WM_USER + 1)

#endif