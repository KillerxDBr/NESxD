#include "resource.h"
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

NOTIFYICONDATAA nid = { 0 };

void KxD_Handle_Tray(void) {
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        printf("msg.message: %u\n", msg.message);
        if (msg.message == WM_APP) {
            // Handle system tray icon events
            printf("msg.lParam: %lld\n", msg.lParam);
            switch (msg.lParam) {
                case WM_LBUTTONDOWN: // Handle left mouse button click
                    printf("WM_LBUTTONDOWN\n");
                    break;
                case WM_RBUTTONDOWN: // Handle right mouse button click
                    printf("WM_RBUTTONDOWN\n");
                    break;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void KxD_Create_Tray(void *hWnd) {
    nid.cbSize = sizeof(NOTIFYICONDATAA);

    nid.uID = 1;

    nid.hIcon = LoadIconA(NULL, IDI_APPLICATION);

    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_APP;

    lstrcpynA(nid.szTip, "raylib system tray icon", sizeof(nid.szTip));

    Shell_NotifyIconA(NIM_ADD, &nid);
}

void KxD_Destroy_Tray(void) { //
    Shell_NotifyIconA(NIM_DELETE, &nid);
}
