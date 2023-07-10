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

static bool _g_preview_mode = false;

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

    {
        std::string s = std::string((char*)lpCmdLine).substr(0, 2); /// first 2 letters from command line argument
        if (s == "\\c" || s == "\\C" || s == "/c" || s == "/C" || s == "") {
            //MessageBoxA(NULL, "There are no settings for this.", "Info", MB_OK);
            //return 0;
        }
        else if (s == "\\s" || s == "\\S" || s == "/s" || s == "/S") {            
            _g_preview_mode = false;
        }
        else if (s == "\\p" || s == "\\P" || s == "/p" || s == "/P") {
            _g_preview_mode = true;

            //HWND hwnd = (HWND)atoi(lpCmdLine + 3);
            //return 0;
        }
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RANDOMPIXELSCREEN));

    MSG msg;

    ShowCursor(FALSE);

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
   DWORD width = GetSystemMetrics(SM_CXMAXIMIZED);
   DWORD height = GetSystemMetrics(SM_CYMAXIMIZED);
   //width = 200;
   //height = 200;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, style,
      0, 0, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, 100, 12, (TIMERPROC)NULL);
   SetTimer(hWnd, 101, 1000, (TIMERPROC)NULL);
   return TRUE;
}

static void UpdateScreen(HWND hwnd, HDC hdc, PAINTSTRUCT &ps) {
    SmartPrivateHDC mem_hdc(SmartHDCInner(hwnd, CreateCompatibleDC(hdc)));

    // sometimes hdc can be NULL or did resources run out?? resource leak?
    // likely a resource leak
    if (mem_hdc.GetHandle().hdc == NULL) {
        return;
    }

    //BOOL_THROW(mem_hdc.GetHandle().hdc != NULL);

    RECT win_rect;
    GetClientRect(hwnd, &win_rect);

    LONG width = win_rect.right - win_rect.left;
    LONG height = win_rect.bottom - win_rect.top;

    SmartBitmap hbm_win(CreateCompatibleBitmap(
        hdc,
        width,
        height
    ));

    BOOL_THROW(hbm_win.GetHandle() != NULL);

    SmartGdiObj sgdi0(SmartGdiObjInner(
        mem_hdc.GetHandle().hdc,
        SelectObject(mem_hdc.GetHandle().hdc, hbm_win.GetHandle())
    ));

    int r2 = BitBlt(mem_hdc.GetHandle().hdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    /*
    BITMAP bmp;

    BOOL_THROW(GetObject(hbm_win.GetHandle(), sizeof(bmp), &bmp) != 0);

    BITMAPINFOHEADER bmp_hdr;

    SIZE_T bmp_bit_count = 32;

    SIZE_T bmp_size = ((bmp.bmWidth * bmp_bit_count + 31) / 32) * 4 * bmp.bmHeight;

    SmartGlobalAlloc h_data(GlobalAlloc(GHND, bmp_size));

    BOOL_THROW(h_data.GetHandle() != NULL);

    unsigned char* p_data = (unsigned char*)GlobalLock(h_data.GetHandle());

    BOOL_THROW(p_data != NULL);

    memset(&bmp_hdr, 0, sizeof(bmp_hdr));

    bmp_hdr.biSize = sizeof(BITMAPINFOHEADER);
    bmp_hdr.biWidth = bmp.bmWidth;
    bmp_hdr.biHeight = bmp.bmHeight;
    bmp_hdr.biPlanes = 1;
    bmp_hdr.biBitCount = 32;
    bmp_hdr.biCompression = BI_RGB;

    int r0 = GetDIBits(
        mem_hdc.GetHandle().hdc,
        hbm_win.GetHandle(),
        0,
        bmp.bmHeight,
        p_data,
        (BITMAPINFO*)&bmp_hdr,
        DIB_RGB_COLORS
    );
    */

    std::random_device rd;
    std::mt19937 g(rd());

    float aw = 55;
    float ah = 55;

    for (int x = 0; x < 10; ++x) {
        unsigned char rb = g() % 256;
        unsigned char gb = g() % 256;
        unsigned char bb = g() % 256;
        SmartBrush scb0(CreateSolidBrush(RGB(rb, gb, bb)));
        RECT _rect;
        memset(&_rect, 0, sizeof(_rect));
        _rect.top = g() % height;
        _rect.left = g() % width;
        _rect.right = _rect.left + (double)g() / (double)0xffffffff * aw;
        _rect.bottom = _rect.top + (double)g() / (double)0xffffffff * ah;
        FillRect(hdc, &_rect, scb0.GetHandle());
    }

    /*
    unsigned int _g_max = 0;
    _g_max--;
    double g_max = (double)_g_max;

    SIZE_T pixel_count = bmp_size / 4;

    for (SIZE_T x = 0; x < pixel_count; ++x) {
        unsigned char r = g() & 0xff;
        unsigned char g = g() & 0xff;
        unsigned char b = g() & 0xff;
        p_data[x * 4 + 0] = r;
        p_data[x * 4 + 1] = g;
        p_data[x * 4 + 2] = b;
        p_data[x * 4 + 3] = 0;
    }
    */

    /*
    double r_alpha = (double)g() / g_max;
    double g_alpha = (double)g() / g_max;
    double b_alpha = (double)g() / g_max;

    double rnd_data_ri = 0.0;
    double rnd_data_gi = 0.0;
    double rnd_data_bi = 0.0;
    
    for (SIZE_T x = 0; x < pixel_count; ++x) {
        double _rr = (double)g() / g_max;
        double _rg = (double)g() / g_max;
        double _rb = (double)g() / g_max;

        rnd_data_ri = rnd_data_ri * r_alpha + _rr * (1 - r_alpha);
        rnd_data_gi = rnd_data_gi * g_alpha + _rg * (1 - g_alpha);
        rnd_data_bi = rnd_data_bi * b_alpha + _rb * (1 - b_alpha);
        
        p_data[x * 4 + 0] = (unsigned char)roundf(rnd_data_ri * 255);
        p_data[x * 4 + 1] = (unsigned char)roundf(rnd_data_gi * 255);
        p_data[x * 4 + 2] = (unsigned char)roundf(rnd_data_bi * 255);
        p_data[x * 4 + 3] = 0;
    }
    */

    /*
    SmartBitmap hbm_tmp(CreateCompatibleBitmap(
        mem_hdc.GetHandle().hdc,
        width,
        height
    ));

    int r6 = SetDIBits(
        NULL, // only used if DIB_PAL_COLORS is used
        hbm_tmp.GetHandle(),
        0,
        bmp.bmHeight,
        p_data,
        (BITMAPINFO*)&bmp_hdr,
        DIB_RGB_COLORS
    );
    */

    /*
    h_data.Dealloc();

    SmartGdiObj sgdi1(SmartGdiObjInner(
        mem_hdc.GetHandle().hdc,
        SelectObject(mem_hdc.GetHandle().hdc, hbm_tmp.GetHandle())
    ));

    int r4 = BitBlt(hdc, 0, 0, width, height, mem_hdc.GetHandle().hdc, 0, 0, SRCCOPY);
    */

    // NOTE: These select the old objects when deconstructed as per the win32 API documentation.
    //sgdi0.Dealloc();
    //sgdi1.Dealloc();
    //h_data.Dealloc();
    mem_hdc.Dealloc();
    hbm_win.Dealloc();
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

class LastMousePos {
public:
    int x;
    int y;
    int i;

    LastMousePos() : x(0), y(0), i(0) {

    }
};

static LastMousePos _g_last_mouse_pos;

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
#if MODE_SCREENSAVER == 1
            LASTINPUTINFO lii;
            memset(&lii, 0, sizeof(lii));

            if (GetLastInputInfo(&lii) != 0) {
                DWORD delta;
                DWORD ctime = GetTickCount();

                if (lii.dwTime > ctime) {
                    DWORD tmp = 0;
                    tmp--;
                    delta = (tmp - lii.dwTime) + ctime;
                } else {
                    delta = ctime - lii.dwTime;
                }

                if (delta > (SCREENSAVER_TIMEOUT * 1000)) {
                    ShowWindow(hWnd, 1);
                }
                else {
                    ShowWindow(hWnd, 0);
                }
            }
#endif
#if MODE_FLIPFLOP == 1
            if (_g_tick == 0) {
                float rand_float = (float)std::rand() / (float)RAND_MAX;
                _g_mode = (_g_mode + 1) % 2;
                if (_g_mode) {
                    _g_tick = (int)(rand_float * MAX_SECONDS_MODE0);
                    ShowWindow(hWnd, 0);
                }
                else {
                    _g_tick = (int)(rand_float * MAX_SECONDS_MODE1);
                    ShowWindow(hWnd, 1);
                }
            }
            _g_tick--;
#endif
            break;
        }
        }
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
    case WM_MOUSEMOVE:
    {
#if EXIT_ON_MOUSEMOVE == 1
        int x_pos = GET_X_LPARAM(lParam);
        int y_pos = GET_Y_LPARAM(lParam);

        if (_g_last_mouse_pos.i > 0) {
            float dx = x_pos - _g_last_mouse_pos.x;
            float dy = y_pos - _g_last_mouse_pos.y;
            float d = sqrtf(dx * dx + dy * dy);

            if (d > 10.0f) {
                ShowCursor(TRUE);
                PostQuitMessage(0);
            }
        }

        _g_last_mouse_pos.i = 1;

        _g_last_mouse_pos.x = x_pos;
        _g_last_mouse_pos.y = y_pos;
#endif
        break;
    }
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
