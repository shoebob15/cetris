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

// sprites
Texture tetromino_block;

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
   InitWindow(800, 640, "Cetris");
   SetTargetFPS(60);

   tetromino_block = LoadTexture("assets/tetromino_block.png");
}

void draw_game_board() {
    for (int r = 0; r < 2; r++) {
        for (int i = 0; i < 20; i++) {
            DrawTextureEx(tetromino_block,
                (Vector2) {
                    (240 - 16) + (r * (32 * 10)),
                    i * 32
                },
                0.0, 2.0, WHITE
            );
        }
    }
}

int main() {
    init();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        draw_game_board();
        ClearBackground(bg);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
