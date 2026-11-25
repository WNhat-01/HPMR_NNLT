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
        .title = "Màn 1: Đèn Báo Khẩn Cấp (Cổng NOT)",
        .story_desc = "Đèn khẩn cấp Y phải TỰ ĐỘNG SÁNG (1) khi nguồn điện chính A bị ngắt (0), và TẮT (0) khi có điện (1).",
        .hint = "Kéo 1 cổng NOT từ thanh công cụ xuống và nối A -> NOT -> Y.",
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
        .title = "Màn 2: Khóa An Toàn Kép (Cổng AND)",
        .story_desc = "Máy dập công nghiệp Y chỉ được phép hoạt động khi công nhân nhấn ĐỒNG THỜI cả 2 nút A và B.",
        .hint = "Cổng AND chỉ xuất ra 1 khi tất cả ngõ vào đều bằng 1.",
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
        .title = "Màn 3: Chuông Báo Cháy 2 Tầng (Cổng OR)",
        .story_desc = "Còi báo động Y phải kêu (1) khi cảm biến khói Tầng 1 (A) HOẶC Tầng 2 (B) phát hiện khói.",
        .hint = "Sử dụng 1 cổng OR nối với A và B.",
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
        .title = "Màn 4: Công Tắc Cầu Thang (Cổng XOR)",
        .story_desc = "Đèn cầu thang Y sáng (1) khi 2 công tắc A và B ở trạng thái KHÁC NHAU, và tắt khi chúng cùng trạng thái.",
        .hint = "Cổng XOR xuất 1 khi hai ngõ vào A, B khác nhau (0-1 hoặc 1-0).",
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
        .title = "Màn 5: Bảo Vệ Quá Nhiệt Động Cơ",
        .story_desc = "Động cơ Y chạy (1) khi ông tắc nguồn A = 1 NHƯNG cảm biến quá nhiệt B phải bằng 0 (chưa nóng).",
        .hint = "Đảo ngược tín hiệu B bằng cổng NOT, sau đó đưa vào cổng AND cùng với A: Y = A . (~B).",
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
        .title = "Màn 6: Chế Tạo Cổng NAND",
        .story_desc = "Kho linh kiện hết cổng NAND! Hãy ghép các cổng cơ bản để tạo mạch chỉ TẮT (0) khi cả A và B đều bằng 1.",
        .hint = "Cổng NAND chính là cổng AND mắc nối tiếp với cổng NOT ở ngõ ra: Y = ~(A . B).",
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
        .title = "Màn 7: Định Lý De Morgan (Tạo OR từ NAND)",
        .story_desc = "Không có cổng OR! Hãy dùng Định lý De Morgan để tạo hàm Y = A + B chỉ từ cổng NOT và NAND.",
        .hint = "Theo De Morgan: A + B = ~(~A . ~B). Dùng 2 cổng NOT đảo A, B rồi đưa vào 1 cổng NAND.",
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
        .title = "Màn 8: Bộ So Sánh Bằng (Equality Detector)",
        .story_desc = "Đèn Y sáng (1) khi hai tín hiệu A và B GIỐNG HẾT NHAU (cung bang 0 hoac cung bang 1).",
        .hint = "Kết hợp cổng XOR và cổng NOT: Y = ~(A ^ B).",
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
        .title = "Màn 9: Hệ Thống Tưới Cây Thông Minh (3 Input)",
        .story_desc = "Máy bom Y bật (1) khi Đặt kho (A=1) VÀ (Đang là ban đêm B=1 HOẶC Chế độ khẩn cấp C=1).",
        .hint = "Biểu thức Boole: Y = A . (B + C). Dùng 1 cổng OR cho (B, C) rồi nối vào cổng AND cùng với A.",
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
        .title = "Màn 10: Tự Chế Cổng XOR (Không Dùng Có Sẵn)",
        .story_desc = "Thử thách kinh điển: Tạo hàm XOR (Y = A ^ B) chỉ bằng 3 cổng: AND, OR và NAND!",
        .hint = "Ta có: A ^ B = (A + B) . ~(A . B). Lấy ngo ra của OR(A,B) và NAND(A,B) đưa vào cổng AND!",
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
        .title = "Màn 11: Bộ Cộng Bán Phân (Half Adder)",
        .story_desc = "Thiết kế mạch cộng 2 bit nhị phân A + B với 2 ngo ra: Tổng SUM (S) và Bit nhớ CARRY (C).",
        .hint = "SUM = A ^ B (dùng cổng XOR) và CARRY = A . B (dùng cổng AND).",
        .num_inputs = 2,
        .input_labels = {"A", "B"},
        .num_outputs = 2,
        .output_labels = {"SUM", "CARRY"},
        .allowed_gates = { [COMP_XOR] = 2, [COMP_AND] = 2, [COMP_OR] = 2 },
        .optimal_gate_count = 2,
        /* {SUM, CARRY} cho 00, 01, 10, 11 */
        .expected_outputs = { {0, 0}, {1, 0}, {1, 0}, {0, 1} }
    },
    {
        .level_number = 12,
        .chapter = 3,
        .title = "Màn 12: Bộ So sánh 1-Bit (Comparator)",
        .story_desc = "So sánh hai bit A và B: Ngo ra GT = 1 khi (A > B), và ngo ra EQ = 1 khi (A == B).",
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
        .title = "Màn 13: Bộ Chọn Kênh MUX 2-sang-1",
        .story_desc = "Mạch Multiplexer: Khi chọn S = 0 thì ngõ ra Y = A; khi S = 1 thì ngõ ra Y = B.",
        .hint = "Công thức MUX: Y = (A . ~S) + (B . S). Dùng 1 cổng NOT, 2 cổng AND và 1 cổng OR.",
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
        .title = "Màn 14: Mạch Biểu Quyết Đại Số (Majority Voter)",
        .story_desc = "3 cảm biến A, B, C trên tàu vũ trụ. Ngõ ra Y = 1 nếu có ít nhất 2 trên 3 cảm biến bằng 1.",
        .hint = "Tối ưu với 4 cổng: Y = (A . B) + (C . (A ^ B)). Dùng 1 XOR, 2 AND và 1 OR!",
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
        .title = "Màn 15: Bộ Cộng Toán Phân (Full Adder - BOSS)",
        .story_desc = "Bài toán đỉnh cao môn KTS: Cộng 3 bit (A, B, Cin) xuất ra tổng SUM và bit nhớ COUT!",
        .hint = "Gọi X = A ^ B. Khi đó: SUM = X ^ Cin; và COUT = (A . B) + (Cin . X). Chỉ cần 5 cổng!",
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
