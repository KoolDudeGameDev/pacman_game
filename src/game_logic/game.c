#include "game_logic.h"
#include <stdio.h>

int score = 0;

void InitGame(Pacman* pacman, Ghost ghosts[], Maze* maze) {
    // Initialize Pacman
    pacman->x = 640;  // Center of 1280x800 screen
    pacman->y = 400;
    pacman->speed = 5;
    pacman->radius = 15;

    // Initialize Ghosts (4 ghosts)
    ghosts[0] = (Ghost){100, 100, 4, RED};
    ghosts[1] = (Ghost){1180, 100, 4, PINK};
    ghosts[2] = (Ghost){100, 700, 4, GREEN};
    ghosts[3] = (Ghost){1180, 700, 4, ORANGE};

    // Initialize Maze (simplified example)
    maze->tileSize = 32;
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 25; x++) {
            if (x == 0 || x == 24 || y == 0 || y == 19) {
                maze->grid[y][x] = 0;  // Walls
            } else {
                maze->grid[y][x] = 2;  // Pellets
            }
        }
    }
    // Add some inner walls
    maze->grid[5][5] = 0;
    maze->grid[5][6] = 0;
}

void UpdatePacman(Pacman* pacman, Maze* maze) {
    int newX = pacman->x;
    int newY = pacman->y;

    if (IsKeyDown(KEY_RIGHT)) newX += pacman->speed;
    if (IsKeyDown(KEY_LEFT)) newX -= pacman->speed;
    if (IsKeyDown(KEY_UP)) newY -= pacman->speed;
    if (IsKeyDown(KEY_DOWN)) newY += pacman->speed;

    int gridX = newX / maze->tileSize;
    int gridY = newY / maze->tileSize;

    if (maze->grid[gridY][gridX] != 0) {  // Not a wall
        pacman->x = newX;
        pacman->y = newY;
        if (maze->grid[gridY][gridX] == 2) {
            maze->grid[gridY][gridX] = 1;  // Eat pellet
            score += 10;
        }
    }
}

void UpdateGhost(Ghost* ghost, Maze* maze) {
    // Simple movement (random for now)
    int dir = GetRandomValue(0, 3);
    int newX = ghost->x;
    int newY = ghost->y;

    if (dir == 0) newX += ghost->speed;
    if (dir == 1) newX -= ghost->speed;
    if (dir == 2) newY -= ghost->speed;
    if (dir == 3) newY += ghost->speed;

    int gridX = newX / maze->tileSize;
    int gridY = newY / maze->tileSize;

    if (gridX >= 0 && gridX < 25 && gridY >= 0 && gridY < 20 && maze->grid[gridY][gridX] != 0) {
        ghost->x = newX;
        ghost->y = newY;
    }
}

void DrawGame(Pacman* pacman, Ghost ghosts[], Maze* maze) {
    // Draw Maze
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 25; x++) {
            int tileX = x * maze->tileSize;
            int tileY = y * maze->tileSize;
            if (maze->grid[y][x] == 0) {
                DrawRectangle(tileX, tileY, maze->tileSize, maze->tileSize, BLUE);
            } else if (maze->grid[y][x] == 2) {
                DrawCircle(tileX + maze->tileSize / 2, tileY + maze->tileSize / 2, 5, YELLOW);
            }
        }
    }

    // Draw Pacman
    DrawCircle(pacman->x, pacman->y, pacman->radius, YELLOW);

    // Draw Ghosts
    for (int i = 0; i < 4; i++) {
        DrawCircle(ghosts[i].x, ghosts[i].y, 15, ghosts[i].color);
    }

    // Draw Score
    DrawText(TextFormat("Score: %i", score), 20, 20, 40, WHITE);
}