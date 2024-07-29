#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>

// personal typedefs
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

Color bg = (Color) { 28, 28, 28, 255 }; // bg color

// tetromino types
typedef enum {
    TETROMINO_STRAIGHT,
    TETROMINO_SQUARE,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_SKREW
} TetrominoType;


typedef struct {
    u8 matrix[20][10]; // 20x10 grid (0 empty, 1 filled)
} GameState;

GameState state = (GameState) {
    .matrix = {0} // init to empty
};

// init window
void init() {
   InitWindow(800, 600, "Cetris");
   SetTargetFPS(60);
}

int main() {
    init();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(bg);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
