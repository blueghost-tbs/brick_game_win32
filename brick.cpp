#include "brick.h"
#include "game_interfaces.h"
#include "resource.h"

#include <windows.h>
#include <stdio.h>

#define CLIENTWIDTH    300 // 220 playfield + 80 scoreboard
#define CLIENTHEIGHT   420
#define WINDOWSTYLE    WS_OVERLAPPEDWINDOW

#define GAMELOOP_TICK  50
#define GAMELOOP_CLOCK 2

// Declaration of static functions
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void draw_field(HDC hdc);
static void invalidate_window_part(HWND hwnd);
static void reinitialize_block_bitmaps(HDC hdc);
static void set_font_size(HDC hdc, unsigned short size);
static void initialize_game_interfaces(void);
static void change_game(int game, HWND hwnd);

static int minimum_window_width = CLIENTWIDTH;
static int minimum_window_height = CLIENTHEIGHT;
static int block_size = 20;
static int block_border = 2;
static HBRUSH background_brush;
static HBITMAP block_bitmap_full = NULL;
static HBITMAP block_bitmap_empty = NULL;
static HFONT hf = NULL;

#define SCORE_TEXT_X  (block_size * (BRICK_PLAYFIELD_WIDTH + 1) + 5)
#define SCORE_TEXT_Y  (block_size / 4)
#define LEVEL_TEXT_X  (block_size * (BRICK_PLAYFIELD_WIDTH + 1) + 5)
#define LEVEL_TEXT_Y  (block_size / 4) + block_size * 2
#define NEXT_TEXT_X   (block_size * (BRICK_PLAYFIELD_WIDTH + 1) + 5)
#define NEXT_TEXT_Y   (block_size / 4) + block_size * 6

/* Games */
#define GAME_FIRST  0
#define GAME_TETRIS 0
#define GAME_SNAKE  1
#define GAME_LAST   1
game_interface_t games[GAME_LAST + 1];
static short active_game = GAME_LAST;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    background_brush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);

    static TCHAR szAppName[] = TEXT("BrickGame");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = background_brush;
    wndclass.lpszMenuName  = szAppName;
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

void reset_redraw_rectangle(redraw_rectangle_t *rr) {
    rr->left = BRICK_PLAYFIELD_WIDTH;
    rr->top = BRICK_PLAYFIELD_HEIGHT;
    rr->right = 0;
    rr->bottom = 0;
    rr->clean = 1;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
        case WM_CREATE:
            initialize_game_interfaces();
            change_game(GAME_FIRST, hwnd);
            SetTimer(hwnd, GAMELOOP_CLOCK, GAMELOOP_TICK, NULL);
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case GAMELOOP_CLOCK:
                    games[active_game].game_loop();
                    invalidate_window_part(hwnd);
                    /* Check for game over */
                    brick_state_t *ts = games[active_game].game_get_state();
                    if (ts->game_over_notification_flag) {
                        ts->game_over_notification_flag = false;
                        char message[100] = {'\0',};
                        _snprintf(message, 100, "Game over! Your final score is %lu.\nWould you like to start a new game?", ts->score);
                        if (MessageBox(hwnd, TEXT(message), TEXT("Game over"), MB_APPLMODAL | MB_ICONINFORMATION | MB_YESNO) == IDYES)
                            games[active_game].game_init();
                    }
                    break;
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            draw_field(hdc);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, GAMELOOP_CLOCK);
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
            int block_width = (cr.right - cr.left) * 2 / 30;
            int block_height = (cr.bottom - cr.top) / 21;
            if (block_width < block_height)
                block_size = block_width;
            else
                block_size = block_height;
            block_border = block_size / 10;
            if (block_border < 2)
                block_border = 2;
            reinitialize_block_bitmaps(hdc);
            set_font_size(hdc, block_size / 2);
            ReleaseDC(hwnd, hdc);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_RIGHT)
                games[active_game].game_right_key_press();
            else if (wParam == VK_LEFT)
                games[active_game].game_left_key_press();
            else if (wParam == VK_UP)
                games[active_game].game_up_key_press();
            else if (wParam == VK_DOWN)
                games[active_game].game_down_key_press();
            return 0;

        case WM_KEYUP:
            if (wParam == VK_RIGHT)
                games[active_game].game_right_key_release();
            else if (wParam == VK_LEFT)
                games[active_game].game_left_key_release();
            else if (wParam == VK_UP)
                games[active_game].game_up_key_release();
            else if (wParam == VK_DOWN)
                games[active_game].game_down_key_release();
            return 0;

        case WM_COMMAND:
            switch (LOWORD (wParam)) {
                case IDM_FILE_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_FILE_NEW_GAME:
                    games[active_game].game_init();
                    break;
                case IDM_HELP_ABOUT:
                    MessageBox(NULL, TEXT("Brick Game by Tibor Lajos Füzi\n\nWork-in-progress demo version 0.3"), TEXT("Brick Game"), MB_ICONINFORMATION);
                    break;
                case IDM_GAME_TETRIS:
                    change_game(GAME_TETRIS, hwnd);
                    break;
                case IDM_GAME_SNAKE:
                    change_game(GAME_SNAKE, hwnd);
                    break;
            }
            return 0;

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

static void draw_field(HDC hdc) {
    int i, j;
    RECT rect;
    brick_state_t *ts = games[active_game].game_get_state();
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    LOGBRUSH lbrush = {BS_SOLID, RGB(0, 0, 0), 0};
    HPEN pen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL, block_border, &lbrush, 0, NULL);
    char score[32] = {'\0',};
    char level[32] = {'\0',};

    // Draw margin rectangle
    SelectObject(hdc, pen);
    MoveToEx(hdc, block_size / 4, block_size / 4, NULL);
    LineTo(hdc, block_size * (BRICK_PLAYFIELD_WIDTH + 1) - block_size / 4, block_size / 4);
    LineTo(hdc, block_size * (BRICK_PLAYFIELD_WIDTH + 1) - block_size / 4, block_size * (BRICK_PLAYFIELD_HEIGHT + 1) - block_size / 4);
    LineTo(hdc, block_size / 4, block_size * (BRICK_PLAYFIELD_HEIGHT + 1) - block_size / 4);
    LineTo(hdc, block_size / 4, block_size / 4);

    // Set font & color
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, hf);

    // Draw score
    SetRect(&rect, SCORE_TEXT_X,
                   SCORE_TEXT_Y + block_size,
                   SCORE_TEXT_X + block_size * 4,
                   SCORE_TEXT_Y + 2 * block_size);
    // Delete the previous score because we don't erase the background with InvalidateRect to avoid flashing
    FillRect(hdc, &rect, background_brush);
    TextOutA(hdc, SCORE_TEXT_X, SCORE_TEXT_Y, "SCORE", 5);
    _snprintf(score, 31, "%lu", ts->score);
    TextOutA(hdc, SCORE_TEXT_X, SCORE_TEXT_Y + block_size, score, strlen(score));

    // Draw level
    SetRect(&rect, LEVEL_TEXT_X,
                   LEVEL_TEXT_Y + block_size,
                   LEVEL_TEXT_X + block_size * 4,
                   LEVEL_TEXT_Y + 2 * block_size);
    // Delete the previous level because we don't erase the background with InvalidateRect to avoid flashing
    FillRect(hdc, &rect, background_brush);
    TextOutA(hdc, LEVEL_TEXT_X, LEVEL_TEXT_Y, "LEVEL", 5);
    _snprintf(level, 31, "%d", ts->level);
    TextOutA(hdc, LEVEL_TEXT_X, LEVEL_TEXT_Y + block_size, level, strlen(level));

    // Draw "next" label
    TextOutA(hdc, NEXT_TEXT_X, NEXT_TEXT_Y, "NEXT", 4);

    HDC hdcfull = CreateCompatibleDC(hdc);
    SelectObject(hdcfull, block_bitmap_full);
    HDC hdcempty = CreateCompatibleDC(hdc);
    SelectObject(hdcempty, block_bitmap_empty);

    // Draw next figure
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            rect.left = NEXT_TEXT_X + i * block_size;
            rect.top = (j + 7) * block_size + block_size / 2;
            rect.right =  rect.left + block_size;
            rect.bottom = rect.top + block_size;

            if (ts->next[i][j] == BRICK_FIELD_EMPTY) {
                BitBlt(hdc, rect.left, rect.top, block_size, block_size, hdcempty, 0, 0, SRCCOPY);
            } else
                BitBlt(hdc, rect.left, rect.top, block_size, block_size, hdcfull, 0, 0, SRCCOPY);
        }
    }

    // Draw playfield
    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            rect.left = i * block_size + block_size / 2;
            rect.top = j * block_size + block_size / 2;
            rect.right = rect.left + block_size;
            rect.bottom = rect.top + block_size;

            if (ts->playfield[i][j] == BRICK_FIELD_EMPTY) {
                BitBlt(hdc, rect.left, rect.top, block_size, block_size, hdcempty, 0, 0, SRCCOPY);
            } else
                BitBlt(hdc, rect.left, rect.top, block_size, block_size, hdcfull, 0, 0, SRCCOPY);
        }
    }

    DeleteDC(hdcfull);
    DeleteDC(hdcempty);
    DeleteObject(pen);
}

static void invalidate_window_part(HWND hwnd) {
    RECT rc = {0};

    brick_state_t *ts = games[active_game].game_get_state();
    if (!(ts->rr.clean)) {
        LONG offset = block_size / 2;
        rc.left = offset + ts->rr.left * block_size;
        rc.right = offset + (ts->rr.right + 1) * block_size;
        rc.top = offset + ts->rr.top * block_size;
        rc.bottom = offset + (ts->rr.bottom + 1) * block_size;
        InvalidateRect(hwnd, &rc, FALSE);
        reset_redraw_rectangle(&ts->rr);
    }

    if (ts->next_changed) {
        rc.left = NEXT_TEXT_X;
        rc.top = 7 * block_size + block_size / 2;
        rc.right =  rc.left + block_size * 4;
        rc.bottom = rc.top + block_size * 4;
        InvalidateRect(hwnd, &rc, FALSE);
        games[active_game].game_next_figure_accepted();
    }

    if (ts->score_changed) {
        SetRect(&rc, SCORE_TEXT_X,
                     SCORE_TEXT_Y + block_size,
                     SCORE_TEXT_X + block_size * 4,
                     SCORE_TEXT_Y + 2 * block_size);
        InvalidateRect(hwnd, &rc, FALSE);
        ts->score_changed = 0;
    }

    if (ts->level_changed) {
        SetRect(&rc, LEVEL_TEXT_X,
                     LEVEL_TEXT_Y + block_size,
                     LEVEL_TEXT_X + block_size * 4,
                     LEVEL_TEXT_Y + 2 * block_size);
        InvalidateRect(hwnd, &rc, FALSE);
        ts->level_changed = 0;
    }
}

static void reinitialize_block_bitmaps(HDC hdc) {
    // Memory allocation for new bitmaps
    unsigned char *bitmap_full = (unsigned char*)malloc(block_size * block_size * 4);
    if (bitmap_full == NULL) {
        // Not enough memory!
        return;
    }
    memset(bitmap_full, 192, block_size * block_size * 4);
    unsigned char *bitmap_empty = (unsigned char*)malloc(block_size * block_size * 4);
    if (bitmap_empty == NULL) {
        // Not enough memory!
        free(bitmap_full);
        return;
    }
    memset(bitmap_empty, 192, block_size * block_size * 4);

    // Create and initialize BITMAPINFOHEADER for the 32-bit image
    BITMAPINFOHEADER bmih;
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
    bmih.biWidth = block_size;
    bmih.biHeight = block_size;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biPlanes = 1;
    bmih.biSizeImage = block_size * block_size * 4;

    // Create and initialize BITMAPINFO for the 32-bit image
    PBITMAPINFO pbmi = (PBITMAPINFO)calloc(1, sizeof(BITMAPINFOHEADER) + 3 * sizeof(DWORD));
    pbmi->bmiHeader.biWidth = block_size;
    pbmi->bmiHeader.biHeight = block_size;
    pbmi->bmiHeader.biBitCount = 32;
    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biSizeImage = block_size * block_size * 4;

    // Fill bitmaps
    int i, j;
    // Borders
    int outer_border = block_border * 7 / 10;
    for (i = outer_border; i < block_size - outer_border; i++) {
        for (j = outer_border; j < outer_border + block_border; j++) {
            // Vertical border
            bitmap_full[(i * block_size + j) * 4] = 0;
            bitmap_full[(i * block_size + j) * 4 + 1] = 0;
            bitmap_full[(i * block_size + j) * 4 + 2] = 0;
            bitmap_empty[(i * block_size + j) * 4] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 1] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 2] = 160;
            // Horizontal border (i and j are swapped)
            bitmap_full[(j * block_size + i) * 4] = 0;
            bitmap_full[(j * block_size + i) * 4 + 1] = 0;
            bitmap_full[(j * block_size + i) * 4 + 2] = 0;
            bitmap_empty[(j * block_size + i) * 4] = 160;
            bitmap_empty[(j * block_size + i) * 4 + 1] = 160;
            bitmap_empty[(j * block_size + i) * 4 + 2] = 160;
        }
        for (j = block_size - block_border - outer_border; j < block_size - outer_border; j++) {
            // Vertical border
            bitmap_full[(i * block_size + j) * 4] = 0;
            bitmap_full[(i * block_size + j) * 4 + 1] = 0;
            bitmap_full[(i * block_size + j) * 4 + 2] = 0;
            bitmap_empty[(i * block_size + j) * 4] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 1] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 2] = 160;
            // Horizontal border (i and j are swapped)
            bitmap_full[(j * block_size + i) * 4] = 0;
            bitmap_full[(j * block_size + i) * 4 + 1] = 0;
            bitmap_full[(j * block_size + i) * 4 + 2] = 0;
            bitmap_empty[(j * block_size + i) * 4] = 160;
            bitmap_empty[(j * block_size + i) * 4 + 1] = 160;
            bitmap_empty[(j * block_size + i) * 4 + 2] = 160;
        }
    }
    // Inner rectangle
    for (i = block_border * 2; i < block_size - block_border * 2; i++) {
        for (j = block_border * 2; j < block_size - block_border * 2; j++) {
            bitmap_full[(i * block_size + j) * 4] = 0;
            bitmap_full[(i * block_size + j) * 4 + 1] = 0;
            bitmap_full[(i * block_size + j) * 4 + 2] = 0;
            bitmap_empty[(i * block_size + j) * 4] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 1] = 160;
            bitmap_empty[(i * block_size + j) * 4 + 2] = 160;
        }
    }

    // Delete previous bitmaps
    if (block_bitmap_full != NULL)
        DeleteObject(block_bitmap_full);
    if (block_bitmap_empty != NULL)
        DeleteObject(block_bitmap_empty);

    block_bitmap_full = CreateDIBitmap(hdc, &bmih, CBM_INIT, bitmap_full, pbmi, DIB_RGB_COLORS);
    block_bitmap_empty = CreateDIBitmap(hdc, &bmih, CBM_INIT, bitmap_empty, pbmi, DIB_RGB_COLORS);

    free(pbmi);
    free(bitmap_full);
    free(bitmap_empty);
}

static void set_font_size(HDC hdc, unsigned short size) {
    long lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);

    if (hf != NULL)
        DeleteObject(hf);
    hf = CreateFont(lfHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
}

static void initialize_game_interfaces(void) {
    tetris_init_interface(&games[GAME_TETRIS]);
    snake_init_interface(&games[GAME_SNAKE]);
}

static void change_game(int game, HWND hwnd) {
    HMENU menu = GetMenu(hwnd);
    HMENU menu_game = GetSubMenu(menu, 1);

    if (active_game != game) {
        active_game = game;
        games[active_game].game_init();
        CheckMenuRadioItem(menu_game, GAME_FIRST, GAME_LAST, active_game, MF_BYPOSITION);
    }
}
