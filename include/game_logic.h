#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "raylib.h"
#include "all_libs.h"
#include "resource_dir.h"

#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define TILE_SIZE 20
#define MAX_GHOSTS 4
#define PACMAN_DEATH_FRAMES 11

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
    STATE_READY,         // Ready phase before playing
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_DEATH_ANIM,
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
    GHOST_EATEN,
    GHOST_PENNED,
    GHOST_RETURNING
} GhostState;

// Ghost mode (for GHOST_NORMAL state)
typedef enum {
    MODE_CHASE,
    MODE_SCATTER
} GhostMode;

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
    Texture2D sprite;
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
    int scatterTargetX;
    int scatterTargetY;

    // Animation data
    Texture2D normalSprite[2];  // Two frames for normal state animation
    Texture2D frightenedSprite; // Frightened sprite
    Texture2D eyeballSprite;    // Eyeball sprite for returning state
    float animTimer;            // Timer to control animation frame switching
    int currentFrame;           // Current animation frame (0 or 1 for normal state)
    float frightenedBlinkTimer; // Timer for frightened state blinking
} Ghost;

// Global Variables (extern to declare them, defined in game.c)
extern Texture2D spriteSheet;
extern GameState gameState;
extern GhostMode ghostMode;
extern float modeTimer;
extern float readyTimer;
extern float deathAnimTimer;  // Timer for death animation
extern float blinkTimer;      // Timer for blinking animations (power pellets)
extern int deathAnimFrame;    // Current frame of death animation
extern bool isResetting;      // Flag to indicate if game is resetting
extern int maze[MAZE_HEIGHT][MAZE_WIDTH];
extern Player pacman;
extern Ghost ghosts[MAX_GHOSTS];

// Function Declarations

// game.c
void init_maze(void);
void find_pacman_start(int *startX, int *startY);
void reset_game_state(void);

// pacman_movement.c
void init_pacman(int startX, int startY);
void update_pacman(void);

// ghost_ai.c
void init_ghosts(void);
void update_ghosts(void);
void update_ghost_mode(void);

#endif // GAME_LOGIC_H