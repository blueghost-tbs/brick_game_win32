#include "tetris.h"

#include <windows.h>

#define CLIENTWIDTH    330 // 220 playfield + 110 scoreboard
#define CLIENTHEIGHT   420
#define WINDOWSTYLE    WS_OVERLAPPEDWINDOW

#define CLOCK_TICK     1000
#define KEYBOARD_TICK  100
#define TIMER_CLOCK    1
#define TIMER_KEYBOARD 2

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void draw_field(HDC hdc);
static void handle_keystrokes(HWND hwnd);

static int minimum_window_width = CLIENTWIDTH;
static int minimum_window_height = CLIENTHEIGHT;
static int block_size = 20;
static int block_border = 2;
static HBRUSH background_brush;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    background_brush = (HBRUSH)GetStockObject(GRAY_BRUSH);

    static TCHAR szAppName[] = TEXT("blockclass");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = background_brush;
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    RECT rect = {0, 0, CLIENTWIDTH, CLIENTHEIGHT};
    if (!AdjustWindowRect(&rect, WINDOWSTYLE, 0)) {
        MessageBox(NULL, TEXT("AdjustWindowRect() failed!"), szAppName, MB_ICONERROR);
        return 0;
    }
    minimum_window_width = rect.right - rect.left;
    minimum_window_height = rect.bottom - rect.top;

    hwnd = CreateWindow(szAppName,                         // window class name
                        TEXT("Brick game"),                // window caption
                        WINDOWSTYLE,                       // window style
                        CW_USEDEFAULT,                     // initial x position
                        CW_USEDEFAULT,                     // initial y position
                        minimum_window_width,              // initial x size
                        minimum_window_height,             // initial y size
                        NULL,                              // parent window handle
                        NULL,                              // window menu handle
                        hInstance,                         // program instance handle
                        NULL);                             // creation parameters

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
        case WM_CREATE:
            tetris_init();
            SetTimer(hwnd, TIMER_CLOCK, CLOCK_TICK, NULL);
            SetTimer(hwnd, TIMER_KEYBOARD, KEYBOARD_TICK, NULL);
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case TIMER_CLOCK:
                    tetris_tick();
                    /* Here we have to determine
                       the invalidating rectangle.
                       For now, just invalidate the whole client area.
                       TODO: optimize this! */
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case TIMER_KEYBOARD:
                    handle_keystrokes(hwnd);
                    break;
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            draw_field(hdc);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, TIMER_CLOCK);
            KillTimer(hwnd, TIMER_KEYBOARD);
            PostQuitMessage(0);
            return 0;

        case WM_GETMINMAXINFO: {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = minimum_window_width;
            lpMMI->ptMinTrackSize.y = minimum_window_height;
            return 0;
        }

        case WM_SIZE: {
            /* Recalculate block size */
            hdc = GetDC(hwnd);
            RECT cr;
            GetClientRect(WindowFromDC(hdc), &cr);
            int block_width = (cr.right - cr.left) * 2 / 33;
            int block_height = (cr.bottom - cr.top) / 21;
            if (block_width < block_height)
                block_size = block_width;
            else
                block_size = block_height;
            block_border = block_size / 10;
            return 0;
        }

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

static void draw_field(HDC hdc) {
    int i, j;
    RECT rect;
    tetris_state_t *ts = tetris_get_state();
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    HPEN pen = CreatePen(PS_SOLID, block_border, RGB(0, 0, 0));
    int block_border_fix = block_border - block_border % 2;

    // Draw margin rectangle
    SelectObject(hdc, pen);
    MoveToEx(hdc, block_size / 4, block_size / 4, NULL);
    LineTo(hdc, block_size * (TETRIS_PLAYFIELD_WIDTH + 1) - block_size / 4, block_size / 4);
    LineTo(hdc, block_size * (TETRIS_PLAYFIELD_WIDTH + 1) - block_size / 4, block_size * (TETRIS_PLAYFIELD_HEIGHT + 1) - block_size / 4);
    LineTo(hdc, block_size / 4, block_size * (TETRIS_PLAYFIELD_HEIGHT + 1) - block_size / 4);
    LineTo(hdc, block_size / 4, block_size / 4);

    // Draw score
    SetBkMode(hdc, TRANSPARENT);
    TextOutA(hdc, block_size * (TETRIS_PLAYFIELD_WIDTH + 2), block_size / 4, "SCORE", 5);

    // Draw playfield
    for (i = 0; i < TETRIS_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < TETRIS_PLAYFIELD_HEIGHT; j++) {
            rect.left = i * block_size + block_size / 2;
            rect.top = j * block_size + block_size / 2;
            rect.right = rect.left + block_size;
            rect.bottom = rect.top + block_size;

            if (ts->playfield[i][j] == TETRIS_FIELD_EMPTY) {
                FillRect(hdc, &rect, background_brush);
                continue;
            }

            rect.left += block_border;
            rect.top += block_border;
            rect.right -= block_border;
            rect.bottom -= block_border;

            MoveToEx(hdc, rect.left, rect.top, NULL);
            LineTo(hdc, rect.right, rect.top);
            LineTo(hdc, rect.right, rect.bottom);
            LineTo(hdc, rect.left, rect.bottom);
            LineTo(hdc, rect.left, rect.top);

            rect.left += block_border;
            rect.top += block_border;
            rect.right -= block_border_fix;
            rect.bottom -= block_border_fix;
            FillRect(hdc, &rect, brush);
        }
    }

    DeleteObject(pen);
}

static void handle_keystrokes(HWND hwnd) {
    if (GetAsyncKeyState(VK_RIGHT)) {
        tetris_right_key();
        /* Here we have to determine
           the invalidating rectangle.
           For now, just invalidate the whole client area.
           TODO: optimize this! */
        InvalidateRect(hwnd, NULL, FALSE);
    }

    if (GetAsyncKeyState(VK_LEFT)) {
        tetris_left_key();
        /* Here we have to determine
           the invalidating rectangle.
           For now, just invalidate the whole client area.
           TODO: optimize this! */
        InvalidateRect(hwnd, NULL, FALSE);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        tetris_down_key();
        /* Here we have to determine
           the invalidating rectangle.
           For now, just invalidate the whole client area.
           TODO: optimize this! */
        InvalidateRect(hwnd, NULL, FALSE);
    }

    if (GetAsyncKeyState(VK_UP)) {
        tetris_up_key();
        /* Here we have to determine
           the invalidating rectangle.
           For now, just invalidate the whole client area.
           TODO: optimize this! */
        InvalidateRect(hwnd, NULL, FALSE);
    }
}
