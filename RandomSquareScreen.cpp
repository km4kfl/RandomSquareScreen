// RandomPixelScreen.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "RandomPixelScreen.h"
#include "AppExceptions.h"
#include "SmartHandleClass.h"

#include <WinUser.h>
#include <windowsx.h>

#include <memory>
#include <string>
#include <vector>
#include <random>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RANDOMPIXELSCREEN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RANDOMPIXELSCREEN));

    MSG msg;

    ShowCursor(TRUE);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RANDOMPIXELSCREEN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_RANDOMPIXELSCREEN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

static int _g_tick = 1;
static int _g_mode = 0;

// covered
#define MAX_SECONDS_MODE0 (60 * 5)
// uncovered
#define MAX_SECONDS_MODE1 (60 * 15)
#define MODE_FLIPFLOP 0
#define MODE_SCREENSAVER 0
#define SCREENSAVER_TIMEOUT 60
#define EXIT_ON_MOUSEMOVE 0

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   DWORD style = WS_VISIBLE | WS_THICKFRAME | WS_POPUP;
   //style = WS_VISIBLE | WS_POPUP;
   style = WS_OVERLAPPEDWINDOW;
   DWORD width = 300; //GetSystemMetrics(SM_CXMAXIMIZED);
   DWORD height = 300; //GetSystemMetrics(SM_CYMAXIMIZED);
   //width = 200;
   //height = 200;
   HWND hWnd = CreateWindowW(
      szWindowClass, szTitle, style,
      0, 0, width, height,
      nullptr, nullptr,
      hInstance, nullptr
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, 100, 1000, (TIMERPROC)NULL);
   //SetTimer(hWnd, 101, 1000, (TIMERPROC)NULL);
   return TRUE;
}

static void UpdateScreen(HWND hwnd, HDC hdc, PAINTSTRUCT &ps) {
    RECT win_rect;
    GetClientRect(hwnd, &win_rect);

    LONG width = win_rect.right - win_rect.left;
    LONG height = win_rect.bottom - win_rect.top;

    float aw = 55;
    float ah = 55;

    std::random_device rd;
    std::mt19937 g(rd());

    for (int x = 0; x < 400; ++x) {
        unsigned char rb = g() % 256;
        unsigned char gb = g() % 256;
        unsigned char bb = g() % 256;
        SmartBrush scb0(CreateSolidBrush(RGB(rb, gb, bb)));
        RECT _rect;
        memset(&_rect, 0, sizeof(_rect));
        _rect.top = g() % height;
        _rect.left = g() % width;
        double pr = (double)g() / (double)0xffffffff;
        double pb = (double)g() / (double)0xffffffff;
        _rect.right = _rect.left + pr * aw;
        _rect.bottom = _rect.top + pb * ah;
        FillRect(hdc, &_rect, scb0.GetHandle());
    }
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
    case WM_KEYUP:
    {
        SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE | WS_MAXIMIZE);
        break;
    }
    case WM_TIMER:
    {
        switch (wParam) {
            case 100:
            {
                InvalidateRect(hWnd, NULL, FALSE);
                break;
            }
            case 101:
            {
                break;
            }
        }
        break;
    }
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
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        UpdateScreen(hWnd, hdc, ps);
        EndPaint(hWnd, &ps);
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
