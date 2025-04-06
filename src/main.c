#include "raylib.h"
#include "resource_dir.h"
#include "game_logic.h"
#include <stdio.h>

int main(void) {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;

    Pacman pacman;
    Ghost ghosts[4];
    Maze maze;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman Game v1.0");
    SetTargetFPS(60);

    InitGame(&pacman, ghosts, &maze);

    while (!WindowShouldClose()) {
        UpdatePacman(&pacman, &maze);
        for (int i = 0; i < 4; i++) {
            UpdateGhost(&ghosts[i], &maze);
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawGame(&pacman, ghosts, &maze);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}