#include "game_logic.h"

// Game State
GameState gameState = STATE_MENU;       // Initial state
GhostMode ghostMode = MODE_SCATTER;     // Start in Scatter mode
float modeTimer = 0.0f;                 // Timer for switching between Chase and Scatter
float readyTimer = 0.0f;                // Timer for "READY!" phase
bool isResetting = false;               // Flag to indicate if the game is resetting

// Maze array
int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;
Ghost ghosts[MAX_GHOSTS];

// Maze Layout
static char game_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####O#",
    "#O####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###||### ##.#     ",
    "######.## #      # ##.######",
    "      .   #      #   .      ",
    "      .   ########   .      ",
    "######.##          ##.######",
    "#......## ######## ##......#",
    "#.####.......##.......####.#",
    "#......#####.##.#####.... .#",
    "######.#####.##.#####.######",
    "     #.##..........##.#     ",
    "     #.##.########.##.#     ",
    "     #.##.....P....##.#     ",
    "######.##.########.##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#O...#................#...O#",
    "####.#.##.########.##.#.####",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

// Function Definitions
// --------------------------------------------------------------------------------------------------------------------------

// Initialize maze

void init_maze(void) {

    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            switch (game_maze[y][x]) {
            case '#':
                maze[y][x] = WALL;
                break;
            case '.':
                maze[y][x] = PELLET;
                break;
            case 'O':
                maze[y][x] = POWER_PELLET;
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
    init_pacman(startX, startY);
    pacman.score = score;       // Restore score
    init_ghosts();
    readyTimer = 3.0f;          // Shpw "READY!" for 3 secs
    gameState = STATE_READY;
    isResetting = true;
}