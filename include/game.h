#ifndef GAME_H
#define GAME_H

#include "gfx.h"
#include "circuit.h"
#include "levels.h"

/* Trang thai man hinh cua Game */
typedef enum {
    STATE_MAIN_MENU = 0,
    STATE_LEVEL_SELECT,
    STATE_PLAYING,
    STATE_SANDBOX
} GameScreenState;

/* Thong tin keo tha va noi day (Thanh vien 4 - Interaction) */
typedef struct {
    int dragging_comp_id;       /* ID linh kien dang keo (-1 neu khong co) */
    float drag_offset_x;
    float drag_offset_y;

    bool is_wiring;             /* Dang keo day tu 1 chan Output */
    int wire_from_comp_id;

    int hovered_comp_id;
    int hovered_pin_comp_id;
    int hovered_pin_index;      /* -1 = Output pin, 0..1 = Input pin */
} EditorState;

/* Cau truc trang thai tong the cua Game */
typedef struct {
    GameScreenState screen;
    int current_level_idx;      /* 0 -> TOTAL_LEVELS - 1 */
    Circuit circuit;
    VerificationResult verify_res;
    PlayerProgress progress;
    EditorState editor;

    bool show_hint;
    bool level_completed_modal;
    int earned_stars;
    double level_start_time;
    int elapsed_seconds;
    char status_msg[128];
    double status_msg_timer;
} GameContext;

/* --- API Thanh vien 3: Graphics & Component Renderer (src/renderer.c) --- */
void Renderer_GetPinPos(const Component *comp, bool is_output, int pin_index, float *out_x, float *out_y);
void Renderer_DrawBoardGrid(int x, int y, int w, int h);
void Renderer_DrawCircuit(const Circuit *circuit, const EditorState *editor, const InputState *input, double anim_time);
void Renderer_DrawTruthTable(const LevelDef *level, const VerificationResult *res, int x, int y, int w, int h);
Color32 Renderer_GetGateColor(ComponentType type);

/* --- API Thanh vien 4: User Interaction & Editor Controller (src/interaction.c) --- */
void Interaction_Reset(EditorState *editor);
void Interaction_UpdateBoard(GameContext *game, const InputState *input);
bool Interaction_SpawnGateFromToolbox(GameContext *game, ComponentType type);

/* --- API Thanh vien 5: UI Screens, Menu & Game Flow (src/ui.c) --- */
void Game_Init(GameContext *game);
void Game_LoadLevel(GameContext *game, int level_idx);
void Game_StartSandbox(GameContext *game);
void Game_SetStatus(GameContext *game, const char *msg);
void UI_UpdateAndDraw(GameContext *game, const InputState *input, double now);

#endif /* GAME_H */
