#include "game_logic.h"

#include <stdio.h>
#include <string.h>

// Global Variables
// In game.c
Texture2D spriteSheet;
GameState gameState = STATE_MENU;       // Initial state
GhostMode ghostMode = MODE_SCATTER;     // Start in Scatter mode
float modeTimer = 0.0f;                 // Timer for switching between Chase and Scatter
float readyTimer = 0.0f;                // Timer for "READY!" phase
float deathAnimTimer = 0.0f;            // Timer for death animation
float blinkTimer = 0.0f;                // Timer for blinking animation
float ghostEatenTimer = 0.0f;           // Timer for ghost eaten
int deathAnimFrame = 0;                 // Current frame of death animation
bool isResetting = false;               // Flag to indicate if the game is resetting
int level = 1;                          // Start at level 1

int initialPelletCount = 0;             // Total number of pellets at the start
int remainingPelletCount = 0;           // Number of pellets remaining
int pelletsEaten = 0;                   // Number of pellets eaten in the current level

int eatenGhostCount = 0;                // Number of ghosts eaten in current power pellet
int eatenGhostIndex = -1;               // Index of the ghost being animated
float powerPelletTimer;                 // Timer to track power pellet duration

Fruit fruit;                            // Bonus fruit
int totalFruitsCollected = 0;           // Total number of fruits collected across levels

HighScore highscores[MAX_HIGH_SCORES];  // High scores array

// Sound effects
Sound sfx_menu;           // Background sound for menu
Sound sfx_menu_nav;       // Played when navigating to menu options
Sound sfx_ready;          // Played during READY state
Sound sfx_pacman_move;    // Played when Pac-Man is moving
Sound sfx_pacman_chomp;   // Waka waka sound when eating pellets
Sound sfx_pacman_death;   // Played during death animation
Sound sfx_eat_fruit;      // Played when eating fruit
Sound sfx_eat_ghost;      // Played when eating a ghost
Sound sfx_ghost_frightened;// Played when ghosts are frightened
Sound sfx_intermission;   // Played during level complete
Sound sfx_extra_life;     // Played when gaining an extra life

bool isFrightenedSoundPaused = false;
// Maze array
int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;
Ghost ghosts[MAX_GHOSTS];

// Maze Layout

/*  No. of pellets = 240
    No. of power pellets = 4
*/

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

// Load high scores from file
void load_high_scores(void) {
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        highscores[i].score = 0;
        strcpy(highscores[i].name, "AAA");
    }

    FILE *file = fopen("highscores.txt", "r");
    if (file) {
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            if (fscanf(file, "%3s %d\n", highscores[i].name, &highscores[i].score) != 2) {
                break;
            }
        }
        fclose(file);
    } else {
        printf("Warning: Could not open highscores.txt for reading. Using default scores.\n");
    }
}

// Save high scores to file
void save_high_scores(void) {
    FILE *file = fopen("highscores.txt", "w");
    if (file) {
        for (int i = 0; i < MAX_HIGH_SCORES; i ++) {
            fprintf(file, "%s %d\n", highscores[i].name, highscores[i].score);
        }
        fclose(file);
    } else {
        printf("Error: Could not open highscores.txt for writing.\n");
    }
}

// Checl and update high scores
void check_and_update_high_scores(int score) {
    int insertIndex = -1;
    for (int i = 0; i < MAX_HIGH_SCORES; i ++) {
        if (score > highscores[i].score) {
            insertIndex = i;
            break;
        }
    }

    if (insertIndex >= 0) {
        // Shift lower scores down
        for (int i = MAX_HIGH_SCORES - 1; i > insertIndex; i --) {
            highscores[i] = highscores[i - 1];
        }
        // Insert new score
        highscores[insertIndex].score = score;
        strcpy(highscores[insertIndex].name, "NEW");       // PLace holderr
    }
}



// Initialize maze
// --------------------------------------------------------------------------------------------------------------------------

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
    init_fruit();
    readyTimer = 4.0f;          // Show "READY!" for 4 secs
    gameState = STATE_READY;
    isResetting = true;
    deathAnimFrame = 0;
    eatenGhostCount = 0;
    eatenGhostIndex = -1;
    powerPelletTimer = 0.0f;
    PlaySound(sfx_ready);
}

// Check if the maze is cleared (no pellets or power pellets remain)
bool is_maze_cleared(void) {
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            if (maze[y][x] == PELLET || maze[y][x] == POWER_PELLET) {
                return false;    // Found a pellet or power pellet, maze not cleared
            }
        }
    }
    PlaySound(sfx_intermission); // Play intermission SFX when maze is cleared
    return true;                 // No pellets or power pellets found, maze is cleared
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
    fruit.gridX = 14;   // Center of the maze, near Pac-Man's start (row 19, col 14)
    fruit.gridY = 19;
    fruit.points = 100;
}

// Update fruit state
void update_fruit(void) {
    float deltaTime = GetFrameTime();

    // Check if fruit should spawn (after 70 or 170 pellets eaten)
    if (!fruit.active && (pelletsEaten == 70 || pelletsEaten == 170)) {
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
            PlaySound(sfx_eat_fruit);
        }
    }
}