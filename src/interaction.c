/*
 * ============================================================================
 * MODULE 4: USER INTERACTION & EDITOR CONTROLLER (interaction.c)
 * Phu trach: Thanh vien 4
 * Noi dung:
 *   - Bat va xu ly su kien Chuot tren bang mach (Keo tha, Noi day, Xoa cong)
 *   - Ho tro ca 2 kieu noi day: Keo-Tha (Drag) hoac Click-Chon (Trackpad friendly)
 *   - Bat/tat cong tac Input (0/1) va Tu dong cap nhat Bang Chan Tri
 * ============================================================================
 */

#include "../include/game.h"
#include <math.h>
#include <stdio.h>

#define BOARD_MIN_X 30.0f
#define BOARD_MAX_X 880.0f
#define BOARD_MIN_Y 125.0f
#define BOARD_MAX_Y 625.0f

void Interaction_Reset(EditorState *editor) {
    if (!editor) return;
    editor->dragging_comp_id = -1;
    editor->drag_offset_x = 0.0f;
    editor->drag_offset_y = 0.0f;
    editor->is_wiring = false;
    editor->wire_from_comp_id = -1;
    editor->hovered_comp_id = -1;
    editor->hovered_pin_comp_id = -1;
    editor->hovered_pin_index = -2;
}

static float DistSq(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    return dx * dx + dy * dy;
}

bool Interaction_SpawnGateFromToolbox(GameContext *game, ComponentType type) {
    if (!game) return false;

    if (game->screen == STATE_PLAYING) {
        const LevelDef *lvl = Levels_Get(game->current_level_idx);
        int max_allowed = lvl->allowed_gates[type];
        int used = Circuit_CountGateByType(&game->circuit, type);
        if (max_allowed == 0) {
            Game_SetStatus(game, "Man choi nay khong cho phep su dung cong nay!");
            return false;
        }
        if (max_allowed > 0 && used >= max_allowed) {
            Game_SetStatus(game, "Da dat gioi han so luong cong loai nay trong man!");
            return false;
        }
    }

    /* Tim vi tri trung tam bang mach de dat cong moi, tranh de len cong cu */
    int total_user = Circuit_CountUsedGates(&game->circuit);
    float spawn_x = 340.0f + (total_user % 3) * 115.0f;
    float spawn_y = 220.0f + ((total_user / 3) % 3) * 95.0f;

    int cid = Circuit_AddComponent(&game->circuit, type, spawn_x, spawn_y, false, NULL);
    if (cid >= 0) {
        if (game->screen == STATE_PLAYING) {
            Levels_VerifyCircuit(Levels_Get(game->current_level_idx), &game->circuit, &game->verify_res);
        }
        char msg[96];
        snprintf(msg, sizeof(msg), "Da them cong %s! Keo tha de di chuyen, Chuot phai de xoa.", Circuit_GetTypeName(type));
        Game_SetStatus(game, msg);
        return true;
    }
    return false;
}

void Interaction_UpdateBoard(GameContext *game, const InputState *input) {
    if (!game || !input) return;
    Circuit *circuit = &game->circuit;
    EditorState *ed = &game->editor;

    float mx = (float)input->mouse_x;
    float my = (float)input->mouse_y;

    /* 1. Kiem tra Hover tren cac chan Pin (uu tien cao nhat) */
    ed->hovered_pin_comp_id = -1;
    ed->hovered_pin_index = -2;
    const float pin_hit_radius_sq = 15.0f * 15.0f;

    for (int i = MAX_COMPONENTS - 1; i >= 0; i--) {
        const Component *c = &circuit->components[i];
        if (!c->active) continue;

        /* Kiem tra chan Output (-1) */
        if (c->num_outputs > 0) {
            float px, py;
            Renderer_GetPinPos(c, true, 0, &px, &py);
            if (DistSq(mx, my, px, py) <= pin_hit_radius_sq) {
                ed->hovered_pin_comp_id = i;
                ed->hovered_pin_index = -1;
                break;
            }
        }

        /* Kiem tra cac chan Input (0, 1) */
        for (int p = 0; p < c->num_inputs; p++) {
            float px, py;
            Renderer_GetPinPos(c, false, p, &px, &py);
            if (DistSq(mx, my, px, py) <= pin_hit_radius_sq) {
                ed->hovered_pin_comp_id = i;
                ed->hovered_pin_index = p;
                break;
            }
        }
        if (ed->hovered_pin_comp_id != -1) break;
    }

    /* 2. Kiem tra Hover tren than Linh kien */
    ed->hovered_comp_id = -1;
    if (ed->hovered_pin_comp_id == -1) {
        for (int i = MAX_COMPONENTS - 1; i >= 0; i--) {
            const Component *c = &circuit->components[i];
            if (!c->active) continue;
            if (mx >= c->x && mx <= c->x + c->width &&
                my >= c->y && my <= c->y + c->height) {
                ed->hovered_comp_id = i;
                break;
            }
        }
    }

    /* 3. Xu ly khi dang keo linh kien (Dragging Gate) */
    if (ed->dragging_comp_id != -1) {
        Component *c = &circuit->components[ed->dragging_comp_id];
        if (c->active && input->left_down) {
            float nx = mx - ed->drag_offset_x;
            float ny = my - ed->drag_offset_y;
            if (nx < BOARD_MIN_X) nx = BOARD_MIN_X;
            if (nx + c->width > BOARD_MAX_X) nx = BOARD_MAX_X - c->width;
            if (ny < BOARD_MIN_Y) ny = BOARD_MIN_Y;
            if (ny + c->height > BOARD_MAX_Y) ny = BOARD_MAX_Y - c->height;
            c->x = nx;
            c->y = ny;
        } else {
            ed->dragging_comp_id = -1;
        }
        return;
    }

    /* 4. Xu ly Chuot Trai (Left Click) */
    if (input->left_pressed) {
        /* Truong hop A: Click vao 1 chan Pin */
        if (ed->hovered_pin_comp_id != -1) {
            if (ed->hovered_pin_index == -1) {
                /* Click chan Output -> Bat dau keo day */
                ed->is_wiring = true;
                ed->wire_from_comp_id = ed->hovered_pin_comp_id;
                Game_SetStatus(game, "Dang noi day: Hay click hoac tha chuot vao chan Input (ben trai cong dich)!");
                return;
            } else if (ed->is_wiring && ed->hovered_pin_index >= 0) {
                /* Dang keo day ma click vao chan Input -> Hoan tat noi day! */
                if (Circuit_AddWire(circuit, ed->wire_from_comp_id, ed->hovered_pin_comp_id, ed->hovered_pin_index)) {
                    if (game->screen == STATE_PLAYING) {
                        Levels_VerifyCircuit(Levels_Get(game->current_level_idx), circuit, &game->verify_res);
                    }
                    Game_SetStatus(game, "Da noi day thanh cong!");
                }
                ed->is_wiring = false;
                ed->wire_from_comp_id = -1;
                return;
            }
        }

        /* Neu dang noi day ma click ra ngoai khoang trong -> Huy noi day */
        if (ed->is_wiring) {
            ed->is_wiring = false;
            ed->wire_from_comp_id = -1;
            Game_SetStatus(game, "Da huy thao tac noi day.");
            return;
        }

        /* Truong hop B: Click vao than Linh kien */
        if (ed->hovered_comp_id != -1) {
            Component *c = &circuit->components[ed->hovered_comp_id];
            if (c->type == COMP_INPUT) {
                /* Bat/tat cong tac Input 0 <-> 1 */
                c->output_value = c->output_value ? 0 : 1;
                Circuit_Simulate(circuit);
                char msg[80];
                snprintf(msg, sizeof(msg), "Da chuyen cong tac %s sang muc logic %d.", c->label, c->output_value);
                Game_SetStatus(game, msg);
            } else if (!c->locked || game->screen == STATE_SANDBOX) {
                /* Bat dau keo tha linh kien */
                ed->dragging_comp_id = c->id;
                ed->drag_offset_x = mx - c->x;
                ed->drag_offset_y = my - c->y;
            }
        }
    }

    /* 5. Ho tro Keo-Tha day (Drag from Output pin -> Release on Input pin) */
    if (input->left_released && ed->is_wiring) {
        if (ed->hovered_pin_comp_id != -1 && ed->hovered_pin_index >= 0) {
            if (Circuit_AddWire(circuit, ed->wire_from_comp_id, ed->hovered_pin_comp_id, ed->hovered_pin_index)) {
                if (game->screen == STATE_PLAYING) {
                    Levels_VerifyCircuit(Levels_Get(game->current_level_idx), circuit, &game->verify_res);
                }
                Game_SetStatus(game, "Da noi day thanh cong!");
            }
            ed->is_wiring = false;
            ed->wire_from_comp_id = -1;
        }
    }

    /* 6. Xu ly Chuot Phai (Right Click): Xoa day hoac Xoa linh kien */
    if (input->right_pressed) {
        if (ed->is_wiring) {
            ed->is_wiring = false;
            ed->wire_from_comp_id = -1;
            return;
        }

        /* Chuot phai vao chan Input -> Thao day noi tai chan do */
        if (ed->hovered_pin_comp_id != -1 && ed->hovered_pin_index >= 0) {
            if (Circuit_RemoveWireAtInput(circuit, ed->hovered_pin_comp_id, ed->hovered_pin_index)) {
                if (game->screen == STATE_PLAYING) {
                    Levels_VerifyCircuit(Levels_Get(game->current_level_idx), circuit, &game->verify_res);
                }
                Game_SetStatus(game, "Da thao day noi tai chan Input.");
            }
            return;
        }

        /* Chuot phai vao Cong logic -> Xoa cong do */
        if (ed->hovered_comp_id != -1) {
            Component *c = &circuit->components[ed->hovered_comp_id];
            if (!c->locked) {
                Circuit_RemoveComponent(circuit, ed->hovered_comp_id);
                if (game->screen == STATE_PLAYING) {
                    Levels_VerifyCircuit(Levels_Get(game->current_level_idx), circuit, &game->verify_res);
                }
                Game_SetStatus(game, "Da xoa linh kien khoi bang mach.");
            } else {
                Game_SetStatus(game, "Khong the xoa ngo INPUT/OUTPUT co dinh cua de bai!");
            }
        }
    }
}
