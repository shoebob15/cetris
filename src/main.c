#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

float TICK_INT = 0.5f; // 800 ms (not const for speeding up the game when holding down)
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
Font font_b; // bold
Font font_r; // regular

// sounds
Sound clear;
Sound death;
Sound select;
Sound song;

// colors
Color bg;

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

typedef enum {
    PLAYING,
    MENU,
    GAME_OVER
} GameScreen;

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
    { .patterns = { 0x0E20, 0x44C0, 0xE800, 0xC440 } },
    // J
    { .patterns = { 0x0E80, 0x6440, 0x2E00, 0x4460 } },
    // S
    { .patterns = { 0x06C0, 0x4620, 0x06C0, 0x4620 } },
    // Z
    { .patterns = { 0x0C60, 0x2640, 0x0C60, 0x2640 } },
};

typedef struct {
    u8 active;
    Color color;
} MatrixSquare;

typedef struct {
    MatrixSquare matrix[20][10]; // 20x10 grid (if color == black)
    TetrominoType current_tetrominoes[2]; // tetromino buffer (current, next)
    Rotation current_rotation;
    Vector2 current_position;
    bool game_over;
    GameScreen screen;
    u32 score;
    u8 level;
    u8 lines;
} GameState;

GameState state;

// init window
void init() {
    InitAudioDevice();
    SetExitKey(0); // no exit with esc
    InitWindow(800, 640, "Cetris");
    SetTargetFPS(60);

    tetromino_block = LoadTexture("assets/tetromino_block.png");
    font_b = LoadFont("assets/font_b.ttf");
    font_r = LoadFont("assets/font_r.ttf");
    bg = (Color) { 28, 28, 28, 255 }; // bg color

    state = (GameState) {
        .current_tetrominoes = { TETROMINO_STRAIGHT, TETROMINO_STRAIGHT },
        .current_rotation = ZERO,
        .current_position = (Vector2) { -5, -5 },
        .game_over = false,
        .screen = MENU,
        .score = 0,
        .level = 1,
        .lines = 0
    };

    clear = LoadSound("assets/clear.wav");
    death = LoadSound("assets/death.wav");
    select = LoadSound("assets/select.wav");
    song = LoadSound("assets/song.wav");
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

void randomize_tetromino_buffer() {
    state.current_tetrominoes[0] = get_random_tetromino();
    state.current_tetrominoes[1] = get_random_tetromino();
}

void swap_tetromino_buffer() {
    TetrominoType tmp = state.current_tetrominoes[1];
    state.current_tetrominoes[1] = state.current_tetrominoes[0];
    state.current_tetrominoes[0] = tmp;
}


void spawn_new_tetromino() {
    state.current_tetrominoes[0] = state.current_tetrominoes[1];
    state.current_tetrominoes[1] = get_random_tetromino();
    state.current_rotation = ZERO;
    state.current_position = (Vector2) { 4, 0 };
}

void reset_game() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 20; y++) {
            state.matrix[y][x].active = false;
            state.matrix[y][x].color = BLACK;
        }
    }

    state.game_over = false;
    state.score = 0;
    state.level = 1;
    state.lines = 0;
}



// TODO: can_move_left, can_move_right, can_rotate all use the mostly same code - figure out way to abstract
int can_move_left() {
    u16 pattern = tetrominoes[state.current_tetrominoes[0]].patterns[state.current_rotation];
    Vector2 pos = state.current_position;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                if (pos.x + x <= 1 || state.matrix[(int)(pos.y + y)][(int)(pos.x + x - 1)].active) {
                    return false;
                }
            }
        }
    }

    return true;
}

int can_move_right() {
    u16 pattern = tetrominoes[state.current_tetrominoes[0]].patterns[state.current_rotation];
    Vector2 pos = state.current_position;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                if (pos.x + x >= 9 || state.matrix[(int)(pos.y + y)][(int)(pos.x + x + 1)].active) {
                    return false;
                }
            }
        }
    }

    return true;
}

int can_rotate() {
    Rotation next_rotation = (state.current_rotation + 1) % 4;
    u16 pattern = tetrominoes[state.current_tetrominoes[0]].patterns[next_rotation];
    Vector2 pos = state.current_position;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                if (pos.x + x < 1 || pos.x + x > 9 || pos.y + y > 19 || state.matrix[(int)(pos.y + y)][(int)(pos.x + x)].active) {
                    return false;
                }
            }
        }
    }

    return true;
}

// calculate the current level and it's speed
void calculate_level() {
    state.level = (state.lines / 10) + 1;

    TICK_INT = 0.5f - (state.level * 0.05f); // TODO: time will stop at level 20
}

void check_input() {
    if (IsKeyPressed(KEY_LEFT) && can_move_left()) state.current_position.x--;

    if (IsKeyPressed(KEY_RIGHT) && can_move_right()) state.current_position.x++;

    if (IsKeyPressed(KEY_DOWN)) TICK_INT = 0.05f;

    if (IsKeyReleased(KEY_DOWN)) calculate_level();

    if (IsKeyPressed(KEY_ENTER)) swap_tetromino_buffer();

    // menu-state stuff

    if (IsKeyPressed(KEY_ENTER) && (state.screen == GAME_OVER || state.screen == MENU)) {
        state.screen = PLAYING;
        PlaySound(select);
        randomize_tetromino_buffer();
        reset_game();
        spawn_new_tetromino();
    }

    if (IsKeyPressed(KEY_UP) && can_rotate()) state.current_rotation = (state.current_rotation + 1) % 4;
}

void commit_current_tetromino_to_matrix() {
    u16 pattern = tetrominoes[state.current_tetrominoes[0]].patterns[state.current_rotation];
    Vector2 pos = state.current_position;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                state.matrix[(int)(pos.y + y)][(int)(pos.x + x)].active = 1;
                state.matrix[(int)(pos.y + y)][(int)(pos.x + x)].color = get_tetromino_color(state.current_tetrominoes[0]);
            }
        }
    }
}

void draw_matrix() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 20; y++) {
            if (state.matrix[y][x].active) {
                DrawTextureEx(tetromino_block,
                    (Vector2) { 240 - 16 + x * 32, y * 32 },
                    0.0, 2.0, state.matrix[y][x].color
                );
            }
        }
    }
}

void draw_tetromino_buffer() {
    DrawTextEx(font_b, "NEXT",
        (Vector2) { GetScreenWidth() / 2.0f - (MeasureTextEx(font_r, "NEXT", 40.0f, 1.0f).x / 2.0f - 275),
            50 }, 40.0f, 1.0f, WHITE
    );
    if (state.screen == PLAYING) {
        draw_tetromino(&(Vector2) { 12.5, 3.5 }, state.current_tetrominoes[1], 0);
    }
}

void draw_menus() {
    switch (state.screen) {
        case MENU:
            DrawTextEx(font_b, "TETRIS",
                (Vector2) { GetScreenWidth() / 2.0f - (MeasureTextEx(font_b, "TETRIS", 100.0f, 1.0f).x / 2),
                    100 }, 100.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, "Press <ENTER> to start",
                (Vector2) { GetScreenWidth() / 2.0f - (MeasureTextEx(font_r, "Press <ENTER> to start", 50.0f, 1.0f).x / 2.0f + 35),
                    250 }, 50.0f, 1.0f, WHITE
            );
            break;

        case GAME_OVER:
            DrawTextEx(font_b, "GAME OVER",
                (Vector2) { GetScreenWidth() / 2.0f - (MeasureTextEx(font_b, "GAME OVER", 100.0f, 1.0f).x / 2),
                    100 }, 100.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, "Press <ENTER> to restart",
                (Vector2) { GetScreenWidth() / 2.0f - (MeasureTextEx(font_r, "Press <ENTER> to restart", 50.0f, 1.0f).x / 2.0f + 35),
                    250 }, 50.0f, 1.0f, WHITE
            );
            break;

        case PLAYING: {
            char scoreText[12];
            sprintf(scoreText, "%u", state.score);

            char linesText[12];
            sprintf(linesText, "%u", state.lines);

            char levelText[12];
            sprintf(levelText, "%u", state.level);

            DrawTextEx(font_b, "SCORE",
                (Vector2) { 30, 50 }, 40.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, scoreText,
                (Vector2) { 30, 100 }, 60.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, "LINES",
                (Vector2) { 30, 200 }, 40.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, linesText,
                (Vector2) { 30, 250 }, 60.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, "LEVEL",
                (Vector2) { 30, 350 }, 40.0f, 1.0f, WHITE
            );

            DrawTextEx(font_b, levelText,
                (Vector2) { 30, 400 }, 60.0f, 1.0f, WHITE
            );

            draw_tetromino_buffer();



            break;
        }
    }
}

void check_for_full_lines() {
    for (int y = 0; y < 20; y++) {
        bool full = true;

        for (int x = 1; x < 10; x++) {
            if (!state.matrix[y][x].active) {
                full = false;
                break;
            }
        }

        if (full) {
            state.score += 100;
            state.lines += 1;

            calculate_level();

            PlaySound(clear);

            for (int i = y; i > 0; i--) {
                for (int x = 0; x < 10; x++) {
                    state.matrix[i][x] = state.matrix[i - 1][x];
                }
            }
        }
    }
}

void check_for_game_over() {
    for (int x = 1; x < 10; x++) {
        if (state.matrix[0][x].active) {
            state.game_over = true;
            state.screen = GAME_OVER;
            PlaySound(death);
            StopSound(song);
            break;
        }
    }
}




// called every tick-frame (defined as tick-int const)
void tick() {
    u16 pattern = tetrominoes[state.current_tetrominoes[0]].patterns[state.current_rotation];
    Vector2 pos = state.current_position;

    // check collision with bottom
    bool canMoveDown = true;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if ((pattern >> (15 - (y * 4 + x))) & 1) {
                // check if a block is at the bottom or touching another block
                if (pos.y + y >= 20 || state.matrix[(int)(pos.y + y)][(int)(pos.x + x)].active) {
                    canMoveDown = false;
                    break;
                }
            }
        }
        if (!canMoveDown) break;
    }

    if (canMoveDown) {
        state.current_position.y++;
    } else {
        state.current_position.y--; // TODO: jank
        commit_current_tetromino_to_matrix();
        check_for_full_lines();
        check_for_game_over();
        spawn_new_tetromino();
    }



    elapsed_time = 0.0f;
}

void play_song() {
    if (!IsSoundPlaying(song) && state.screen != GAME_OVER) {
        PlaySound(song);
    }
}

// called every frame
void update() {
    check_input();

    play_song();
}

// (also) called every frame
void draw() {
    BeginDrawing();
    ClearBackground(bg);

    draw_game_box();
    draw_tetromino(&state.current_position, state.current_tetrominoes[0], state.current_rotation);
    draw_matrix();
    draw_menus();

    EndDrawing();
}

// (and also do the tick 🤫)
void check_for_tick() {
    elapsed_time += GetFrameTime();

    if (elapsed_time >= TICK_INT && state.screen == PLAYING) {
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
