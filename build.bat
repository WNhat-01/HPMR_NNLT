@echo off
echo ============================================================
echo   BIEN DICH PROJECT C: CIRCUIT PUZZLE (KHOA DIEN HCMUT)
echo ============================================================

gcc -Wall -Wextra -O2 src/main.c src/circuit.c src/levels.c src/renderer.c src/interaction.c src/ui.c src/gfx.c -Iinclude -lgdi32 -lm -o circuit_puzzle.exe

if %ERRORLEVEL% EQU 0 (
    echo [THANH CONG] Da tao file circuit_puzzle.exe!
    echo Dang khoi dong game...
    start "" circuit_puzzle.exe
) else (
    echo [LOI] Bien dich that bai! Kiem tra lai GCC trong PATH.
    pause
)
