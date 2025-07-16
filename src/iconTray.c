#if !defined(PLATFORM_WEB) && defined(_WIN32)

#include "iconTray.h"

NOTIFYICONDATAW nid;

void KxD_Handle_Tray(void) {
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        LOG_INF("msg.message: 0x%04X", msg.message);
        if (msg.message == KXD_ICONTRAY_MSG || msg.message == WM_USER) {
            // Handle system tray icon events
            if (msg.message == KXD_ICONTRAY_MSG)
                LOG_INF("KXD_ICONTRAY_MSG--------------------------");

            if (msg.message == WM_USER)
                LOG_INF("WM_USER-----------------------------------");

            LOG_INF("msg.lParam: %lld (0x%04X)", msg.lParam, LOWORD(msg.lParam));

            switch (LOWORD(msg.lParam)) {
            case WM_LBUTTONDOWN: // Handle left mouse button click
                LOG_INF("WM_LBUTTONDOWN----------------------------");
                break;
            case WM_RBUTTONDOWN: // Handle right mouse button click
                LOG_INF("WM_RBUTTONDOWN----------------------------");
                break;
            case NIN_BALLOONSHOW:
                LOG_INF("NIN_BALLOONSHOW---------------------------");
                break;
            case NIN_POPUPOPEN:
                LOG_INF("NIN_POPUPOPEN-----------------------------");
                break;
            case WM_CONTEXTMENU:
                LOG_INF("WM_CONTEXTMENU----------------------------");
                break;
            default:
                break;
            }

        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

void KxD_Create_Tray(void *hWnd) {
    SecureZeroMemory(&nid, sizeof(nid));

    nid.cbSize = sizeof(nid);

    // 974e95f3-10f2-40da-aa6d-e90099f0d1f8
    const GUID myGuid = (GUID){
        0x974e95f3, 0x10f2, 0x40da, {0xaa, 0x6d, 0xe9, 0x00, 0x99, 0xf0, 0xd1, 0xf8}
    };
    nid.guidItem = myGuid;

    nid.hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(NESXDICO));
    assert(nid.hIcon != NULL);

    nid.hWnd             = hWnd;
    nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
    nid.uVersion         = NOTIFYICON_VERSION_4;
    nid.uCallbackMessage = KXD_ICONTRAY_MSG;

    memcpy(nid.szTip, KXD_ICON_TEXT, sizeof(KXD_ICON_TEXT));

    if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
        LOG_ERR("Could not add Icon to tray: %s", WinH_win32_error_message(GetLastError()));
    }

    if (!Shell_NotifyIconW(NIM_SETVERSION, &nid)) {
        LOG_ERR("Could not set version to Icon: %s", WinH_win32_error_message(GetLastError()));
    }
}

void KxD_Destroy_Tray(void) {
    if (!Shell_NotifyIconW(NIM_DELETE, &nid)) {
        LOG_ERR("Could not delete icon from tray: %s", WinH_win32_error_message(GetLastError()));
    }

    if (!DestroyIcon(nid.hIcon)) {
        LOG_ERR("Could not destroy icon resource: %s", WinH_win32_error_message(GetLastError()));
    }

    SecureZeroMemory(&nid, sizeof(nid));
}
#else
void KxD_Create_Tray(void *hWnd) { (void)(hWnd); }
void KxD_Handle_Tray(void) {}
void KxD_Destroy_Tray(void) {}
#endif // !defined(PLATFORM_WEB) && defined(_WIN32)
