/*
 * ============================================================================
 * MODULE 5: UI SCREENS, MENU & GAME STATE FLOW (ui.c)
 * Phu trach: Thanh vien 5
 * Noi dung:
 *   - Quan ly chuyen canh: Main Menu -> Level Select -> Playing -> Victory Modal
 *   - Thanh cong cu Toolbox, Dong ho dem gio (< 5 phut), Tinh diem 1-3 Sao
 *   - Che do Sandbox tu do va Che do Demo Giang Vien (Unlock All)
 * ============================================================================
 */

#include "../include/game.h"
#include <stdio.h>
#include <string.h>

#define SAVE_FILE_PATH "progress.dat"

static bool DrawButton(int x, int y, int w, int h, const char *label, Color32 bg_col, const InputState *input) {
    bool hover = Gfx_PointInRect(input->mouse_x, input->mouse_y, x, y, w, h);
    Color32 fill = hover ? COL_ACCENT_BLUE : bg_col;
    Color32 brd  = hover ? COL_TEXT_GOLD : COL_BORDER;
    Gfx_DrawRoundedRect(x, y, w, h, 8, fill, brd);
    Gfx_DrawTextCentered(x + w / 2, y + h / 2, label, 2, COL_TEXT_WHITE);
    return (hover && input->left_pressed);
}

void Game_SetStatus(GameContext *game, const char *msg) {
    if (!game || !msg) return;
    strncpy(game->status_msg, msg, sizeof(game->status_msg) - 1);
    game->status_msg_timer = Gfx_GetTimeSeconds();
}

void Game_Init(GameContext *game) {
    if (!game) return;
    memset(game, 0, sizeof(GameContext));
    game->screen = STATE_MAIN_MENU;
    Interaction_Reset(&game->editor);

    if (!Progress_Load(&game->progress, SAVE_FILE_PATH)) {
        Progress_InitDefault(&game->progress);
    }
}

void Game_LoadLevel(GameContext *game, int level_idx) {
    if (!game) return;
    if (level_idx < 0) level_idx = 0;
    if (level_idx >= TOTAL_LEVELS) level_idx = TOTAL_LEVELS - 1;

    game->current_level_idx = level_idx;
    game->screen = STATE_PLAYING;
    game->show_hint = false;
    game->level_completed_modal = false;
    game->earned_stars = 0;
    game->level_start_time = Gfx_GetTimeSeconds();
    game->elapsed_seconds = 0;
    Interaction_Reset(&game->editor);

    const LevelDef *lvl = Levels_Get(level_idx);
    Levels_SetupCircuit(lvl, &game->circuit);
    Levels_VerifyCircuit(lvl, &game->circuit, &game->verify_res);

    Game_SetStatus(game, "Click vao cong logic o thanh duoi de dat len mach. Keo tu chan Phai sang chan Trai de noi day!");
}

void Game_StartSandbox(GameContext *game) {
    if (!game) return;
    game->screen = STATE_SANDBOX;
    game->show_hint = false;
    game->level_completed_modal = false;
    Interaction_Reset(&game->editor);
    Circuit_Init(&game->circuit);

    /* Tao san 3 Input A, B, C va 2 Output Y1, Y2 cho che do Sandbox */
    Circuit_AddComponent(&game->circuit, COMP_INPUT, 65.0f, 185.0f, true, "A");
    Circuit_AddComponent(&game->circuit, COMP_INPUT, 65.0f, 295.0f, true, "B");
    Circuit_AddComponent(&game->circuit, COMP_INPUT, 65.0f, 405.0f, true, "C");
    Circuit_AddComponent(&game->circuit, COMP_OUTPUT, 1080.0f, 235.0f, true, "Y1");
    Circuit_AddComponent(&game->circuit, COMP_OUTPUT, 1080.0f, 365.0f, true, "Y2");

    Game_SetStatus(game, "Che do SANDBOX: Tu do lap rap va thu nghiem bat ky mach logic nao!");
}

static void DrawMainMenu(GameContext *game, const InputState *input, double now) {
    /* Vung trang tri nen */
    Renderer_DrawBoardGrid(40, 40, SCREEN_WIDTH - 80, SCREEN_HEIGHT - 80);

    /* Tieu de lon */
    Gfx_DrawRoundedRect(240, 85, 800, 145, 12, COL_BG_PANEL, COL_ACCENT_BLUE);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 130, "CIRCUIT PUZZLE: LOGIC ARCHITECT", 4, COL_WIRE_HIGH);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 172, "Project Ngon Ngu C - Khoa Dien - Dien Tu HCMUT (Nhom 5 Sinh Vien)", 2, COL_TEXT_GOLD);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 202, "Thiet ke mach logic so, kiem tra Bang Chan Tri & chinh phuc 15 thu thach KTS!", 1, COL_TEXT_MUTED);

    /* Ve minh hoa 1 day xung dien chay o giua Menu */
    Gfx_DrawBezierWire(290.0f, 265.0f, 990.0f, 265.0f, true, now);

    int bx = SCREEN_WIDTH / 2 - 210;
    int bw = 420;
    int bh = 54;

    if (DrawButton(bx, 300, bw, bh, "CHON MAN CHOI (15 MAN KTS)", RGB32(28, 115, 78), input)) {
        game->screen = STATE_LEVEL_SELECT;
    }

    if (DrawButton(bx, 372, bw, bh, "PHONG THI NGHIEM TU DO (SANDBOX)", COL_BG_CARD, input)) {
        Game_StartSandbox(game);
    }

    if (DrawButton(bx, 444, bw, bh, "MO KHOA TAT CA 15 MAN (DEMO GIANG VIEN)", RGB32(95, 68, 25), input)) {
        game->progress.unlocked_level = TOTAL_LEVELS;
        Progress_Save(&game->progress, SAVE_FILE_PATH);
        game->screen = STATE_LEVEL_SELECT;
    }

    if (DrawButton(bx, 516, bw, bh, "DAT LAI TIEN DO (RESET SAVE)", RGB32(85, 36, 46), input)) {
        Progress_InitDefault(&game->progress);
        Progress_Save(&game->progress, SAVE_FILE_PATH);
    }

    /* Bang thong tin 5 Module phan cong nhom */
    Gfx_DrawRoundedRect(140, 595, 1000, 95, 8, COL_BG_PANEL, COL_BORDER);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 618, "CAU TRUC 5 MODULE TRONG PROJECT C (PHAN CONG 5 THANH VIEN):", 2, COL_TEXT_GOLD);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 646,
        "[1] circuit.c (Logic Simulation)   |   [2] levels.c (15 Levels & File I/O)   |   [3] renderer.c (2D PCB & Bezier)",
        1, COL_TEXT_WHITE);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 668,
        "[4] interaction.c (Mouse Drag & Wiring)   |   [5] ui.c & main.c (Menu, Star Rating, Timer < 5 phut)",
        1, COL_TEXT_MUTED);
}

static void DrawLevelSelect(GameContext *game, const InputState *input) {
    Gfx_DrawRoundedRect(30, 20, SCREEN_WIDTH - 60, 64, 8, COL_BG_PANEL, COL_BORDER);
    Gfx_DrawTextCentered(SCREEN_WIDTH / 2, 52, "CHON MAN CHOI (15 THU THACH MACH LOGIC - MOI MAN < 5 PHUT)", 3, COL_TEXT_GOLD);

    if (DrawButton(45, 32, 150, 40, "< VE MENU", COL_BG_CARD, input)) {
        game->screen = STATE_MAIN_MENU;
        return;
    }

    /* Hien thi tong so sao dat duoc */
    int total_stars = 0;
    for (int i = 0; i < TOTAL_LEVELS; i++) total_stars += game->progress.stars[i];
    char star_buf[64];
    snprintf(star_buf, sizeof(star_buf), "Tong Sao: %d / %d *", total_stars, TOTAL_LEVELS * 3);
    Gfx_DrawText(SCREEN_WIDTH - 230, 42, star_buf, 2, COL_WIRE_HIGH);

    const char *chap_names[3] = {
        "CHUONG 1: CO BAN - NHAP MON CONG LOGIC (MAN 1 - 5)",
        "CHUONG 2: TRUNG CAP - BIEN DOI BOOLE & DE MORGAN (MAN 6 - 10)",
        "CHUONG 3: KY SU KTS BACH KHOA - MACH CHUC NANG (MAN 11 - 15)"
    };

    for (int chap = 0; chap < 3; chap++) {
        int cy = 102 + chap * 210;
        Gfx_DrawText(50, cy, chap_names[chap], 2, COL_ACCENT_BLUE);

        for (int col = 0; col < 5; col++) {
            int idx = chap * 5 + col;
            const LevelDef *lvl = Levels_Get(idx);
            bool unlocked = (lvl->level_number <= game->progress.unlocked_level);

            int card_x = 45 + col * 242;
            int card_y = cy + 28;
            int card_w = 225;
            int card_h = 155;

            bool hover = Gfx_PointInRect(input->mouse_x, input->mouse_y, card_x, card_y, card_w, card_h);
            Color32 bg = unlocked ? (hover ? RGB32(42, 62, 88) : COL_BG_PANEL) : RGB32(20, 25, 34);
            Color32 brd = unlocked ? (hover ? COL_TEXT_GOLD : COL_BORDER) : RGB32(40, 48, 60);

            Gfx_DrawRoundedRect(card_x, card_y, card_w, card_h, 8, bg, brd);

            char num_str[32];
            snprintf(num_str, sizeof(num_str), "MAN %d", lvl->level_number);
            Gfx_DrawText(card_x + 14, card_y + 14, num_str, 2, unlocked ? COL_WIRE_HIGH : COL_TEXT_MUTED);

            /* Ve so sao (1-3 sao) */
            int st = game->progress.stars[idx];
            char st_str[16];
            snprintf(st_str, sizeof(st_str), "%s %s %s",
                     (st >= 1) ? "[*]" : "[ ]",
                     (st >= 2) ? "[*]" : "[ ]",
                     (st >= 3) ? "[*]" : "[ ]");
            Gfx_DrawText(card_x + 112, card_y + 16, st_str, 1, st > 0 ? COL_TEXT_GOLD : COL_TEXT_MUTED);

            /* Ten rut gon cua man */
            const char *short_title = strchr(lvl->title, ':');
            short_title = short_title ? (short_title + 2) : lvl->title;
            Gfx_DrawText(card_x + 14, card_y + 50, short_title, 1, unlocked ? COL_TEXT_WHITE : COL_TEXT_MUTED);

            char io_info[64];
            snprintf(io_info, sizeof(io_info), "Input: %d  |  Output: %d", lvl->num_inputs, lvl->num_outputs);
            Gfx_DrawText(card_x + 14, card_y + 82, io_info, 1, COL_TEXT_MUTED);

            if (unlocked) {
                if (game->progress.best_time_sec[idx] > 0) {
                    char tbuf[48];
                    snprintf(tbuf, sizeof(tbuf), "Ky luc: %ds (Toi uu: %d cong)",
                             game->progress.best_time_sec[idx], lvl->optimal_gate_count);
                    Gfx_DrawText(card_x + 14, card_y + 114, tbuf, 1, COL_ACCENT_GREEN);
                } else {
                    Gfx_DrawText(card_x + 14, card_y + 114, "Click de bat dau giai ->", 1, COL_TEXT_GOLD);
                }

                if (hover && input->left_pressed) {
                    Game_LoadLevel(game, idx);
                    return;
                }
            } else {
                Gfx_DrawText(card_x + 14, card_y + 114, "[CHUA MO KHOA]", 1, COL_ACCENT_RED);
            }
        }
    }
}

static void CheckAndTriggerVictory(GameContext *game) {
    const LevelDef *lvl = Levels_Get(game->current_level_idx);
    Levels_VerifyCircuit(lvl, &game->circuit, &game->verify_res);

    if (game->verify_res.all_passed) {
        game->level_completed_modal = true;
        int used_gates = Circuit_CountUsedGates(&game->circuit);

        /* Tinh so sao (1 -> 3 sao):
           - 3 sao: Hoan thanh < 5 phut (300s) VA so cong <= optimal_gate_count + 1
           - 2 sao: Hoan thanh < 5 phut (300s)
           - 1 sao: Hoan thanh > 5 phut */
        int stars = 1;
        if (game->elapsed_seconds <= 300) {
            stars = 2;
            if (used_gates <= lvl->optimal_gate_count + 1) {
                stars = 3;
            }
        }
        game->earned_stars = stars;

        int idx = game->current_level_idx;
        if (stars > game->progress.stars[idx]) {
            game->progress.stars[idx] = stars;
        }
        if (game->progress.best_time_sec[idx] == 0 || game->elapsed_seconds < game->progress.best_time_sec[idx]) {
            game->progress.best_time_sec[idx] = (game->elapsed_seconds > 0) ? game->elapsed_seconds : 1;
        }
        if (lvl->level_number >= game->progress.unlocked_level && lvl->level_number < TOTAL_LEVELS) {
            game->progress.unlocked_level = lvl->level_number + 1;
        }
        Progress_Save(&game->progress, SAVE_FILE_PATH);
        Game_SetStatus(game, "CHUC MUNG! Mach cua ban da vuot qua 100% Bang Chan Tri!");
    } else {
        char msg[112];
        snprintf(msg, sizeof(msg), "Mach chua chinh xac (%d/%d hang dung). Hay xem Bang Chan Tri ben phai!",
                 game->verify_res.passed_rows, game->verify_res.total_rows);
        Game_SetStatus(game, msg);
    }
}

static void DrawToolbox(GameContext *game, const InputState *input) {
    int tb_x = 20;
    int tb_y = 636;
    int tb_w = SCREEN_WIDTH - 40;
    int tb_h = 108;

    Gfx_DrawRoundedRect(tb_x, tb_y, tb_w, tb_h, 8, COL_BG_PANEL, COL_BORDER);
    Gfx_DrawText(tb_x + 14, tb_y + 8, "KHO LINH KIEN CONG LOGIC (CLICK DE THEM CONG VAO MACH):", 1, COL_TEXT_GOLD);

    const ComponentType gate_types[7] = {
        COMP_NOT, COMP_AND, COMP_OR, COMP_XOR, COMP_NAND, COMP_NOR, COMP_XNOR
    };

    const LevelDef *lvl = (game->screen == STATE_PLAYING) ? Levels_Get(game->current_level_idx) : NULL;

    for (int i = 0; i < 7; i++) {
        ComponentType t = gate_types[i];
        int gx = tb_x + 16 + i * 172;
        int gy = tb_y + 30;
        int gw = 158;
        int gh = 68;

        int used = Circuit_CountGateByType(&game->circuit, t);
        int allowed = lvl ? lvl->allowed_gates[t] : -1;
        bool can_use = (allowed == -1) || (allowed > 0 && used < allowed);

        bool hover = Gfx_PointInRect(input->mouse_x, input->mouse_y, gx, gy, gw, gh);
        Color32 bg = can_use ? (hover ? RGB32(45, 62, 86) : COL_BG_CARD) : RGB32(20, 26, 36);
        Color32 brd = can_use ? (hover ? COL_TEXT_GOLD : Renderer_GetGateColor(t)) : RGB32(45, 52, 65);

        Gfx_DrawRoundedRect(gx, gy, gw, gh, 6, bg, brd);
        Gfx_DrawRect(gx + 3, gy + 3, gw - 6, 8, can_use ? Renderer_GetGateColor(t) : RGB32(55, 65, 78));

        Gfx_DrawText(gx + 10, gy + 16, Circuit_GetTypeName(t), 2, can_use ? COL_TEXT_WHITE : COL_TEXT_MUTED);
        Gfx_DrawText(gx + 10, gy + 42, Circuit_GetTypeFormula(t), 1, COL_TEXT_MUTED);

        if (lvl) {
            char qbuf[32];
            if (allowed == 0) {
                snprintf(qbuf, sizeof(qbuf), "Cam dung");
            } else {
                snprintf(qbuf, sizeof(qbuf), "Con: %d/%d", allowed - used, allowed);
            }
            Gfx_DrawText(gx + 86, gy + 18, qbuf, 1, can_use ? COL_WIRE_HIGH : COL_ACCENT_RED);
        } else {
            Gfx_DrawText(gx + 92, gy + 18, "Vo han", 1, COL_WIRE_HIGH);
        }

        if (hover && input->left_pressed && !game->level_completed_modal) {
            Interaction_SpawnGateFromToolbox(game, t);
        }
    }
}

static void DrawPlayingOrSandbox(GameContext *game, const InputState *input, double now) {
    bool is_sandbox = (game->screen == STATE_SANDBOX);
    const LevelDef *lvl = is_sandbox ? NULL : Levels_Get(game->current_level_idx);

    if (!is_sandbox && !game->level_completed_modal) {
        game->elapsed_seconds = (int)(now - game->level_start_time);
    }

    /* 1. Thanh Header phia tren */
    Gfx_DrawRoundedRect(20, 12, SCREEN_WIDTH - 40, 88, 8, COL_BG_PANEL, COL_BORDER);

    if (DrawButton(32, 24, 120, 34, is_sandbox ? "< MENU" : "< CHON MAN", COL_BG_CARD, input)) {
        game->screen = is_sandbox ? STATE_MAIN_MENU : STATE_LEVEL_SELECT;
        return;
    }

    if (is_sandbox) {
        Gfx_DrawText(168, 22, "PHONG THI NGHIEM TU DO (SANDBOX MODE)", 3, COL_WIRE_HIGH);
        Gfx_DrawText(168, 56, "Click cong tac A, B, C de doi 0/1. Keo tha cong logic, noi day va quan sat tin hieu!", 1, COL_TEXT_WHITE);
    } else {
        Gfx_DrawText(168, 20, lvl->title, 3, COL_TEXT_GOLD);
        Gfx_DrawText(168, 54, lvl->story_desc, 1, COL_TEXT_WHITE);

        /* Dong ho dem thoi gian (< 5 phut) */
        int mins = game->elapsed_seconds / 60;
        int secs = game->elapsed_seconds % 60;
        char tstr[48];
        snprintf(tstr, sizeof(tstr), "Thoi gian: %02d:%02d / 05:00", mins, secs);
        Color32 tcol = (game->elapsed_seconds <= 300) ? COL_WIRE_HIGH : COL_ACCENT_WARN;
        Gfx_DrawText(730, 24, tstr, 2, tcol);
    }

    /* Cac nut chuc nang goc phai Header */
    if (!is_sandbox) {
        if (DrawButton(945, 22, 95, 36, "GOI Y (H)", RGB32(95, 72, 28), input) || input->key_h_pressed) {
            game->show_hint = !game->show_hint;
        }
    }
    if (DrawButton(1048, 22, 95, 36, "XOA MACH", RGB32(95, 36, 46), input) || input->key_r_pressed) {
        Circuit_ClearUserItems(&game->circuit);
        if (!is_sandbox) {
            Levels_VerifyCircuit(lvl, &game->circuit, &game->verify_res);
        }
        Game_SetStatus(game, "Da xoa tat ca linh kien va day noi de lam lai.");
    }
    if (!is_sandbox) {
        if (DrawButton(1150, 22, 98, 36, "CHAM DIEM", RGB32(24, 130, 82), input) || input->key_space_pressed) {
            CheckAndTriggerVictory(game);
        }
    }

    /* 2. Khu vuc Bang Mach PCB */
    int board_w = is_sandbox ? (SCREEN_WIDTH - 40) : 880;
    Renderer_DrawBoardGrid(20, 108, board_w, 520);

    /* Neu bat Goi Y (Hint), hien thi hop goi y tren bang mach */
    if (!is_sandbox && game->show_hint) {
        Gfx_DrawRoundedRect(35, 118, 850, 38, 6, RGB32(60, 48, 20), COL_TEXT_GOLD);
        char hbuf[256];
        snprintf(hbuf, sizeof(hbuf), "GOI Y GIAI: %s", lvl->hint);
        Gfx_DrawText(48, 129, hbuf, 1, COL_TEXT_GOLD);
    }

    /* Cap nhat tuong tac chuot neu khong mo hop thoai Chien thang */
    if (!game->level_completed_modal) {
        Interaction_UpdateBoard(game, input);
    }

    /* Ve toan bo Mach Dien & Day Bezier */
    Renderer_DrawCircuit(&game->circuit, &game->editor, input, now);

    /* Thanh trang thai huong dan o day Bang Mach */
    Gfx_DrawText(34, 604, game->status_msg, 1, COL_TEXT_MUTED);

    /* 3. Bang Chan Tri (Truth Table) ben phai (chi hien thi trong che do Playing) */
    if (!is_sandbox) {
        Renderer_DrawTruthTable(lvl, &game->verify_res, 910, 108, 350, 520);
    }

    /* 4. Thanh cong cu Toolbox o duoi cung */
    DrawToolbox(game, input);

    /* 5. Hop thoai Chien Thang (Victory Modal) khi giai xong man */
    if (!is_sandbox && game->level_completed_modal) {
        int mx = SCREEN_WIDTH / 2 - 260;
        int my = SCREEN_HEIGHT / 2 - 165;
        int mw = 520;
        int mh = 310;

        Gfx_DrawRoundedRect(mx, my, mw, mh, 12, COL_BG_PANEL, COL_WIRE_HIGH);
        Gfx_DrawTextCentered(mx + mw / 2, my + 38, "HOAN THANH MAN CHOI!", 3, COL_WIRE_HIGH);
        Gfx_DrawTextCentered(mx + mw / 2, my + 74, lvl->title, 2, COL_TEXT_WHITE);

        char star_line[64];
        snprintf(star_line, sizeof(star_line), "DANH GIA:   %s   %s   %s",
                 (game->earned_stars >= 1) ? "[* SAO *]" : "[ ]",
                 (game->earned_stars >= 2) ? "[* SAO *]" : "[ ]",
                 (game->earned_stars >= 3) ? "[* SAO *]" : "[ ]");
        Gfx_DrawTextCentered(mx + mw / 2, my + 116, star_line, 2, COL_TEXT_GOLD);

        char stat_line[128];
        int used = Circuit_CountUsedGates(&game->circuit);
        snprintf(stat_line, sizeof(stat_line),
                 "Thoi gian giai: %d giay   |   So cong su dung: %d (Toi uu: %d)",
                 game->elapsed_seconds, used, lvl->optimal_gate_count);
        Gfx_DrawTextCentered(mx + mw / 2, my + 158, stat_line, 1, COL_TEXT_MUTED);

        if (DrawButton(mx + 45, my + 220, 195, 48, "CHOI LAI MAN NAY", COL_BG_CARD, input)) {
            game->level_completed_modal = false;
        }

        if (game->current_level_idx + 1 < TOTAL_LEVELS) {
            if (DrawButton(mx + 275, my + 220, 205, 48, "MAN TIEP THEO ->", RGB32(24, 130, 82), input)) {
                Game_LoadLevel(game, game->current_level_idx + 1);
            }
        } else {
            if (DrawButton(mx + 275, my + 220, 205, 48, "VE CHON MAN", RGB32(24, 130, 82), input)) {
                game->screen = STATE_LEVEL_SELECT;
            }
        }
    }
}

void UI_UpdateAndDraw(GameContext *game, const InputState *input, double now) {
    if (!game || !input) return;

    switch (game->screen) {
        case STATE_MAIN_MENU:
            DrawMainMenu(game, input, now);
            break;
        case STATE_LEVEL_SELECT:
            DrawLevelSelect(game, input);
            break;
        case STATE_PLAYING:
        case STATE_SANDBOX:
            DrawPlayingOrSandbox(game, input, now);
            break;
    }
}
