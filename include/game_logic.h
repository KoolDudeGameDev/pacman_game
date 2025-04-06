#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "resource_dir.h"

extern int score;

// Pacman structure
typedef struct {
    float x, y;
    int speed;
    int radius;
} Pacman;

// Ghost structure
typedef struct {
    float x, y;
    int speed;
    Color color;
} Ghost;

// Maze structure (simplified as a grid)
typedef struct {
    int grid[20][25];  // 0 = wall, 1 = path, 2 = pellet
    int tileSize;
} Maze;

// Function declarations
void InitGame(Pacman* pacman, Ghost ghosts[], Maze* maze);
void UpdatePacman(Pacman* pacman, Maze* maze);
void UpdateGhost(Ghost* ghost, Maze* maze);
void DrawGame(Pacman* pacman, Ghost ghosts[], Maze* maze);

#endif // GAME_H