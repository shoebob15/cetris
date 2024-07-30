#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

const float TICK_INT = 0.5f; // 800 ms
float elapsed_time = 0.0f; // elapsed time since last tick


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

// colors
Color bg = (Color) { 28, 28, 28, 255 }; // bg color

// tetromino types
typedef enum {
    TETROMINO_STRAIGHT,
    TETROMINO_SQUARE,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_COUNT
} TetrominoType;

// clockwise-rotation
typedef enum {
    ZERO,
    ONE,
    TWO,
    THREE
} Rotation;

Color get_tetromino_color(TetrominoType t) {
    switch(t) {
        case TETROMINO_STRAIGHT: return (Color) { 0, 255, 255, 255 };
        case TETROMINO_SQUARE: return (Color) { 255, 255, 0, 255 };
        case TETROMINO_T: return (Color) { 255, 0, 255, 255 };
        case TETROMINO_L: return (Color) { 255, 165, 0, 255 };
        case TETROMINO_J: return (Color) { 0, 0, 255, 255 };
        case TETROMINO_S: return (Color) { 0, 255, 0, 255 };
        case TETROMINO_Z: return (Color) { 255, 0, 0, 255 };
        default: return (Color) { 255, 255, 255, 255 };
    }
}

// tetromino patterns stored in 16-bit integers
typedef struct {
    u16 patterns[4];
} Tetromino;

// Tetromino definitions
Tetromino tetrominoes[TETROMINO_COUNT] = {
    // I
    { .patterns = { 0x0F00, 0x2222, 0x0F00, 0x2222 } },
    // O
    { .patterns = { 0x0660, 0x0660, 0x0660, 0x0660 } },
    // T
    { .patterns = { 0x0E40, 0x4640, 0x4E00, 0x4C40 } },
    // L
    { .patterns = { 0x0E20, 0x44C0, 0x8E00, 0xC440 } },
    // J
    { .patterns = { 0x0E80, 0x6440, 0x2E00, 0x4460 } },
    // S
    { .patterns = { 0x06C0, 0x4620, 0x06C0, 0x4620 } },
    // Z
    { .patterns = { 0x0C60, 0x2640, 0x0C60, 0x2640 } },
};

typedef struct {
    u8 matrix[20][10]; // 20x10 grid (0 empty, 1 filled)
    TetrominoType current_tetromino;
    Rotation current_rotation;
    Vector2 current_position;
} GameState;

GameState state = {
    .matrix = {0},
    .current_tetromino = TETROMINO_STRAIGHT,
    .current_rotation = ZERO,
    .current_position = (Vector2) { 1, 0 } // position relative to 2d grid (top-left)
};

// init window
void init() {
    SetExitKey(0); // no exit with esc
    InitWindow(800, 640, "Cetris");
    SetTargetFPS(60);

    tetromino_block = LoadTexture("assets/tetromino_block.png");
}

void draw_game_box() {
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

// pos relative to top-left corner of matrix (0, 0)
// TODO: no vector2, can be represented with decimals
void draw_tetromino(Vector2* pos, TetrominoType type, Rotation rotation) {
    u16 pattern = tetrominoes[type].patterns[rotation];

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                DrawTextureEx(tetromino_block,
                    (Vector2) { 240 - 16 + (pos->x + x) * 32, ((pos->y + y) * 32) - 32 },
                    0.0, 2.0, get_tetromino_color(type)
                );
            }
        }
    }
}

TetrominoType get_random_tetromino() {
    return (TetrominoType) GetRandomValue(0, TETROMINO_COUNT - 1);
}

// called every tick-frame (defined as tick-int const)
void tick() {
    if (!(state.current_position.y > 18)) {
        state.current_position.y++;
    }
    TraceLog(LOG_INFO, "%f", state.current_position.y);

    elapsed_time = 0.0f;
}

// called every frame
void update() {

}

// (also) called every frame
void draw() {
    BeginDrawing();
    ClearBackground(bg);

    draw_game_box();
    draw_tetromino(&state.current_position, state.current_tetromino, state.current_rotation);

    EndDrawing();
}

// (and also do the tick 🤫)
void check_for_tick() {
    elapsed_time += GetFrameTime();

    if (elapsed_time >= TICK_INT) {
        tick();
    }
}

int main() {
    init();

    while (!WindowShouldClose()) {
        check_for_tick(); // "update"
        update();

        draw();
    }

    CloseWindow();

    return 0;
}
