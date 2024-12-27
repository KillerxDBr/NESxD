#if !defined(PLATFORM_WEB)

#include <stdio.h>
#include <windows.h>

#include "iconTray.h"

NOTIFYICONDATAA nid = { 0 };

void KxD_Handle_Tray(void) {
    MSG msg;
    UINT i = 0;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        i++;
        printf("msg.message: 0x%04X\n", msg.message);
        if (msg.message == KXD_ICONTRAY_MSG) {
            // Handle system tray icon events
            printf("KXD_ICONTRAY_MSG----------------------------\n");
            printf("msg.lParam: %lld (0x%04X)\n", msg.lParam, LOWORD(msg.lParam));
            switch (LOWORD(msg.lParam)) {
                // case WM_TRAYMESSAGE: break;
            case WM_LBUTTONDOWN: // Handle left mouse button click
                printf("WM_LBUTTONDOWN----------------------------\n");
                break;
            case WM_RBUTTONDOWN: // Handle right mouse button click
                printf("WM_RBUTTONDOWN----------------------------\n");
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (i == 100) {
            i = 0;
            fflush(stdout);
        }
    }
}

void KxD_Create_Tray(void *hWnd) {
    nid.cbSize = sizeof(NOTIFYICONDATAA);

    nid.uID = 1;

    nid.hIcon = LoadIconA(NULL, MAKEINTRESOURCEA(NESXDICO));

    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    // nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP | NIF_INFO;
    nid.uCallbackMessage = KXD_ICONTRAY_MSG;
    // nid.dwInfoFlags = NIIF_INFO;
    strcpy_s(nid.szTip, sizeof(KXD_ICON_TEXT), KXD_ICON_TEXT);

    Shell_NotifyIconA(NIM_ADD, &nid);
}

void KxD_Destroy_Tray(void) {
    DestroyIcon(nid.hIcon);

    Shell_NotifyIconA(NIM_DELETE, &nid);

    memset(&nid, 0, sizeof(nid));
}
#else
void KxD_Create_Tray(void *hWnd) { (void)(hWnd); }
void KxD_Handle_Tray(void) {}
void KxD_Destroy_Tray(void) {}
#endif // PLATFORM_WEB
