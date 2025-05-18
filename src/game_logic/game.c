#include "game_logic.h"

// Global Variables

Texture2D spriteSheet;
GameState gameState = STATE_KOOLDUDE_LOGO;       // Initial state
GhostMode ghostMode = MODE_SCATTER;       // Start in Scatter mode
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
int powerPelletsEaten = 0;              // Number of power pellets eaten in the current level
float powerPelletTimer;                 // Timer to track power pellet duration

int eatenGhostCount = 0;                // Number of ghosts eaten in current power pellet
int eatenGhostIndex = -1;               // Index of the ghost being animated
int totalGhostsEaten;                   // Total ghosts eanten in the current level

Fruit fruit;                            // Bonus fruit
int totalFruitsCollected = 0;           // Total number of fruits collected across levels

HighScore highscores[MAX_HIGH_SCORES];  // High scores array
char playerNameInput[4] = "AAA";        // Default to "AAA"
int nameInputIndex = 0;
bool nameInputComplete = false;
bool saveHighScoreFailed = false;

const char* gameOverMessages[] = {
    "Better Luck Next Time!",
    "Ghosts Got You Down?",
    "Try Again, Champ!",
    "Keep Chasing Those Pellets!",
    "Don't Let Blinky Win!"
};
int selectedMessageIndex = 0;

PauseMenuState pauseMenuState = PAUSE_MENU_MAIN;
int pauseSelectedOption = 0;
float bgMusicVolume = 0.3f;          // Default full volume
float sfxVolume = 1.0f;              // Default full volume
bool soundMuted = false;             // Default unmuted
bool playPacmanMove = false;         // Flag to control sfx_pacman_move playback

// Sound effects
Sound sfx_menu;             // Background sound for menu
Sound sfx_menu_nav;         // Played when navigating to menu options
Sound sfx_ready;            // Played during READY state
Sound sfx_pacman_move;      // Played when Pac-Man is moving
Sound sfx_pacman_chomp;     // Waka waka sound when eating pellets
Sound sfx_pacman_death;     // Played during death animation
Sound sfx_eat_fruit;        // Played when eating fruit
Sound sfx_eat_ghost;        // Played when eating a ghost
Sound sfx_ghost_frightened; // Played when ghosts are frightened
Sound sfx_level_complete;   // Played when entering STATE_LEVEL_COMPLETE
Sound sfx_extra_life;       // Played when gaining an extra life
Sound sfx_game_over;        // Game over sound effects

bool isFrightenedSoundPaused = false;   //Tracks whether the ghost frightened sound is paused during STATE_GHOST_EATEN

// Define game maze and main entities
int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;
Ghost ghosts[MAX_GHOSTS];

// Maze Layout

/*  
    31 x 28 array
    No. of pellets = 240
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

// Game over message selection
void select_game_over_message(void) {
    selectedMessageIndex = GetRandomValue(0, 4); // 5 messages
}

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
        strncpy(highscores[insertIndex].name, playerNameInput, MAX_NAME_LENGTH);
        highscores[insertIndex].name[MAX_NAME_LENGTH - 1] = '\0';
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

void reset_game_state(bool fullReset, GameState targetState) {
    int startX, startY;
    find_pacman_start(&startX, &startY);
    int score = pacman.score;   // Preserve score unless full reset
    int lives = pacman.lives;   // Preserve lives unless full reset
    init_pacman(startX, startY);

    if (fullReset) {
        pacman.score = 0;
        pacman.lives = 3;
    } else {
        pacman.score = score;   // Restore score
        pacman.lives = lives;   // Restore lives
    }

    init_ghosts();
    init_fruit();
    readyTimer = 4.0f;          // Show "READY!" for 4 secs (only used if targetState is STATE_READY)
    gameState = targetState;    // Set to specified target state
    isResetting = true;
    deathAnimFrame = 0;
    deathAnimTimer = 6.0f;
    eatenGhostCount = 0;
    eatenGhostIndex = -1;
    powerPelletTimer = 0.0f;

    // Only reset pellet counts and ghost count for full reset (level completion or new game)
    if (fullReset) {
        printf("Full reset: Clearing pelletsEaten, powerPelletsEaten, eatenGhostCount\n");
        pelletsEaten = 0;
        powerPelletsEaten = 0;  
        eatenGhostCount = 0;        // Reset total ghosts eaten during power pellet effect
        totalGhostsEaten = 0;       // Reset total ghosts eaten
        totalFruitsCollected = 0;   // Reset fruits if desired per level

        strcpy(playerNameInput, "AAA");
        nameInputIndex = 0;
        nameInputComplete = false;
    }

    // Recount pellets for the new level state
    remainingPelletCount = 0;
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] == PELLET || maze[y][x] == POWER_PELLET) {
                remainingPelletCount++;
            }
        }
    }

    if (IsSoundPlaying(sfx_ghost_frightened)) {
        StopSound(sfx_ghost_frightened);
    }
    isFrightenedSoundPaused = false;
    playPacmanMove = false;
    StopSound(sfx_pacman_move);
    if (targetState == STATE_READY && !soundMuted) {
        SetSoundVolume(sfx_ready, sfxVolume);
        PlaySound(sfx_ready);
    }
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
    remainingPelletCount = 0;

    if (!soundMuted) {
        SetSoundVolume(sfx_level_complete, sfxVolume);
        PlaySound(sfx_level_complete);
    }

    return true;                    // No pellets or power pellets found, maze is cleared
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

    // Check if fruit should spawn (after 70, 140 pellets eaten)
    if (!fruit.active && (pelletsEaten == 70 || pelletsEaten == 140)) {
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
            if (!soundMuted) {
                SetSoundVolume(sfx_eat_fruit, sfxVolume);
                PlaySound(sfx_eat_fruit);
            }
        }
    }
}