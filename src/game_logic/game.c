#include "raylib.h"
#include "game_logic.h"

// Game State
GameState gameState = STATE_MENU; // Initial state

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
    "#.#....#####.##.#####....#.#",
    "#....#.#####.##.#####.#....#",
    "######.##..........##.######",
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