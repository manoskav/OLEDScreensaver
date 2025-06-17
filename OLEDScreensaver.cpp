// OLEDScreensaver.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "OLEDScreensaver.h"
#include <windows.h>
#include <shellapi.h>   // For Shell_NotifyIcon, NOTIFYICONDATA
#include <tchar.h>      // For _tcscpy_s and TCHAR compatibility (if using Unicode)


#define MAX_LOADSTRING 100

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_PAUSE 40001
#define ID_TRAY_SET_THRESHOLD 40002
#define ID_TRAY_EXIT 40003

NOTIFYICONDATA nid = { 0 };
HMENU hTrayMenu = NULL;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


POINT lastPos = { 0, 0 };
ULONG64 lastMoveTime = 0;
ULONG64 lastInTime;
ULONG64 outTime = 0;
POINT prevPos;
BOOL mouseIdle = FALSE;
ULONG64 mouseIdleTime = 0;

DWORD INACTITY_THR = 60000;
BOOL VISIBLE = FALSE;
BOOL PAUSE = FALSE;
RECT monitorRect;

void turnScreen(HWND hWnd, BOOL visible);
void SetInactivityThreshold(HWND hwnd);
INT_PTR CALLBACK ThresholdDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    const wchar_t CLASS_NAME[] = L"OLEDScreensaver";

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Black background

    RegisterClass(&wc);

    // Get primary monitor resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create window
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,            // Always on top
        CLASS_NAME,
        L"Blackout",
        WS_POPUP,                 // No border, no title bar
        0, 0,                     // Top-left corner
        screenWidth, screenHeight,
        nullptr, nullptr,
        hInstance, nullptr);

    if (!hwnd) return 0;

    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);


    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    // Hide mouse cursor
    //ShowCursor(FALSE);

    // Tray Icon
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_PAUSE, TEXT("Pause"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_SET_THRESHOLD, TEXT("Set Inactivity Threshold"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;                      // Your main window handle
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = hIcon;  // Use your own icon if desired
    _tcscpy_s(nid.szTip, TEXT("Monitor Blackout"));

    Shell_NotifyIcon(NIM_ADD, &nid);


    // Initialize global variables
    GetCursorPos(&lastPos);
    prevPos.x = lastPos.x;
    prevPos.y = lastPos.y;
    lastMoveTime = GetTickCount64();
    lastInTime = GetTickCount64();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OLEDSCREENSAVER, szWindowClass, MAX_LOADSTRING);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OLEDSCREENSAVER));

    monitorRect.left = 0;
    monitorRect.top = 0;
    monitorRect.right = GetSystemMetrics(SM_CXSCREEN);
    monitorRect.bottom = GetSystemMetrics(SM_CYSCREEN);

    SetTimer(hwnd, 1, 200, NULL); // Every 1000ms (1 sec), send WM_TIMER with ID=1
    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Restore cursor before exit
    //ShowCursor(TRUE);
    return (int) msg.wParam;
}



void InitBlackoutSystem(HINSTANCE hInstance) {
    // Register class, create windows, set up timers, etc.
}


void turnScreen(HWND hWnd, BOOL visible) {

    if (visible) {
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
        VISIBLE = TRUE;
    }
    else {
        ShowWindow(hWnd, SW_HIDE);
        UpdateWindow(hWnd);
        VISIBLE = FALSE;
    }

}


void SetInactivityThreshold(HWND hwnd)
{
    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_THRESHOLD_DIALOG), hwnd, ThresholdDlgProc, (LPARAM)&INACTITY_THR);
}

INT_PTR CALLBACK ThresholdDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD* pThreshold;

    switch (message)
    {
    case WM_INITDIALOG:
        pThreshold = (DWORD*)lParam;
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            TCHAR inputBuffer[16];
            GetDlgItemText(hDlg, IDC_THRESHOLD_INPUT, inputBuffer, 16);
            int newThreshold = _ttoi(inputBuffer);
            if (newThreshold > 0)
            {
                *pThreshold = newThreshold*1000;
            }
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            case ID_TRAY_EXIT:
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
                break;

            case ID_TRAY_PAUSE:
                PAUSE = !PAUSE;
                ModifyMenu(hTrayMenu, ID_TRAY_PAUSE, MF_BYCOMMAND | MF_STRING, ID_TRAY_PAUSE, PAUSE ? TEXT("Resume") : TEXT("Pause"));
                break;

            case ID_TRAY_SET_THRESHOLD:
                SetInactivityThreshold(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_TIMER:
        if (wParam == 1) {

            POINT currentPos;
            GetCursorPos(&currentPos);
            ULONG64 now = GetTickCount64();

            bool isOutside =
                currentPos.x < monitorRect.left ||
                currentPos.x >= monitorRect.right ||
                currentPos.y < monitorRect.top ||
                currentPos.y >= monitorRect.bottom;

            mouseIdle = (currentPos.x == prevPos.x) && (currentPos.y == prevPos.y);
            if (!mouseIdle) lastMoveTime = now;
            mouseIdleTime = now - lastMoveTime;

            prevPos.x = currentPos.x;
            prevPos.y = currentPos.y;

            if (!isOutside) lastInTime = now;
            outTime = now - lastInTime;

            BOOL blackoutCond = (!VISIBLE) && (!PAUSE) && ((isOutside && outTime >= INACTITY_THR) ||
                                (mouseIdleTime >= INACTITY_THR));


            if (blackoutCond) turnScreen(hWnd, TRUE);
            if (PAUSE) turnScreen(hWnd, FALSE);

        }
        break;

    case WM_SETCURSOR:
        if (VISIBLE && LOWORD(lParam) == HTCLIENT) {
            SetCursor(NULL); // Hide cursor when over our window
            return TRUE;     // Prevent Windows from setting default cursor
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);  // ESC to quit
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        //PostQuitMessage(0);      // Quit on any mouse click
        turnScreen(hWnd, FALSE);
        break;

     
    // Tray Icon
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);  // Required before TrackPopupMenu
            TrackPopupMenu(hTrayMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
            PostMessage(hWnd, WM_NULL, 0, 0);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
