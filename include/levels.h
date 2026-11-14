#ifndef LEVELS_H
#define LEVELS_H

#include "circuit.h"

#define TOTAL_LEVELS 15
#define MAX_TRUTH_ROWS 16   /* 2^4 = 16 to hop dau vao toi da */

/* Cau truc du lieu cua 1 man choi (Level) */
typedef struct {
    int level_number;               /* So thu tu man: 1 -> 15 */
    int chapter;                    /* Chuong: 1 (Co ban), 2 (Trung cap), 3 (Ky su KTS) */
    const char *title;              /* Ten man choi */
    const char *story_desc;         /* Mo ta bai toan thuc te */
    const char *hint;               /* Goi y giai (khi nguoi choi bam nut Hint) */

    int num_inputs;                 /* So cong tac dau vao (1 -> 4) */
    const char *input_labels[MAX_INPUTS];

    int num_outputs;                /* So den LED dau ra (1 -> 2) */
    const char *output_labels[MAX_OUTPUTS];

    /* So luong toi da moi loai cong duoc phep dung trong man nay (-1 = khong gioi han, 0 = cam dung) */
    int allowed_gates[COMP_TYPE_COUNT];

    /* So cong toi uu de dat 3 sao */
    int optimal_gate_count;

    /* Bang chan tri muc tieu: expected_outputs[row][out_idx] voi row = 0 .. (2^num_inputs - 1) */
    int expected_outputs[MAX_TRUTH_ROWS][MAX_OUTPUTS];
} LevelDef;

/* Ket qua kiem tra bang chan tri cua man choi hien tai */
typedef struct {
    int total_rows;
    int passed_rows;
    bool row_passed[MAX_TRUTH_ROWS];
    int actual_outputs[MAX_TRUTH_ROWS][MAX_OUTPUTS];
    bool all_passed;
} VerificationResult;

/* Tien do luu tru cua nguoi choi (Save/Load file) */
typedef struct {
    int unlocked_level;             /* Man cao nhat da mo khoa (1 -> TOTAL_LEVELS) */
    int stars[TOTAL_LEVELS];        /* So sao dat duoc o moi man (0 -> 3) */
    int best_time_sec[TOTAL_LEVELS];/* Thoi gian nhanh nhat (giay) */
} PlayerProgress;

/* API cho Thanh vien 2 (Level Design & Verification / Save-Load) */
const LevelDef* Levels_Get(int level_index); /* level_index: 0 -> TOTAL_LEVELS - 1 */
void Levels_SetupCircuit(const LevelDef *level, Circuit *circuit);
void Levels_VerifyCircuit(const LevelDef *level, Circuit *circuit, VerificationResult *result);

/* Luu va tai tien do tu file progress.dat */
void Progress_InitDefault(PlayerProgress *prog);
bool Progress_Load(PlayerProgress *prog, const char *filepath);
bool Progress_Save(const PlayerProgress *prog, const char *filepath);

#endif /* LEVELS_H */
