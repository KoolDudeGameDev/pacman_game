#include "raylib.h"
#include "game_logic.h"

// Game State
GameState gameState = STATE_MENU; // Initial state
Difficulty selectedDifficulty = DIFFICULTY_EASY; // Default difficulty

// Maze array
int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;

// Define maze layouts for each difficulty

/*
Easy: Simple maze, slower Pac-Man speed.
Medium: More walls, medium speed.
Hard: Complex maze, faster speed.
*/

static char easy_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
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
    "#O...#..............#.#...O#",
    "#### #.##.########.##.#.####",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

static char medium_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
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
    "#O...#..............#.#...O#",
    "#### #.##.########.##.#.####",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

static char hard_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
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
    "#O...#..............#.#...O#",
    "#### #.##.########.##.#.####",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

// Function Definitions
// --------------------------------------------------------------------------------------------------------------------------

// Initialize maze

void init_maze(void) {

    // Copy appropriate maze based on difficulty
    char (*selected_maze)[MAZE_WIDTH] = easy_maze; // Default to easy
    if (selectedDifficulty == DIFFICULTY_MEDIUM) {
        selected_maze = medium_maze;
    } else if (selectedDifficulty == DIFFICULTY_HARD) {
        selected_maze = hard_maze;
    }

    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            switch (selected_maze[y][x]) {
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

// Initialize Pac-Man with difficulty-based speed
// --------------------------------------------------------------------------------------------------------------------------

void init_pacman(void) {
    // Find Pac-Man's starting position (marked as 'P' in the maze)
    bool found = false;
    char (*selected_maze)[MAZE_WIDTH] = easy_maze; // Default to easy
    if (selectedDifficulty == DIFFICULTY_MEDIUM) {
        selected_maze = medium_maze;
    } else if (selectedDifficulty == DIFFICULTY_HARD) {
        selected_maze = hard_maze;
    }

    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            if (selected_maze[y][x] == 'P') {
                pacman.x = x;
                pacman.y = y;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        // Fallback position if 'P' is not found
        pacman.x = 14;      // Center of the maze (28/2)
        pacman.y = 23;      // Default starting position
    }

    pacman.score = 0;
    pacman.lives = 3;

    // Set speed based on difficulty
    switch (selectedDifficulty) {
        case DIFFICULTY_EASY:
            pacman.speed = 2;
            break;
        case DIFFICULTY_MEDIUM:
            pacman.speed = 3;
            break;
        case DIFFICULTY_HARD:
            pacman.speed = 4;
            break;
        default:
            pacman.speed = 3;
            break;
    }
}

// Update position and handle pellet collection
void update_pacman(void) {
    static float moveTimer = 0.0f;
    moveTimer += GetFrameTime() * pacman.speed * 10;
    
    if (moveTimer >= 1.0f) {
        int newX = pacman.x;
        int newY = pacman.y;

        if (IsKeyDown(KEY_RIGHT)) newX++;
        if (IsKeyDown(KEY_LEFT)) newX--;
        if (IsKeyDown(KEY_UP)) newY--;
        if (IsKeyDown(KEY_DOWN)) newY++;

        if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && maze[newY][newX] != WALL) {
            pacman.x = newX;
            pacman.y = newY;

            if (maze[newY][newX] == PELLET) {
                pacman.score += 10;
                maze[newY][newX] = EMPTY;
            } else if (maze[newY][newX] == POWER_PELLET) {
                pacman.score += 50;
                maze[newY][newX] = EMPTY;
            }
        }
        moveTimer = 0.0f;
    }   
}
