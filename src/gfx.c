/*
 * ============================================================================
 * 2D GRAPHICS BACKEND (gfx.c)
 * Su dung Win32 GDI Double-Buffering + 32-bit DIB Framebuffer
 * Uu diem:
 *   - Khong can cai dat thu vien ngoai, bien dich chay ngay voi GCC tren Windows
 *   - Ho tro ve duong cong Bezier muot ma, hieu ung xung dien va ClearType Font
 * ============================================================================
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/gfx.h"

static HWND      g_hwnd = NULL;
static HDC       g_mem_dc = NULL;
static HBITMAP   g_dib_bmp = NULL;
static uint32_t *g_pixels = NULL;
static int       g_width = SCREEN_WIDTH;
static int       g_height = SCREEN_HEIGHT;
static bool      g_running = true;
static HFONT     g_fonts[5] = {NULL};

static LARGE_INTEGER g_freq;
static LARGE_INTEGER g_start_counter;

/* Trang thai Input tich luy tu Window Procedure */
static int  s_mouse_x = 0;
static int  s_mouse_y = 0;
static bool s_left_down = false;
static bool s_left_pressed = false;
static bool s_left_released = false;
static bool s_right_pressed = false;
static bool s_key_esc = false;
static bool s_key_space = false;
static bool s_key_r = false;
static bool s_key_h = false;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
        case WM_DESTROY:
            g_running = false;
            PostQuitMessage(0);
            return 0;

        case WM_MOUSEMOVE:
            s_mouse_x = (int)(short)LOWORD(lParam);
            s_mouse_y = (int)(short)HIWORD(lParam);
            return 0;

        case WM_LBUTTONDOWN:
            s_mouse_x = (int)(short)LOWORD(lParam);
            s_mouse_y = (int)(short)HIWORD(lParam);
            s_left_down = true;
            s_left_pressed = true;
            SetCapture(hwnd);
            return 0;

        case WM_LBUTTONUP:
            s_mouse_x = (int)(short)LOWORD(lParam);
            s_mouse_y = (int)(short)HIWORD(lParam);
            s_left_down = false;
            s_left_released = true;
            ReleaseCapture();
            return 0;

        case WM_RBUTTONDOWN:
            s_mouse_x = (int)(short)LOWORD(lParam);
            s_mouse_y = (int)(short)HIWORD(lParam);
            s_right_pressed = true;
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) s_key_esc = true;
            else if (wParam == VK_SPACE) s_key_space = true;
            else if (wParam == 'R' || wParam == 'r') s_key_r = true;
            else if (wParam == 'H' || wParam == 'h') s_key_h = true;
            return 0;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

bool Gfx_Init(const char *title, int width, int height) {
    g_width = width;
    g_height = height;

    QueryPerformanceFrequency(&g_freq);
    QueryPerformanceCounter(&g_start_counter);

    HINSTANCE hInst = GetModuleHandleA(NULL);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "CircuitPuzzleWndClass";

    if (!RegisterClassA(&wc)) return false;

    RECT rect = { 0, 0, width, height };
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRect(&rect, style, FALSE);

    int win_w = rect.right - rect.left;
    int win_h = rect.bottom - rect.top;
    int scr_w = GetSystemMetrics(SM_CXSCREEN);
    int scr_h = GetSystemMetrics(SM_CYSCREEN);
    int win_x = (scr_w - win_w) / 2;
    int win_y = (scr_h - win_h) / 2;
    if (win_x < 0) win_x = 0;
    if (win_y < 0) win_y = 0;

    g_hwnd = CreateWindowExA(
        0, wc.lpszClassName, title, style,
        win_x, win_y, win_w, win_h,
        NULL, NULL, hInst, NULL
    );
    if (!g_hwnd) return false;

    HDC hdc = GetDC(g_hwnd);
    g_mem_dc = CreateCompatibleDC(hdc);

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; /* Top-down DIB */
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_dib_bmp = CreateDIBSection(g_mem_dc, &bmi, DIB_RGB_COLORS, (void**)&g_pixels, NULL, 0);
    SelectObject(g_mem_dc, g_dib_bmp);
    SetBkMode(g_mem_dc, TRANSPARENT);
    ReleaseDC(g_hwnd, hdc);

    /* Khoi tao 4 kich co Font Segoe UI */
    /* Khoi tao 4 kich co Font Segoe UI ho tro Unicode */
    g_fonts[1] = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_fonts[2] = CreateFontW(-17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_fonts[3] = CreateFontW(-22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_fonts[4] = CreateFontW(-30, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    g_running = true;
    return true;
}

bool Gfx_PollEvents(InputState *input) {
    s_left_pressed = false;
    s_left_released = false;
    s_right_pressed = false;
    s_key_esc = false;
    s_key_space = false;
    s_key_r = false;
    s_key_h = false;

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            g_running = false;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    if (input) {
        input->mouse_x = s_mouse_x;
        input->mouse_y = s_mouse_y;
        input->left_down = s_left_down;
        input->left_pressed = s_left_pressed;
        input->left_released = s_left_released;
        input->right_pressed = s_right_pressed;
        input->key_esc_pressed = s_key_esc;
        input->key_space_pressed = s_key_space;
        input->key_r_pressed = s_key_r;
        input->key_h_pressed = s_key_h;
    }
    return g_running;
}

double Gfx_GetTimeSeconds(void) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - g_start_counter.QuadPart) / (double)g_freq.QuadPart;
}

static inline void PutPixelFast(int x, int y, Color32 color) {
    if ((unsigned)x < (unsigned)g_width && (unsigned)y < (unsigned)g_height) {
        g_pixels[y * g_width + x] = color;
    }
}

void Gfx_BeginFrame(Color32 clear_color) {
    if (!g_pixels) return;
    int total = g_width * g_height;
    for (int i = 0; i < total; i++) {
        g_pixels[i] = clear_color;
    }
}

void Gfx_EndFrame(void) {
    if (!g_hwnd || !g_mem_dc) return;
    HDC hdc = GetDC(g_hwnd);
    BitBlt(hdc, 0, 0, g_width, g_height, g_mem_dc, 0, 0, SRCCOPY);
    ReleaseDC(g_hwnd, hdc);
    Sleep(14); /* Gioi han ~60 FPS muot ma, tiet kiem CPU */
}

void Gfx_Shutdown(void) {
    for (int i = 1; i <= 4; i++) {
        if (g_fonts[i]) DeleteObject(g_fonts[i]);
    }
    if (g_dib_bmp) DeleteObject(g_dib_bmp);
    if (g_mem_dc) DeleteDC(g_mem_dc);
    if (g_hwnd) DestroyWindow(g_hwnd);
}

void Gfx_DrawRect(int x, int y, int w, int h, Color32 color) {
    int x0 = (x < 0) ? 0 : x;
    int y0 = (y < 0) ? 0 : y;
    int x1 = (x + w > g_width) ? g_width : (x + w);
    int y1 = (y + h > g_height) ? g_height : (y + h);

    for (int py = y0; py < y1; py++) {
        uint32_t *row = &g_pixels[py * g_width];
        for (int px = x0; px < x1; px++) {
            row[px] = color;
        }
    }
}

void Gfx_DrawRectOutline(int x, int y, int w, int h, int thickness, Color32 color) {
    Gfx_DrawRect(x, y, w, thickness, color);
    Gfx_DrawRect(x, y + h - thickness, w, thickness, color);
    Gfx_DrawRect(x, y, thickness, h, color);
    Gfx_DrawRect(x + w - thickness, y, thickness, h, color);
}

void Gfx_DrawRoundedRect(int x, int y, int w, int h, int radius, Color32 fill_col, Color32 border_col) {
    if (radius < 2) {
        Gfx_DrawRect(x, y, w, h, fill_col);
        Gfx_DrawRectOutline(x, y, w, h, 2, border_col);
        return;
    }
    Gfx_DrawRect(x, y, w, h, fill_col);
    Gfx_DrawRectOutline(x, y, w, h, 2, border_col);
    /* Bo goc nhe 2px */
    PutPixelFast(x, y, COL_BG_DARK);
    PutPixelFast(x + w - 1, y, COL_BG_DARK);
    PutPixelFast(x, y + h - 1, COL_BG_DARK);
    PutPixelFast(x + w - 1, y + h - 1, COL_BG_DARK);
}

void Gfx_DrawCircle(int cx, int cy, int radius, Color32 fill_col) {
    int r2 = radius * radius;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= r2) {
                PutPixelFast(cx + dx, cy + dy, fill_col);
            }
        }
    }
}

void Gfx_DrawCircleOutline(int cx, int cy, int radius, int thickness, Color32 col) {
    int r_outer2 = radius * radius;
    int r_inner = (radius > thickness) ? (radius - thickness) : 0;
    int r_inner2 = r_inner * r_inner;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int d2 = dx * dx + dy * dy;
            if (d2 <= r_outer2 && d2 >= r_inner2) {
                PutPixelFast(cx + dx, cy + dy, col);
            }
        }
    }
}

void Gfx_DrawLine(float x0, float y0, float x1, float y1, int thickness, Color32 col) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float steps = fabsf(dx) > fabsf(dy) ? fabsf(dx) : fabsf(dy);
    if (steps < 1.0f) {
        Gfx_DrawCircle((int)x0, (int)y0, thickness / 2, col);
        return;
    }
    float sx = dx / steps;
    float sy = dy / steps;
    float x = x0;
    float y = y0;
    int r = (thickness > 1) ? (thickness / 2) : 1;

    for (int i = 0; i <= (int)steps; i++) {
        for (int oy = -r; oy <= r; oy++) {
            for (int ox = -r; ox <= r; ox++) {
                PutPixelFast((int)x + ox, (int)y + oy, col);
            }
        }
        x += sx;
        y += sy;
    }
}

void Gfx_DrawBezierWire(float x0, float y0, float x1, float y1, bool is_high, double anim_time) {
    float ctrl_dist = fabsf(x1 - x0) * 0.5f;
    if (ctrl_dist < 45.0f) ctrl_dist = 45.0f;

    float cx0 = x0 + ctrl_dist;
    float cy0 = y0;
    float cx1 = x1 - ctrl_dist;
    float cy1 = y1;

    Color32 wire_col = is_high ? COL_WIRE_HIGH : COL_WIRE_LOW;
    const int segments = 40;
    float prev_x = x0;
    float prev_y = y0;

    for (int i = 1; i <= segments; i++) {
        float t = (float)i / (float)segments;
        float u = 1.0f - t;
        float b0 = u * u * u;
        float b1 = 3.0f * u * u * t;
        float b2 = 3.0f * u * t * t;
        float b3 = t * t * t;

        float px = b0 * x0 + b1 * cx0 + b2 * cx1 + b3 * x1;
        float py = b0 * y0 + b1 * cy0 + b2 * cy1 + b3 * y1;

        Gfx_DrawLine(prev_x, prev_y, px, py, 3, wire_col);
        prev_x = px;
        prev_y = py;
    }

    /* Hieu ung hat xung dien (Electron Pulse) chay doc theo day khi muc logic = 1 */
    if (is_high) {
        for (int p = 0; p < 3; p++) {
            float t = (float)fmod(anim_time * 0.85 + p * 0.333, 1.0);
            float u = 1.0f - t;
            float px = (u*u*u)*x0 + (3.0f*u*u*t)*cx0 + (3.0f*u*t*t)*cx1 + (t*t*t)*x1;
            float py = (u*u*u)*y0 + (3.0f*u*u*t)*cy0 + (3.0f*u*t*t)*cy1 + (t*t*t)*y1;
            Gfx_DrawCircle((int)px, (int)py, 4, COL_WIRE_PULSE);
        }
    }
}

static COLORREF ToGdiColor(Color32 c) {
    uint8_t r = (uint8_t)((c >> 16) & 0xFF);
    uint8_t g = (uint8_t)((c >> 8) & 0xFF);
    uint8_t b = (uint8_t)(c & 0xFF);
    return RGB(r, g, b);
}

void Gfx_DrawText(int x, int y, const char *text, int font_size, Color32 color) {
    if (!text || !g_mem_dc) return;
    if (font_size < 1) font_size = 1;
    if (font_size > 4) font_size = 4;

    /* Chuyen doi chuoi UTF-8 (char) sang UTF-16 (wchar_t) de Win32 GDI doc duoc Tieng Viet */
    wchar_t wtext[512];
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 512);

    GdiFlush();
    SelectObject(g_mem_dc, g_fonts[font_size]);
    SetTextColor(g_mem_dc, ToGdiColor(color));
    TextOutW(g_mem_dc, x, y, wtext, lstrlenW(wtext));
    GdiFlush();
}

void Gfx_DrawTextCentered(int cx, int cy, const char *text, int font_size, Color32 color) {
    if (!text || !g_mem_dc) return;
    if (font_size < 1) font_size = 1;
    if (font_size > 4) font_size = 4;

    /* Chuyen doi chuoi UTF-8 sang UTF-16 */
    wchar_t wtext[512];
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 512);

    GdiFlush();
    SelectObject(g_mem_dc, g_fonts[font_size]);
    SIZE sz;
    int wlen = lstrlenW(wtext);
    GetTextExtentPoint32W(g_mem_dc, wtext, wlen, &sz);

    SetTextColor(g_mem_dc, ToGdiColor(color));
    TextOutW(g_mem_dc, cx - sz.cx / 2, cy - sz.cy / 2, wtext, wlen);
    GdiFlush();
}

bool Gfx_PointInRect(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

void Gfx_DrawTextWrapped(int x, int y, int w, int h, const char *text, int font_size, Color32 color) {
    if (!text || !g_mem_dc) return;
    if (font_size < 1) font_size = 1;
    if (font_size > 4) font_size = 4;

    /* Chuyen doi chuoi UTF-8 sang UTF-16 */
    wchar_t wtext[1024]; /* Chuan bi buffer lon hon mot chut de chua chuoi dai */
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 1024);

    GdiFlush();
    SelectObject(g_mem_dc, g_fonts[font_size]);
    SetTextColor(g_mem_dc, ToGdiColor(color));

    /* Dinh nghia khung hinh chu nhat gioi han chu */
    RECT rect = { x, y, x + w, y + h };
    
    /* Su dung DrawTextW voi DT_WORDBREAK de tu dong xuong dong */
    DrawTextW(g_mem_dc, wtext, -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
    GdiFlush();
}