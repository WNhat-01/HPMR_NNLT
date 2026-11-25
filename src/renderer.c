/*
 * ============================================================================
 * MODULE 3: GRAPHICS & COMPONENT RENDERER (renderer.c)
 * Phu trach: Thanh vien 3
 * Noi dung:
 *   - Ve bang mach PCB (Grid, Cong Logic, Cong tac Input, Den LED Output)
 *   - Ve day dan Bezier mem mai kem hieu ung xung dien chay khi HIGH (1)
 *   - Ve Bang Chan Tri (Truth Table) truc quan so sanh Expected vs Actual
 * ============================================================================
 */

#include "../include/game.h"
#include <stdio.h>
#include <math.h>

Color32 Renderer_GetGateColor(ComponentType type) {
    switch (type) {
        case COMP_INPUT:  return RGB32(45, 95, 165);
        case COMP_OUTPUT: return RGB32(50, 75, 95);
        case COMP_NOT:    return RGB32(215, 72, 96);
        case COMP_AND:    return RGB32(42, 138, 225);
        case COMP_OR:     return RGB32(38, 175, 118);
        case COMP_XOR:    return RGB32(158, 92, 230);
        case COMP_NAND:   return RGB32(230, 126, 45);
        case COMP_NOR:    return RGB32(36, 172, 185);
        case COMP_XNOR:   return RGB32(212, 85, 185);
        default:          return COL_BG_CARD;
    }
}

void Renderer_GetPinPos(const Component *comp, bool is_output, int pin_index, float *out_x, float *out_y) {
    if (!comp || !out_x || !out_y) return;

    if (is_output) {
        *out_x = comp->x + comp->width;
        *out_y = comp->y + comp->height * 0.5f;
    } else {
        *out_x = comp->x;
        if (comp->num_inputs <= 1) {
            *out_y = comp->y + comp->height * 0.5f;
        } else {
            if (pin_index == 0) {
                *out_y = comp->y + comp->height * 0.30f;
            } else {
                *out_y = comp->y + comp->height * 0.70f;
            }
        }
    }
}

void Renderer_DrawBoardGrid(int x, int y, int w, int h) {
    Gfx_DrawRoundedRect(x, y, w, h, 8, COL_BG_DARK, COL_BORDER);

    /* Ve luoi cham PCB */
    const int step = 24;
    for (int gy = y + step; gy < y + h - 8; gy += step) {
        for (int gx = x + step; gx < x + w - 8; gx += step) {
            Gfx_DrawRect(gx, gy, 2, 2, COL_GRID_DOT);
        }
    }
}

/* Hàm hỗ trợ vẽ đường cong mượt mà bằng toán học */
static void DrawCurve(float x1, float y1, float x2, float y2, float cx, float cy, int thickness, Color32 color) {
    int segments = 15; /* Số nét vẽ chia nhỏ */
    float px = x1, py = y1;
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float nx = (1-t)*(1-t)*x1 + 2*(1-t)*t*cx + t*t*x2;
        float ny = (1-t)*(1-t)*y1 + 2*(1-t)*t*cy + t*t*y2;
        Gfx_DrawLine((int)px, (int)py, (int)nx, (int)ny, thickness, color);
        px = nx; py = ny;
    }
}

static void DrawComponentItem(const Component *c, bool is_hovered, const EditorState *editor) {
    int x = (int)c->x;
    int y = (int)c->y;
    int w = (int)c->width;
    int h = (int)c->height;

    Color32 base_col = Renderer_GetGateColor(c->type);
    Color32 border_col = is_hovered ? COL_TEXT_GOLD : COL_BORDER;

    if (c->type == COMP_INPUT) {
        /* Ve cong tac INPUT (Click de bat/tat 0 hoac 1) */
        Color32 fill = c->output_value ? RGB32(22, 130, 85) : RGB32(36, 48, 66);
        Color32 brd  = c->output_value ? COL_WIRE_HIGH : border_col;
        Gfx_DrawRoundedRect(x, y, w, h, 8, fill, brd);

        /* Nut gat Switch */
        int sw_x = c->output_value ? (x + w - 34) : (x + 8);
        Color32 knob_col = c->output_value ? COL_WIRE_HIGH : RGB32(110, 128, 152);
        Gfx_DrawRoundedRect(sw_x, y + 8, 24, h - 16, 5, knob_col, COL_TEXT_WHITE);

        char buf[32];
        snprintf(buf, sizeof(buf), "%s:%d", c->label, c->output_value);
        Gfx_DrawTextCentered(x + w / 2 - 4, y + h / 2, buf, 2, COL_TEXT_WHITE);
    } else if (c->type == COMP_OUTPUT) {
        /* Ve den LED OUTPUT */
        Color32 fill = c->output_value ? RGB32(24, 115, 78) : RGB32(32, 42, 56);
        Color32 brd  = c->output_value ? COL_WIRE_HIGH : border_col;
        Gfx_DrawRoundedRect(x, y, w, h, 10, fill, brd);

        /* Bong den LED tron */
        int led_cx = x + 24;
        int led_cy = y + h / 2;
        if (c->output_value) {
            Gfx_DrawCircle(led_cx, led_cy, 15, RGB32(40, 200, 130));
            Gfx_DrawCircle(led_cx, led_cy, 10, COL_WIRE_PULSE);
        } else {
            Gfx_DrawCircle(led_cx, led_cy, 11, RGB32(55, 70, 90));
            Gfx_DrawCircleOutline(led_cx, led_cy, 11, 2, COL_BORDER);
        }

        char buf[32];
        snprintf(buf, sizeof(buf), "%s=%d", c->label, c->output_value);
        Gfx_DrawText(x + 42, y + h / 2 - 9, buf, 2, c->output_value ? COL_WIRE_PULSE : COL_TEXT_MUTED);
    
    // ... [Phần trên của hàm giữ nguyên (vẽ Input, Output)] ...

    } else {
        /* Ve Cong Logic (AND, OR, NOT, XOR, NAND, NOR, XNOR) bang code thuan (Cach 2) */
        Color32 brd = c->output_value ? COL_WIRE_HIGH : border_col;
        int cx = x + w / 2;
        int cy = y + h / 2;

        /* Xoa vung chu nhat mac dinh de de len bang hinh dang rieng */
        Gfx_DrawRoundedRect(x, y, w, h, 8, RGB32(28, 38, 52), RGB32(28, 38, 52)); 

        switch (c->type) {
            case COMP_NOT:
                /* Tam giac cho NOT */
                Gfx_DrawLine(x + 10, y + 10, x + w - 15, cy, 2, brd);
                Gfx_DrawLine(x + 10, y + h - 10, x + w - 15, cy, 2, brd);
                Gfx_DrawLine(x + 10, y + 10, x + 10, y + h - 10, 2, brd);
                break;
                
            case COMP_AND:
            case COMP_NAND:
                /* Lưng phẳng, mũi cong (Dùng hàm DrawCurve kéo tròn ra phía trước) */
                Gfx_DrawLine(x + 10, y + 10, x + 10, y + h - 10, 2, brd);
                Gfx_DrawLine(x + 10, y + 10, cx, y + 10, 2, brd);
                Gfx_DrawLine(x + 10, y + h - 10, cx, y + h - 10, 2, brd);
                DrawCurve(cx, y + 10, cx, y + h - 10, x + w + 15, cy, 2, brd);
                break;
                
            case COMP_OR:
            case COMP_NOR:
                /* Lưng móp vào trong, 2 cạnh cong vuốt ra chóp */
                DrawCurve(x + 5, y + 10, x + 5, y + h - 10, x + 25, cy, 2, brd);         
                DrawCurve(x + 5, y + 10, x + w - 15, cy, cx + 5, y - 5, 2, brd);         
                DrawCurve(x + 5, y + h - 10, x + w - 15, cy, cx + 5, y + h + 5, 2, brd); 
                break;
                
            case COMP_XOR:
            case COMP_XNOR:
                /* Giống OR nhưng thêm đường song song cách 7 pixel */
                DrawCurve(x + 5, y + 10, x + 5, y + h - 10, x + 25, cy, 2, brd); 
                DrawCurve(x + 12, y + 10, x + 12, y + h - 10, x + 32, cy, 2, brd);       
                
                DrawCurve(x + 12, y + 10, x + w - 15, cy, cx + 10, y - 5, 2, brd);
                DrawCurve(x + 12, y + h - 10, x + w - 15, cy, cx + 10, y + h + 5, 2, brd);
                break;
            default:
                break;
        }

        /* Diem nhan mau sac de biet dang hoat dong hay khong (Fix warning base_col) */
        Gfx_DrawCircle(cx, cy, 3, base_col);

        /* Vong tron (Bubble) dao nguoc cho cac cong NOT, NAND, NOR, XNOR */
        if (c->type == COMP_NOT || c->type == COMP_NAND || c->type == COMP_NOR || c->type == COMP_XNOR) {
            Gfx_DrawCircle(x + w - 10, cy, 5, RGB32(28, 38, 52)); /* Xoa nen trong */
            Gfx_DrawCircleOutline(x + w - 10, cy, 5, 2, COL_TEXT_WHITE); /* Vien trang */
        }
    }

    /* Ve cac chan Input Pin (ben trai) */
// ... [Phần dưới của hàm giữ nguyên (vẽ Node đầu vào, đầu ra)] ...

    /* Ve cac chan Input Pin (ben trai) */
    for (int p = 0; p < c->num_inputs; p++) {
        float px, py;
        Renderer_GetPinPos(c, false, p, &px, &py);
        bool pin_high = (c->input_values[p] != 0);
        bool pin_hover = (editor && editor->hovered_pin_comp_id == c->id && editor->hovered_pin_index == p);

        int radius = pin_hover ? 8 : 6;
        Color32 pcol = pin_high ? COL_WIRE_HIGH : COL_WIRE_LOW;
        if (pin_hover) pcol = COL_TEXT_GOLD;

        Gfx_DrawCircle((int)px, (int)py, radius, pcol);
        Gfx_DrawCircleOutline((int)px, (int)py, radius, 2, COL_TEXT_WHITE);
    }

    /* Ve chan Output Pin (ben phai) */
    if (c->num_outputs > 0) {
        float px, py;
        Renderer_GetPinPos(c, true, 0, &px, &py);
        bool pin_high = (c->output_value != 0);
        bool pin_hover = (editor && editor->hovered_pin_comp_id == c->id && editor->hovered_pin_index == -1);

        int radius = pin_hover ? 8 : 6;
        Color32 pcol = pin_high ? COL_WIRE_HIGH : COL_WIRE_LOW;
        if (pin_hover) pcol = COL_TEXT_GOLD;

        Gfx_DrawCircle((int)px, (int)py, radius, pcol);
        Gfx_DrawCircleOutline((int)px, (int)py, radius, 2, COL_TEXT_WHITE);
    }
}

void Renderer_DrawCircuit(const Circuit *circuit, const EditorState *editor, const InputState *input, double anim_time) {
    if (!circuit) return;

    /* 1. Ve tat ca cac soi day noi (Wires) */
    for (int w = 0; w < MAX_WIRES; w++) {
        const Wire *wire = &circuit->wires[w];
        if (!wire->active) continue;

        const Component *src = &circuit->components[wire->from_comp_id];
        const Component *dst = &circuit->components[wire->to_comp_id];
        if (!src->active || !dst->active) continue;

        float x0, y0, x1, y1;
        Renderer_GetPinPos(src, true, 0, &x0, &y0);
        Renderer_GetPinPos(dst, false, wire->to_pin_index, &x1, &y1);

        Gfx_DrawBezierWire(x0, y0, x1, y1, (wire->signal_value != 0), anim_time);
    }

    /* 2. Neu nguoi choi dang keo day moi tu 1 chan Output, ve day tam thoi */
    if (editor && editor->is_wiring && input) {
        int cid = editor->wire_from_comp_id;
        if (cid >= 0 && cid < MAX_COMPONENTS && circuit->components[cid].active) {
            float x0, y0;
            Renderer_GetPinPos(&circuit->components[cid], true, 0, &x0, &y0);
            bool is_high = (circuit->components[cid].output_value != 0);
            Gfx_DrawBezierWire(x0, y0, (float)input->mouse_x, (float)input->mouse_y, is_high, anim_time);
        }
    }

    /* 3. Ve tat ca cac linh kien tren bang mach */
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        const Component *c = &circuit->components[i];
        if (!c->active) continue;
        bool hovered = (editor && editor->hovered_comp_id == i);
        DrawComponentItem(c, hovered, editor);
    }
}

void Renderer_DrawTruthTable(const LevelDef *level, const VerificationResult *res, int x, int y, int w, int h) {
    if (!level || !res) return;

    Gfx_DrawRoundedRect(x, y, w, h, 8, COL_BG_PANEL, COL_BORDER);
    Gfx_DrawTextCentered(x + w / 2, y + 22, "BẢNG CHÂN TRỊ (TRUTH TABLE)", 2, COL_TEXT_GOLD);

    char summary[64];
    snprintf(summary, sizeof(summary), "Kết quả: %d / %d tổ hợp ĐÚNG", res->passed_rows, res->total_rows);
    Color32 sum_col = res->all_passed ? COL_ACCENT_GREEN : COL_TEXT_MUTED;
    Gfx_DrawTextCentered(x + w / 2, y + 44, summary, 1, sum_col);

    int table_x = x + 12;
    int table_y = y + 64;
    int table_w = w - 24;
    int total_cols = level->num_inputs + level->num_outputs * 2 + 1;
    int col_w = table_w / total_cols;
    int row_h = 28;

    /* Ve Header */
    Gfx_DrawRect(table_x, table_y, table_w, row_h, COL_BG_CARD);
    int col_idx = 0;
    for (int i = 0; i < level->num_inputs; i++) {
        Gfx_DrawTextCentered(table_x + col_idx * col_w + col_w / 2, table_y + row_h / 2, level->input_labels[i], 1, COL_ACCENT_BLUE);
        col_idx++;
    }
    for (int o = 0; o < level->num_outputs; o++) {
        char hdr[16];
        snprintf(hdr, sizeof(hdr), "%s(Mẫu)", level->output_labels[o]);
        Gfx_DrawTextCentered(table_x + col_idx * col_w + col_w / 2, table_y + row_h / 2, hdr, 1, COL_TEXT_GOLD);
        col_idx++;
    }
    for (int o = 0; o < level->num_outputs; o++) {
        char hdr[16];
        snprintf(hdr, sizeof(hdr), "%s(Mạch)", level->output_labels[o]);
        Gfx_DrawTextCentered(table_x + col_idx * col_w + col_w / 2, table_y + row_h / 2, hdr, 1, COL_TEXT_WHITE);
        col_idx++;
    }
    Gfx_DrawTextCentered(table_x + col_idx * col_w + col_w / 2, table_y + row_h / 2, "TT", 1, COL_TEXT_WHITE);

    /* Ve tung hang trong Bang Chan Tri */
    for (int r = 0; r < res->total_rows; r++) {
        int ry = table_y + (r + 1) * row_h;
        if (ry + row_h > y + h - 8) break;

        Color32 row_bg = res->row_passed[r] ? RGB32(18, 52, 42) : RGB32(54, 26, 34);
        Gfx_DrawRect(table_x, ry, table_w, row_h - 2, row_bg);

        int c = 0;
        for (int b = 0; b < level->num_inputs; b++) {
            int bit_val = (r >> (level->num_inputs - 1 - b)) & 1;
            char s[4] = { (char)('0' + bit_val), '\0' };
            Gfx_DrawTextCentered(table_x + c * col_w + col_w / 2, ry + row_h / 2 - 1, s, 1, COL_TEXT_WHITE);
            c++;
        }
        for (int o = 0; o < level->num_outputs; o++) {
            int exp_val = level->expected_outputs[r][o];
            char s[4] = { (char)('0' + exp_val), '\0' };
            Gfx_DrawTextCentered(table_x + c * col_w + col_w / 2, ry + row_h / 2 - 1, s, 1, COL_TEXT_GOLD);
            c++;
        }
        for (int o = 0; o < level->num_outputs; o++) {
            int act_val = res->actual_outputs[r][o];
            char s[4] = { (char)('0' + act_val), '\0' };
            Color32 vcol = (act_val == level->expected_outputs[r][o]) ? COL_WIRE_HIGH : COL_ACCENT_RED;
            Gfx_DrawTextCentered(table_x + c * col_w + col_w / 2, ry + row_h / 2 - 1, s, 1, vcol);
            c++;
        }
        Gfx_DrawTextCentered(
            table_x + c * col_w + col_w / 2,
            ry + row_h / 2 - 1,
            res->row_passed[r] ? "OK" : "SAI",
            1,
            res->row_passed[r] ? COL_WIRE_HIGH : COL_ACCENT_RED
        );
    }
}
