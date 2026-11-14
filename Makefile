CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lgdi32 -lm
SRCS    = src/main.c src/circuit.c src/levels.c src/renderer.c src/interaction.c src/ui.c src/gfx.c
TARGET  = circuit_puzzle.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) --test

clean:
	del /Q $(TARGET) progress.dat 2>nul || true
