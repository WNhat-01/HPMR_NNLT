/*
 * ============================================================================
 * MODULE 2: LEVEL DESIGN, VERIFICATION & FILE I/O (levels.c)
 * Phu trach: Thanh vien 2
 * Noi dung:
 *   - Thiet ke 15 man choi (3 Chuong) tu de den kho (< 5 phut / man)
 *   - Thuat toan kiem tra tu dong Bang Chan Tri (Truth Table Verification)
 *   - Luu / Doc tien do nguoi choi ra file nhi phan (progress.dat)
 * ============================================================================
 */

#include "../include/levels.h"
#include <stdio.h>
#include <string.h>

static const LevelDef g_levels[TOTAL_LEVELS] = {
    /* ==================== CHUONG 1: CO BAN (MAN 1 - 5) ==================== */
    {
        .level_number = 1,
        .chapter = 1,
        .title = "Man 1: Den Bao Khan Cap (Cong NOT)",
        .story_desc = "Den khan cap Y phai TU DONG SANG (1) khi nguon dien chinh A bi cup (0), va TAT (0) khi co dien (1).",
        .hint = "Keo 1 cong NOT tu thanh cong cu xuong va noi A -> NOT -> Y.",
        .num_inputs = 1,
        .input_labels = {"A"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_NOT] = 2 },
        .optimal_gate_count = 1,
        .expected_outputs = { {1}, {0} }
    },
    {
        .level_number = 2,
        .chapter = 1,
        .title = "Man 2: Khoa An Toan Kep (Cong AND)",
        .story_desc = "May dap cong nghiep Y chi duoc phep hoat dong khi cong nhan nhan DONG THOI ca 2 nut A va B.",
        .hint = "Cong AND chi xuat ra 1 khi tat ca ngo vao deu bang 1.",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_AND] = 2, [COMP_NOT] = 1 },
        .optimal_gate_count = 1,
        .expected_outputs = { {0}, {0}, {0}, {1} }
    },
    {
        .level_number = 3,
        .chapter = 1,
        .title = "Man 3: Chuong Bao Chay 2 Tang (Cong OR)",
        .story_desc = "Coi bao dong Y phai keu (1) khi cam bien khoi Tang 1 (A) HOAC Tang 2 (B) phat hien khoi.",
        .hint = "Su dung 1 cong OR noi voi A va B.",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_OR] = 2, [COMP_AND] = 1 },
        .optimal_gate_count = 1,
        .expected_outputs = { {0}, {1}, {1}, {1} }
    },
    {
        .level_number = 4,
        .chapter = 1,
        .title = "Man 4: Cong Tac Cau Thang (Cong XOR)",
        .story_desc = "Den cau thang Y sang (1) khi 2 cong tac A va B o trang thai KHAC NHAU, va tat khi chung cung trang thai.",
        .hint = "Cong XOR xuat 1 khi hai ngo vao A, B khac nhau (0-1 hoac 1-0).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_XOR] = 2, [COMP_OR] = 1 },
        .optimal_gate_count = 1,
        .expected_outputs = { {0}, {1}, {1}, {0} }
    },
    {
        .level_number = 5,
        .chapter = 1,
        .title = "Man 5: Bao Ve Qua Nhiet Dong Co",
        .story_desc = "Dong co Y chay (1) khi Cong tac nguon A = 1 NHUNG Cam bien qua nhiet B phai bang 0 (chua nong).",
        .hint = "Dao nguoc tin hieu B bang cong NOT, sau do dua vao cong AND cung voi A: Y = A . (~B).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_AND] = 2, [COMP_NOT] = 2, [COMP_OR] = 1 },
        .optimal_gate_count = 2,
        .expected_outputs = { {0}, {0}, {1}, {0} }
    },

    /* ==================== CHUONG 2: TRUNG CAP (MAN 6 - 10) ==================== */
    {
        .level_number = 6,
        .chapter = 2,
        .title = "Man 6: Che Tao Cong NAND",
        .story_desc = "Kho linh kien het cong NAND! Hay ghep cac cong co ban de tao mach chi TAT (0) khi ca A va B deu bang 1.",
        .hint = "Cong NAND chinh la cong AND mac noi tiep voi cong NOT o ngo ra: Y = ~(A . B).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_AND] = 2, [COMP_NOT] = 2 },
        .optimal_gate_count = 2,
        .expected_outputs = { {1}, {1}, {1}, {0} }
    },
    {
        .level_number = 7,
        .chapter = 2,
        .title = "Man 7: Dinh Ly De Morgan (Tao OR tu NAND)",
        .story_desc = "Khong co cong OR! Hay dung Dinh ly De Morgan de tao ham Y = A + B chi tu cong NOT va NAND.",
        .hint = "Theo De Morgan: A + B = ~(~A . ~B). Dung 2 cong NOT dao A, B roi dua vao 1 cong NAND.",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_NOT] = 3, [COMP_NAND] = 2 },
        .optimal_gate_count = 3,
        .expected_outputs = { {0}, {1}, {1}, {1} }
    },
    {
        .level_number = 8,
        .chapter = 2,
        .title = "Man 8: Bo So Sanh Bang (Equality Detector)",
        .story_desc = "Den Y sang (1) khi hai tin hieu A va B GIONG HET NHAU (cung bang 0 hoac cung bang 1).",
        .hint = "Ket hop cong XOR va cong NOT: Y = ~(A ^ B).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_XOR] = 2, [COMP_NOT] = 2, [COMP_AND] = 2 },
        .optimal_gate_count = 2,
        .expected_outputs = { {1}, {0}, {0}, {1} }
    },
    {
        .level_number = 9,
        .chapter = 2,
        .title = "Man 9: He Thong Tuoi Cay Thong Minh (3 Input)",
        .story_desc = "May bom Y bat (1) khi Dat kho (A=1) VA (Dang la ban dem B=1 HOAC Che do khan cap C=1).",
        .hint = "Bieu thuc Boole: Y = A . (B + C). Dung 1 cong OR cho (B, C) roi noi vao cong AND cung voi A.",
        .num_inputs = 3,
        .input_labels = {"A", "B", "C"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_AND] = 2, [COMP_OR] = 2, [COMP_NOT] = 2 },
        .optimal_gate_count = 2,
        /* Thu tu row (A,B,C): 000, 001, 010, 011, 100, 101, 110, 111 */
        .expected_outputs = { {0}, {0}, {0}, {0}, {0}, {1}, {1}, {1} }
    },
    {
        .level_number = 10,
        .chapter = 2,
        .title = "Man 10: Tu Che Cong XOR (Khong Dung Co San)",
        .story_desc = "Thu thach kinh dien: Tao ham XOR (Y = A ^ B) chi bang 3 cong: AND, OR va NAND!",
        .hint = "Ta co: A ^ B = (A + B) . ~(A . B). Lay ngo ra cua OR(A,B) va NAND(A,B) dua vao cong AND!",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_OR] = 2, [COMP_NAND] = 2, [COMP_AND] = 2, [COMP_NOT] = 2 },
        .optimal_gate_count = 3,
        .expected_outputs = { {0}, {1}, {1}, {0} }
    },

    /* ==================== CHUONG 3: KY SU KTS HCMUT (MAN 11 - 15) ==================== */
    {
        .level_number = 11,
        .chapter = 3,
        .title = "Man 11: Bo Cong Ban Phan (Half Adder)",
        .story_desc = "Thiet ke mach cong 2 bit nhi phan A + B voi 2 ngo ra: Tong SUM (S) va Bit nho CARRY (C).",
        .hint = "SUM = A ^ B (dung cong XOR) va CARRY = A . B (dung cong AND).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 2,
        .output_labels = {"SUM", "CAR"},
        .allowed_gates = { [COMP_XOR] = 2, [COMP_AND] = 2, [COMP_OR] = 2 },
        .optimal_gate_count = 2,
        /* {SUM, CARRY} cho 00, 01, 10, 11 */
        .expected_outputs = { {0, 0}, {1, 0}, {1, 0}, {0, 1} }
    },
    {
        .level_number = 12,
        .chapter = 3,
        .title = "Man 12: Bo So Sanh 1-Bit (Comparator)",
        .story_desc = "So sanh hai bit A va B: Ngo ra GT = 1 khi (A > B), va ngo ra EQ = 1 khi (A == B).",
        .hint = "GT = A . (~B) (1 cong NOT + 1 cong AND); EQ = ~(A ^ B) (1 cong XNOR).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 2,
        .output_labels = {"GT", "EQ"},
        .allowed_gates = { [COMP_AND] = 2, [COMP_NOT] = 2, [COMP_XNOR] = 2, [COMP_XOR] = 2 },
        .optimal_gate_count = 3,
        /* {GT, EQ} cho 00, 01, 10, 11 */
        .expected_outputs = { {0, 1}, {0, 0}, {1, 0}, {0, 1} }
    },
    {
        .level_number = 13,
        .chapter = 3,
        .title = "Man 13: Bo Chon Kenh MUX 2-sang-1",
        .story_desc = "Mach Multiplexer: Khi chan chon S = 0 thi ngo ra Y = A; khi S = 1 thi ngo ra Y = B.",
        .hint = "Cong thuc MUX: Y = (A . ~S) + (B . S). Dung 1 cong NOT, 2 cong AND va 1 cong OR.",
        .num_inputs = 3,
        .input_labels = {"A", "B", "S"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_NOT] = 2, [COMP_AND] = 3, [COMP_OR] = 2 },
        .optimal_gate_count = 4,
        /* Row (A, B, S):
           000->0, 001->0, 010->0, 011->1,
           100->1, 101->0, 110->1, 111->1 */
        .expected_outputs = { {0}, {0}, {0}, {1}, {1}, {0}, {1}, {1} }
    },
    {
        .level_number = 14,
        .chapter = 3,
        .title = "Man 14: Mach Bieu Quyet Da So (Majority Voter)",
        .story_desc = "3 cam bien A, B, C tren tau vu tru. Ngo ra Y = 1 neu co IT NHAT 2 tren 3 cam bien bang 1.",
        .hint = "Toi uu voi 4 cong: Y = (A . B) + (C . (A ^ B)). Dung 1 XOR, 2 AND va 1 OR!",
        .num_inputs = 3,
        .input_labels = {"A", "B", "C"},
        .num_outputs = 1,
        .output_labels = {"Y"},
        .allowed_gates = { [COMP_AND] = 4, [COMP_OR] = 3, [COMP_XOR] = 2 },
        .optimal_gate_count = 4,
        /* 000:0, 001:0, 010:0, 011:1, 100:0, 101:1, 110:1, 111:1 */
        .expected_outputs = { {0}, {0}, {0}, {1}, {0}, {1}, {1}, {1} }
    },
    {
        .level_number = 15,
        .chapter = 3,
        .title = "Man 15: Bo Cong Toan Phan (Full Adder - BOSS)",
        .story_desc = "Bai toan dinh cao KTS: Cong 3 bit (A, B, Cin) xuat ra tong SUM va bit nho COUT!",
        .hint = "Goi X = A ^ B. Khi do: SUM = X ^ Cin; va COUT = (A . B) + (Cin . X). Chi can 5 cong!",
        .num_inputs = 3,
        .input_labels = {"A", "B", "Cin"},
        .num_outputs = 2,
        .output_labels = {"SUM", "Cout"},
        .allowed_gates = { [COMP_XOR] = 3, [COMP_AND] = 3, [COMP_OR] = 2 },
        .optimal_gate_count = 5,
        /* {SUM, Cout} cho 000..111 */
        .expected_outputs = {
            {0, 0}, {1, 0}, {1, 0}, {0, 1},
            {1, 0}, {0, 1}, {0, 1}, {1, 1}
        }
    }
};

const LevelDef* Levels_Get(int level_index) {
    if (level_index < 0 || level_index >= TOTAL_LEVELS) return &g_levels[0];
    return &g_levels[level_index];
}

void Levels_SetupCircuit(const LevelDef *level, Circuit *circuit) {
    if (!level || !circuit) return;
    Circuit_Init(circuit);

    /* Dat cac cong tac INPUT o cot ben trai bang mach */
    float start_y_in = 220.0f;
    float gap_in = 105.0f;
    if (level->num_inputs == 1) start_y_in = 310.0f;
    else if (level->num_inputs == 2) start_y_in = 250.0f;
    else if (level->num_inputs == 3) start_y_in = 195.0f;

    for (int i = 0; i < level->num_inputs; i++) {
        Circuit_AddComponent(circuit, COMP_INPUT, 65.0f, start_y_in + i * gap_in, true, level->input_labels[i]);
    }

    /* Dat cac den LED OUTPUT o cot ben phai bang mach */
    float start_y_out = 310.0f;
    float gap_out = 130.0f;
    if (level->num_outputs == 2) start_y_out = 245.0f;

    for (int i = 0; i < level->num_outputs; i++) {
        Circuit_AddComponent(circuit, COMP_OUTPUT, 775.0f, start_y_out + i * gap_out, true, level->output_labels[i]);
    }

    Circuit_Simulate(circuit);
}

void Levels_VerifyCircuit(const LevelDef *level, Circuit *circuit, VerificationResult *result) {
    if (!level || !circuit || !result) return;
    memset(result, 0, sizeof(VerificationResult));

    /* Luu trang thai cong tac INPUT hien tai cua nguoi choi de khoi phuc sau khi test */
    int saved_inputs[MAX_INPUTS] = {0};
    for (int i = 0; i < level->num_inputs; i++) {
        int cid = circuit->input_comp_ids[i];
        saved_inputs[i] = circuit->components[cid].output_value;
    }

    int total_rows = 1 << level->num_inputs; /* 2^num_inputs */
    result->total_rows = total_rows;
    result->passed_rows = 0;
    result->all_passed = true;

    for (int row = 0; row < total_rows; row++) {
        /* Gan bit dau vao theo nhi phan: bit cao nhat la Input 0 */
        for (int b = 0; b < level->num_inputs; b++) {
            int bit_shift = (level->num_inputs - 1 - b);
            int bit_val = (row >> bit_shift) & 1;
            int cid = circuit->input_comp_ids[b];
            circuit->components[cid].output_value = bit_val;
        }

        Circuit_Simulate(circuit);

        bool row_ok = true;
        for (int o = 0; o < level->num_outputs; o++) {
            int out_cid = circuit->output_comp_ids[o];
            int actual = circuit->components[out_cid].output_value;
            result->actual_outputs[row][o] = actual;
            if (actual != level->expected_outputs[row][o]) {
                row_ok = false;
            }
        }

        result->row_passed[row] = row_ok;
        if (row_ok) {
            result->passed_rows++;
        } else {
            result->all_passed = false;
        }
    }

    /* Khoi phuc trang thai cong tac INPUT cua nguoi choi */
    for (int i = 0; i < level->num_inputs; i++) {
        int cid = circuit->input_comp_ids[i];
        circuit->components[cid].output_value = saved_inputs[i];
    }
    Circuit_Simulate(circuit);
}

void Progress_InitDefault(PlayerProgress *prog) {
    if (!prog) return;
    memset(prog, 0, sizeof(PlayerProgress));
    prog->unlocked_level = 1; /* Mo san man 1, hoac nguoi choi co the mo tat ca de demo */
}

bool Progress_Load(PlayerProgress *prog, const char *filepath) {
    if (!prog || !filepath) return false;
    Progress_InitDefault(prog);

    FILE *f = fopen(filepath, "rb");
    if (!f) return false;

    size_t n = fread(prog, sizeof(PlayerProgress), 1, f);
    fclose(f);

    if (n != 1 || prog->unlocked_level < 1 || prog->unlocked_level > TOTAL_LEVELS) {
        Progress_InitDefault(prog);
        return false;
    }
    return true;
}

bool Progress_Save(const PlayerProgress *prog, const char *filepath) {
    if (!prog || !filepath) return false;
    FILE *f = fopen(filepath, "wb");
    if (!f) return false;

    size_t n = fwrite(prog, sizeof(PlayerProgress), 1, f);
    fclose(f);
    return (n == 1);
}
