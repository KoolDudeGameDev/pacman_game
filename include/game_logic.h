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
#define MAX_HIGH_SCORES 5
#define MAX_NAME_LENGTH 4   // 3 initials + null terminator

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
    STATE_HIGHSCORES,
    STATE_ABOUT,
    STATE_READY,         // Ready phase before playing
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_DEATH_ANIM,
    STATE_GHOST_EATEN,
    STATE_LEVEL_COMPLETE,
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

// High score structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} HighScore;

// Fruit structure
typedef struct {
    bool active;        // Whether the fruit is currently active
    float timer;        // Timer for how long the fruit is visible
    int gridX;          // Grid position X
    int gridY;          // Grid position Y
    Texture2D sprite;   // Fruit sprite
    int points;         // Points awarded when collected
} Fruit;

// Pac-Man structure
typedef struct {
    float x;         // Grid position X
    float y;         // Grid position Y
    float speed;     // Pixels per frame
    int gridX;       // Grid position X
    int gridY;       // Grid position Y
    int score;       // Player score
    int lives;       // Player lives
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
extern float deathAnimTimer;      // Timer for death animation
extern float blinkTimer;          // Timer for blinking animations (power pellets)
extern float ghostEatenTimer;     // Timer for ghost eaten animation
extern int deathAnimFrame;        // Current frame of death animation
extern bool isResetting;          // Flag to indicate if game is resetting
extern int level;                 // Current level

extern int initialPelletCount;    // Total number of pellets at the start
extern int remainingPelletCount;  // Number of pellets remaining
extern int pelletsEaten;          // Number of pellets eaten in the current level
extern int powerPelletsEaten;     // Number of power pellets eaten in the current level
extern float powerPelletTimer;    // Timer to track power pellet duration

extern int eatenGhostCount;       // Number of ghosts eaten in current power pellet
extern int eatenGhostIndex;       // Index of the ghost being animated
extern int totalGhostsEaten;      // Total ghosts eanten in the current level

extern Fruit fruit;               // Bonus fruit
extern int totalFruitsCollected;  // Total number of fruits collected across levels

extern int maze[MAZE_HEIGHT][MAZE_WIDTH];
extern Player pacman;
extern Ghost ghosts[MAX_GHOSTS];

extern HighScore highscores[MAX_HIGH_SCORES];       // High score array

// Sound effects
extern Sound sfx_menu;           // Background sound for menu
extern Sound sfx_menu_nav;       // Played when navigating to menu options
extern Sound sfx_ready;          // Played during READY state
extern Sound sfx_pacman_move;    // Played when Pac-Man is moving
extern Sound sfx_pacman_chomp;   // Waka waka sound when eating pellets
extern Sound sfx_pacman_death;   // Played during death animation
extern Sound sfx_eat_fruit;      // Played when eating fruit
extern Sound sfx_eat_ghost;      // Played when eating a ghost
extern Sound sfx_ghost_frightened;// Played when ghosts are frightened
extern Sound sfx_level_complete; // Played when entering STATE_LEVEL_COMPLETE
extern Sound sfx_extra_life;     // Played when gaining an extra life

// Sound playback state
extern bool isFrightenedSoundPaused; // Tracks if ghost frightened sound

// Function Declarations

// game.c

// Initializes the maze array from a static layout, setting walls, pellets, and power pellets.
void init_maze(void);

// Finds Pac-Man's starting position in the maze layout ('P' character).
// Parameters:
//   startX - Pointer to store the starting X grid position.
//   startY - Pointer to store the starting Y grid position.
void find_pacman_start(int *startX, int *startY);

// Resets game state, including Pac-Man, ghosts, and timers, with optional full reset.
// Parameters:
//   fullReset - If true, clears pellet counts and maze; if false, preserves them.
void reset_game_state(bool fullReset);

// Checks if all pellets and power pellets have been collected, indicating level completion.
// Returns: true if maze is cleared, false otherwise.
bool is_maze_cleared(void);

// Updates the count of remaining pellets after Pac-Man collects one.
void update_pellet_count(void);

// Initializes the fruit's state, setting it as inactive with default position (14, 19).
void init_fruit(void);

// Updates the fruit's state, handling spawning (at 70 and 140 pellets eaten) and collection.
void update_fruit(void);

// Loads high scores from highscores.txt, using defaults if the file is missing.
void load_high_scores(void);

// Saves high scores to highscores.txt.
void save_high_scores(void);

// Checks if the player's score qualifies for the high score list and updates it if necessary.
// Parameters:
//   score - The player's score to check against the high score list.
void check_and_update_high_scores(int score);

// pacman_movement.c

// Initializes Pac-Man's starting position, speed, score, lives, and direction.
// Parameters:
//   startX - Starting X grid position.
//   startY - Starting Y grid position.
void init_pacman(int startX, int startY);

// Updates Pac-Man's movement, direction changes, pellet collection, and tunnel teleportation.
void update_pacman(void);

// ghost_ai.c

// Initializes ghosts' positions, speeds, states, and scatter targets.
void init_ghosts(void);

// Updates ghosts' movement, behavior (chase/scatter), and collisions with Pac-Man.
void update_ghosts(void);

// Manages the chase/scatter cycle based on level timers.
void update_ghost_mode(void);

#endif // GAME_LOGIC_H