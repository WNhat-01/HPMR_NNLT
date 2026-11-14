#ifndef GFX_H
#define GFX_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 760

typedef uint32_t Color32;

#define RGB32(r, g, b) ((Color32)(((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | ((uint8_t)(b))))

/* Bang mau Dark-Mode PCB chuan ky thuat */
#define COL_BG_DARK      RGB32(15, 20, 28)
#define COL_BG_PANEL     RGB32(24, 32, 44)
#define COL_BG_CARD      RGB32(32, 43, 58)
#define COL_GRID_DOT     RGB32(42, 56, 75)
#define COL_BORDER       RGB32(58, 76, 102)
#define COL_TEXT_WHITE   RGB32(238, 244, 252)
#define COL_TEXT_MUTED   RGB32(148, 165, 188)
#define COL_TEXT_GOLD    RGB32(255, 206, 68)
#define COL_WIRE_LOW     RGB32(72, 92, 118)
#define COL_WIRE_HIGH    RGB32(0, 235, 145)
#define COL_WIRE_PULSE   RGB32(170, 255, 225)
#define COL_ACCENT_BLUE  RGB32(56, 152, 255)
#define COL_ACCENT_GREEN RGB32(36, 205, 125)
#define COL_ACCENT_RED   RGB32(242, 78, 92)
#define COL_ACCENT_WARN  RGB32(255, 168, 42)

/* Trang thai chuot & ban phim trong khung hinh */
typedef struct {
    int mouse_x;
    int mouse_y;
    bool left_down;
    bool left_pressed;
    bool left_released;
    bool right_pressed;
    bool key_esc_pressed;
    bool key_space_pressed;
    bool key_r_pressed;
    bool key_h_pressed;
} InputState;

/* Khoi tao va dong cua so 2D */
bool Gfx_Init(const char *title, int width, int height);
bool Gfx_PollEvents(InputState *input);
void Gfx_BeginFrame(Color32 clear_color);
void Gfx_EndFrame(void);
void Gfx_Shutdown(void);
double Gfx_GetTimeSeconds(void);

/* Cac ham ve 2D co ban (Framebuffer + GDI Text) */
void Gfx_DrawRect(int x, int y, int w, int h, Color32 color);
void Gfx_DrawRectOutline(int x, int y, int w, int h, int thickness, Color32 color);
void Gfx_DrawRoundedRect(int x, int y, int w, int h, int radius, Color32 fill_col, Color32 border_col);
void Gfx_DrawCircle(int cx, int cy, int radius, Color32 fill_col);
void Gfx_DrawCircleOutline(int cx, int cy, int radius, int thickness, Color32 col);
void Gfx_DrawLine(float x0, float y0, float x1, float y1, int thickness, Color32 col);
void Gfx_DrawBezierWire(float x0, float y0, float x1, float y1, bool is_high, double anim_time);

/* Cac ham ve chu (Font sizes: 1 = Nho 13px, 2 = Vua 16px, 3 = Lon 22px, 4 = Tieu de 30px) */
void Gfx_DrawText(int x, int y, const char *text, int font_size, Color32 color);
void Gfx_DrawTextCentered(int cx, int cy, const char *text, int font_size, Color32 color);

/* Kiem tra diem (mx, my) co nam trong hinh chu nhat khong */
bool Gfx_PointInRect(int mx, int my, int x, int y, int w, int h);

#endif /* GFX_H */
