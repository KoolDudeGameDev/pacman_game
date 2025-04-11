#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "raylib.h"
#include "resource_dir.h"

#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define TILE_SIZE 20
#define MAX_GHOSTS 4

// Tile types
typedef enum {
    EMPTY = 0,
    WALL = 1,
    PELLET = 2,
    POWER_PELLET = 3,
    GHOST_GATE = 4
} TileType;

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

// Direction enum
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// Ghost states
typedef enum {
    GHOST_NORMAL,
    GHOST_FRIGHTENED,
    GHOST_EATEN
} GhostState;

// Pac-Man structure
typedef struct {
    float x;         // Grid position X
    float y;         // Grid position Y
    float speed;     // Pixels per frame
    int gridX;       // Grid position X
    int gridY;       // Grid position Y
    int score;     // Player score
    int lives;     // Player lives
    Direction direction;     // Current direction
    Direction nextDirection; // Queued direction
} Player;

// Ghost structure
typedef struct {
    float x;         // Grid position X
    float y;         // Grid position Y
    float speed;     // Pixels per frame
    int gridX;       // Grid position X
    int gridY;       // Grid position Y
    Direction direction;
    GhostState state;
    float stateTimer;
} Ghost;

// Global Variables (extern to declare them, defined in game.c)
extern int maze[MAZE_HEIGHT][MAZE_WIDTH];
extern Player pacman;
extern Ghost ghosts[MAX_GHOSTS];
extern GameState gameState;

// Function Declarations

// game.c
void init_maze(void);

// pacman_movement.c
void init_pacman(void);
void update_pacman(void);

// ghost_ai.c
void init_ghosts(void);
void update_ghosts(void);

#endif // GAME_LOGIC_H