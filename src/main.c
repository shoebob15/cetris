#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "Cetris");
    SetTargetFPS(60);


    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
