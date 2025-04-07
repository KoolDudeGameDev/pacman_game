#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "resource_dir.h"

#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10
#define TILE_SIZE 40

// Tile types
typedef enum {
    EMPTY = 0,
    WALL = 1,
    PELLET = 2,
    POWER_PELLET = 3
};

// Game states
typedef enum {
    STATE_KOOLDUDE_LOGO, // Kyle Ibo Dev Logo
    STATE_DEV_LOGO,      // xAI Dev Logo
    STATE_RAYLIB_LOGO,   // Raylib logo
    STATE_LOGO,          // Game logo
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

// Difficulty levels
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;

// Pac-Man structure
typedef struct {
    int x;         // Grid position X
    int y;         // Grid position Y
    int speed;     // Pixels per frame
    int score;     // Player score
    int lives;     // Player lives
} Player;

// Global Variables (extern to declare them, defined in game.c)
extern int maze [MAZE_HEIGHT][MAZE_WIDTH];
extern Player pacman;
extern GameState gameState;
extern Difficulty selectedDifficulty;

// Function Declarations
void init_maze(void);

void init_pacman(void);
void update_pacman(void);

#endif // GAME_H