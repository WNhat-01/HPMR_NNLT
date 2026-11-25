/*
 * ============================================================================
 * ENTRY POINT (main.c)
 * Project: CIRCUIT PUZZLE: LOGIC ARCHITECT (Ngon ngu C - Khoa Dien HCMUT)
 * Bien dich:
 *   gcc src/main.c src/circuit.c src/levels.c src/renderer.c src/interaction.c src/ui.c src/gfx.c -Iinclude -lgdi32 -lm -o circuit_puzzle.exe
 * ============================================================================
 */

#include "../include/game.h"
#include <stdio.h>
#include <string.h>

extern void Renderer_InitGateImages(void);
extern void Renderer_FreeGateImages(void);

/* Chay Unit Test tu dong kiem tra Engine mo phong mach (dung khi chay: circuit_puzzle.exe --test) */
static int RunSelfTest(void) {
    printf("=== KIEM TRA TU DONG CIRCUIT SIMULATION ENGINE (15 LEVELS) ===\n");
    GameContext ctx;
    Game_Init(&ctx);

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        const LevelDef *lvl = Levels_Get(i);
        Levels_SetupCircuit(lvl, &ctx.circuit);
        Levels_VerifyCircuit(lvl, &ctx.circuit, &ctx.verify_res);
        printf("[OK] Loaded %s | Inputs: %d | Outputs: %d | Truth Table Rows: %d\n",
               lvl->title, lvl->num_inputs, lvl->num_outputs, ctx.verify_res.total_rows);
    }

    /* Thu giai tu dong Man 1 (Cong NOT) va kiem tra Verify = PASS */
    const LevelDef *lvl1 = Levels_Get(0);
    Levels_SetupCircuit(lvl1, &ctx.circuit);
    int not_id = Circuit_AddComponent(&ctx.circuit, COMP_NOT, 350.0f, 300.0f, false, NULL);
    Circuit_AddWire(&ctx.circuit, ctx.circuit.input_comp_ids[0], not_id, 0);
    Circuit_AddWire(&ctx.circuit, not_id, ctx.circuit.output_comp_ids[0], 0);
    Levels_VerifyCircuit(lvl1, &ctx.circuit, &ctx.verify_res);

    if (!ctx.verify_res.all_passed) {
        printf("[FAIL] Level 1 verification test failed!\n");
        return 1;
    }
    printf("[PASS] Level 1 Auto-Solve Verification passed (%d/%d rows)!\n",
           ctx.verify_res.passed_rows, ctx.verify_res.total_rows);
    printf("=== TAT CA KIEM TRA HOAN TAT THANH CONG! ===\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        return RunSelfTest();
    }

    if (!Gfx_Init("Circuit Puzzle: Logic Architect - Project C Khoa Dien HCMUT", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        fprintf(stderr, "Loi: Khong the khoi tao cua so do hoa 2D!\n");
        return 1;
    }

    GameContext game;
    Game_Init(&game);

    InputState input;
    while (Gfx_PollEvents(&input)) {
        double now = Gfx_GetTimeSeconds();

        if (input.key_esc_pressed) {
            if (game.screen == STATE_PLAYING) {
                game.screen = STATE_LEVEL_SELECT;
            } else if (game.screen == STATE_SANDBOX || game.screen == STATE_LEVEL_SELECT) {
                game.screen = STATE_MAIN_MENU;
            } else {
                break;
            }
        }

        Gfx_BeginFrame(COL_BG_DARK);
        UI_UpdateAndDraw(&game, &input, now);
        Gfx_EndFrame();
    }

    Gfx_Shutdown();
    return 0;
}
