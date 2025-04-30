#include "game_logic.h"

// Global Variables
// In game.c
Texture2D spriteSheet;
GameState gameState = STATE_MENU;       // Initial state
GhostMode ghostMode = MODE_SCATTER;     // Start in Scatter mode
float modeTimer = 0.0f;                 // Timer for switching between Chase and Scatter
float readyTimer = 0.0f;                // Timer for "READY!" phase
float deathAnimTimer = 0.0f;            // Timer for death animation
float blinkTimer = 0.0f;                // Timer for blinking animation
int deathAnimFrame = 0;                 // Current frame of death animation
bool isResetting = false;               // Flag to indicate if the game is resetting
int level = 1;                          // Start at level 1

int initialPelletCount = 0;             // Total number of pellets at the start
int remainingPelletCount = 0;           // Number of pellets remaining
int pelletsEaten = 0;                   // Number of pellets eaten in the current level

Fruit fruit;                            // Bonus fruit
int totalFruitsCollected = 0;           // Total number of fruits collected across levels

// Maze array
int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;
Ghost ghosts[MAX_GHOSTS];

// Maze Layout
static char game_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    "############################",     //  0     
    "#............##............#",     //  1
    "#.####.#####.##.#####.####O#",     //  2
    "#O####.#####.##.#####.####.#",     //  3
    "#..........................#",     //  4
    "#.####.##.########.##.####.#",     //  5
    "#......##....##....##......#",     //  6
    "######.##### ## #####.######",     //  7
    "     #.##### ## #####.#     ",     //  8
    "     #.##          ##.#     ",     //  9
    "     #.## ###||### ##.#     ",     //  10
    "######.## #      # ##.######",     //  11
    "      .   #      #   .      ",     //  12
    "      ... ######## ...      ",     //  13
    "######.##          ##.######",     //  14
    "#......## ######## ##......#",     //  15
    "#.####.......##.......####.#",     //  16
    "#......#####.##.#####......#",     //  17
    "######.#####.##.#####.######",     //  18
    "     #.##..........##.#     ",     //  19
    "     #.##.########.##.#     ",     //  20
    "     #.##.....P....##.#     ",     //  21
    "######.##.########.##.######",     //  22
    "#............##............#",     //  23
    "#.####.#####.##.#####.####.#",     //  24
    "#O...#................#...O#",     //  25
    "####.#.##.########.##.#.####",     //  26
    "#......##....##....##......#",     //  27
    "#.##########.##.##########.#",     //  28
    "#..........................#",     //  29
    "############################"      //  30
};

// Function Definitions
// --------------------------------------------------------------------------------------------------------------------------

// Initialize maze

void init_maze(void) {
    initialPelletCount = 0;
    remainingPelletCount = 0;

    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            switch (game_maze[y][x]) {
            case '#':
                maze[y][x] = WALL;
                break;
            case '.':
                maze[y][x] = PELLET;
                initialPelletCount ++;
                remainingPelletCount ++;
                break;
            case 'O':
                maze[y][x] = POWER_PELLET;
                initialPelletCount ++;
                remainingPelletCount ++;
                break;
            case '|':
                maze[y][x] = GHOST_GATE;
                break;
            case 'P':
                maze[y][x] = EMPTY;  // Pac-Man's starting position will be set in init_pacman
                break;
            default:
                maze[y][x] = EMPTY;
                break;
            }
        }
    }
}

void find_pacman_start(int *startX, int *startY) {
    bool found = false;
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            if (game_maze[y][x] == 'P') {
                *startX = x;
                *startY = y;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        *startX = 14;
        *startY = 23;
    }
}

void reset_game_state(void) {
    int startX, startY;
    find_pacman_start(&startX, &startY);
    int score = pacman.score;   // Preserve score
    int lives = pacman.lives;   // Preserve lives
    init_pacman(startX, startY);
    pacman.score = score;       // Restore score
    pacman.lives = lives;       // Restore lives
    init_ghosts();
    readyTimer = 3.0f;          // Show "READY!" for 3 secs
    gameState = STATE_READY;
    isResetting = true;
    deathAnimFrame = 0;
}

// Check if the maze is cleared (no pellets or power pellets remain)
bool is_maze_cleared(void) {
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            if (maze[y][x] == PELLET || maze[y][x] == POWER_PELLET) {
                return false;  // Found a pellet or power pellet, maze not cleared
            }
        }
    }
    return true;               // No pellets or power pellets found, maze is cleared
}

// Update remaining pellet count (called from update_pacman)
void update_pellet_count(void) {
    remainingPelletCount = 0;
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            if (maze[y][x] == PELLET || maze[y][x] == POWER_PELLET) {
                remainingPelletCount ++;
            }
        }
    }
}

// Initialize fruit
void init_fruit(void) {
    fruit.active = false;
    fruit.timer = 0.0f;
    fruit.gridX = 14;   // Center of the maze, near Pac-Man's start (row 17, col 14)
    fruit.gridY = 17;
    fruit.points = 100;
}

// Update fruit state
void update_fruit(void) {
    float deltaTime = GetFrameTime();

    // Check if fruit should spawn (after 70 or 170 pellets eaten)
    if (!fruit.active && (pelletsEaten == 0 || pelletsEaten == 170)) {
        fruit.active = true;
        fruit.timer = 10.0f;    // Fruit only available for 10 secs
    }

    // Update fruit timer
    if (fruit.active) {
        fruit.timer -= deltaTime;
        if (fruit.timer <= 0.0f) {
            fruit.active = false;
            fruit.timer = 0.0f;
        }

        // Check for collection by Pac-Man
        if (pacman.gridX == fruit.gridX && pacman.gridY == fruit.gridY) {
            pacman.score += fruit.points;
            totalFruitsCollected ++;
            fruit.active = false;
            fruit.timer = 0.0f;
        }
    }
}