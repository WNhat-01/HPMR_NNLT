# CIRCUIT PUZZLE: LOGIC ARCHITECT (HCMUT EE Edition)

**Đề tài Project Ngôn Ngữ C** – Thiết kế cho nhóm **5 sinh viên năm 2 Khoa Điện - Điện Tử (ĐH Bách Khoa TP.HCM - HCMUT)**.

---

## 1. Giới thiệu chung
**Circuit Puzzle: Logic Architect** là tựa game giải đố thiết kế mạch logic số 2D viết hoàn toàn bằng **ngôn ngữ C chuẩn**. Người chơi sử dụng các cổng logic (`NOT`, `AND`, `OR`, `XOR`, `NAND`, `NOR`, `XNOR`) để lắp ráp mạch điện sao cho đầu ra khớp 100% với **Bảng Chân Trị (Truth Table)** của từng bài toán thực tế.

### Điểm mạnh khi bảo vệ đồ án trước giảng viên Khoa Điện:
- **Đúng chuyên ngành Điện - Điện Tử:** Kết hợp trực tiếp kiến thức môn *Lập trình C* và *Kỹ thuật số* (Định lý De Morgan, Mạch tổ hợp, Half Adder, Full Adder, Multiplexer 2-to-1, Majority Voter, Comparator).
- **Đồ họa 2D mượt mà không phụ thuộc thư viện ngoài:** Sử dụng **32-bit Framebuffer + Win32 GDI Double-Buffering** (`gfx.c`), vẽ dây dẫn đường cong **Cubic Bezier** kèm hiệu ứng hạt xung điện chạy trên dây khi mức logic = `1` (`HIGH`). Chỉ cần `gcc` có sẵn trong Dev-C++, Code::Blocks hoặc MSYS2 là biên dịch chạy ngay trên mọi máy tính của 5 thành viên.
- **15 Màn chơi chia làm 3 Chương (< 5 phút / màn):** Tăng dần độ khó từ cơ bản đến nâng cao, có hệ thống tính sao (1–3 Sao), đồng hồ đếm giờ, Gợi ý (Hint), lưu tiến độ ra file nhị phân (`progress.dat`) và chế độ **Sandbox** tự do lắp mạch.

---

## 2. Phân công công việc cho nhóm 5 người (5 Modules độc lập)

| Thành viên | Module phụ trách | File code | Nội dung công việc & Kiến thức C áp dụng |
| :--- | :--- | :--- | :--- |
| **Thành viên 1** | **Core Logic & Simulation Engine** | `include/circuit.h`<br>`src/circuit.c` | Quản lý cấu trúc dữ liệu `Circuit`, `Component`, `Wire`. Cài đặt hàm tính toán cổng Boole và thuật toán lan truyền tín hiệu (`Circuit_Simulate`). |
| **Thành viên 2** | **Level Design, Verification & File I/O** | `include/levels.h`<br>`src/levels.c` | Thiết kế 15 màn chơi (`g_levels`), thuật toán quét $2^N$ tổ hợp nhị phân kiểm tra tự động Bảng Chân Trị (`Levels_VerifyCircuit`), đọc/ghi file nhị phân `progress.dat` (`fopen`, `fread`, `fwrite`). |
| **Thành viên 3** | **2D Graphics & PCB Renderer** | `include/gfx.h`<br>`src/gfx.c`<br>`src/renderer.c` | Xây dựng bộ vẽ 2D Double-Buffered, vẽ bảng mạch PCB, cổng logic, đèn LED, đường cong dây dẫn Bezier có xung điện và hiển thị Bảng Chân Trị. |
| **Thành viên 4** | **User Interaction & Editor Controller** | `src/interaction.c` | Xử lý sự kiện chuột: Kéo thả cổng logic (Drag & Drop), nối dây giữa các chân Pin, bật/tắt công tắc Input (`0`/`1`), xóa dây/linh kiện bằng chuột phải. |
| **Thành viên 5** | **UI Screens, Menu & Game Flow** | `include/game.h`<br>`src/ui.c`<br>`src/main.c` | Quản lý máy trạng thái (`STATE_MAIN_MENU`, `STATE_LEVEL_SELECT`, `STATE_PLAYING`, `STATE_SANDBOX`), đồng hồ đếm thời gian (< 5 phút), chấm điểm 1–3 Sao, chế độ Demo mở khóa toàn bộ màn. |

---

## 3. Danh sách 15 Màn chơi & Đáp án nhanh (Dành cho nhóm khi thuyết trình)

### Chương 1: Cơ Bản – Nhập Môn Cổng Logic (Màn 1 – 5)
1. **Màn 1 – Đèn Báo Khẩn Cấp (`NOT`):** Nối `A -> NOT -> Y` ($Y = \overline{A}$).
2. **Màn 2 – Khóa An Toàn Kép (`AND`):** Nối `A, B -> AND -> Y` ($Y = A \cdot B$).
3. **Màn 3 – Chuông Báo Cháy 2 Tầng (`OR`):** Nối `A, B -> OR -> Y` ($Y = A + B$).
4. **Màn 4 – Công Tắc Cầu Thang (`XOR`):** Nối `A, B -> XOR -> Y` ($Y = A \oplus B$).
5. **Màn 5 – Bảo Vệ Quá Nhiệt Động Cơ:** Đảo `B` qua cổng `NOT`, sau đó đưa `A` và `~B` vào cổng `AND` ($Y = A \cdot \overline{B}$).

### Chương 2: Trung Cấp – Biến Đổi Boole & Định Lý De Morgan (Màn 6 – 10)
6. **Màn 6 – Chế Tạo Cổng NAND:** Nối `A, B -> AND -> NOT -> Y` ($Y = \overline{A \cdot B}$).
7. **Màn 7 – Định Lý De Morgan (Tạo OR từ NAND & NOT):** Đảo `A` bằng `NOT`, đảo `B` bằng `NOT`, rồi đưa cả hai vào `NAND` ($Y = \overline{\overline{A} \cdot \overline{B}} = A + B$).
8. **Màn 8 – Bộ So Sánh Bằng (`Equality Detector`):** Nối `A, B -> XOR -> NOT -> Y` ($Y = \overline{A \oplus B}$).
9. **Màn 9 – Hệ Thống Tưới Cây Thông Minh (3 Ngõ Vào):** Đưa `B, C` vào cổng `OR`, sau đó nối ngõ ra của `OR` với `A` vào cổng `AND` ($Y = A \cdot (B + C)$).
10. **Màn 10 – Tự Chế Cổng XOR (Không Dùng XOR Có Sẵn):** Dùng 1 cổng `OR(A,B)` và 1 cổng `NAND(A,B)`, sau đó đưa 2 ngõ ra này vào cổng `AND` ($Y = (A + B) \cdot \overline{A \cdot B}$).

### Chương 3: Kỹ Sư KTS Bách Khoa – Mạch Chức Năng (Màn 11 – 15)
11. **Màn 11 – Bộ Cộng Bán Phần (`Half Adder`):**
    - `SUM` = `XOR(A, B)`
    - `CAR` = `AND(A, B)`
12. **Màn 12 – Bộ So Sánh 1-Bit (`Comparator`):**
    - `GT` ($A > B$) = `AND(A, NOT(B))`
    - `EQ` ($A = B$) = `XNOR(A, B)`
13. **Màn 13 – Bộ Chọn Kênh MUX 2-sang-1 (`Multiplexer`):**
    - $Y = (A \cdot \overline{S}) + (B \cdot S)$ (Dùng 1 cổng `NOT`, 2 cổng `AND`, 1 cổng `OR`).
14. **Màn 14 – Mạch Biểu Quyết Đa Số (`3-Input Majority Voter`):**
    - Cách tối ưu 4 cổng: Tính $X = A \oplus B$, sau đó $Y = (A \cdot B) + (C \cdot X)$.
15. **Màn 15 – Bộ Cộng Toàn Phần (`Full Adder` - Màn BOSS):**
    - Gọi $X = A \oplus B$.
    - `SUM` = $X \oplus C_{in}$
    - `Cout` = $(A \cdot B) + (C_{in} \cdot X)$ (Tổng cộng chỉ tốn đúng 5 cổng: 2 `XOR`, 2 `AND`, 1 `OR`).

---

## 4. Hướng dẫn Biên dịch & Chạy Game

### Cách 1: Nhấp đúp file `build.bat`
Chỉ cần nhấp đúp vào `build.bat` (hoặc gõ `.\build.bat` trong Terminal). Script sẽ tự động gọi `gcc` biên dịch và mở cửa sổ game.

### Cách 2: Biên dịch bằng dòng lệnh GCC
```bash
gcc -Wall -Wextra -O2 src/main.c src/circuit.c src/levels.c src/renderer.c src/interaction.c src/ui.c src/gfx.c -Iinclude -lgdi32 -lm -o circuit_puzzle.exe
./circuit_puzzle.exe
```

### Cách 3: Chạy kiểm thử tự động (Self-Test)
```bash
./circuit_puzzle.exe --test
```
