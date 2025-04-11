#include "raylib.h"
#include "game_logic.h"

#include <stdio.h>

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
    "#O...#................#...O#",
    "####.#.##.########.##.#.####",
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
    "#O...#................#...O#",
    "####.#.##.########.##.#.####",
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
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (medium_maze[y][x] == 'P') { // Check the medium_maze to find 'P'
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
        pacman.x = 14; // Center of the maze (28/2)
        pacman.y = 23; // Default starting position
    }

    pacman.score = 0;
    pacman.lives = 3;
    pacman.direction = DIR_RIGHT; // Start moving right (as in the original)
    pacman.nextDirection = DIR_RIGHT;

    // Set speed in tiles per frame to match the original Pac-Man
    // Original: 8 tiles/sec at 60 FPS = 0.133 tiles/frame (80% speed)
    // Adjust for difficulty: Easy (60%), Medium (80%), Hard (100%)
    switch (selectedDifficulty) {
        case DIFFICULTY_EASY:
            pacman.speed = (float)0.080; // Hardcoded: 0.133 * 0.6
            break;
        case DIFFICULTY_MEDIUM:
            pacman.speed = (float)0.106; // Hardcoded: 0.133 * 0.8
            break;
        case DIFFICULTY_HARD:
            pacman.speed = (float)0.133; // Hardcoded: 0.133 * 1.0
            break;
        default:
            pacman.speed = (float)0.106; // Default to medium speed
            break;
    }

    // Ensure speed is non-zero
    if (pacman.speed <= 0.0f) {
        pacman.speed = (float)0.106; // Fallback to medium speed
    }
}

// Update position and handle pellet collection
void update_pacman(void) {
    static float moveTimer = 0.0f;
    moveTimer += pacman.speed; // Accumulate movement based on speed (tiles/frame)

    // Queue the next direction based on player input
    if (IsKeyPressed(KEY_RIGHT)) pacman.nextDirection = DIR_RIGHT;
    if (IsKeyPressed(KEY_LEFT)) pacman.nextDirection = DIR_LEFT;
    if (IsKeyPressed(KEY_UP)) pacman.nextDirection = DIR_UP;
    if (IsKeyPressed(KEY_DOWN)) pacman.nextDirection = DIR_DOWN;

    // Only move when the timer allows (i.e., enough time has passed to move one tile)
    if (moveTimer >= 1.0f) {
        // First, try to move in the next direction (if it's different from the current direction)
        int newX = pacman.x;
        int newY = pacman.y;
        bool canMoveNext = false;

        if (pacman.nextDirection != pacman.direction) {
            switch (pacman.nextDirection) {
                case DIR_UP:
                    newY--;
                    break;
                case DIR_DOWN:
                    newY++;
                    break;
                case DIR_LEFT:
                    newX--;
                    break;
                case DIR_RIGHT:
                    newX++;
                    break;
                default:
                    break;
            }

            // Check if Pac-Man can move in the next direction
            if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && maze[newY][newX] != WALL) {
                pacman.x = newX;
                pacman.y = newY;
                pacman.direction = pacman.nextDirection; // Update current direction
                canMoveNext = true;
            }
        }

        // If Pac-Man couldn't move in the next direction, continue in the current direction
        if (!canMoveNext) {
            newX = pacman.x;
            newY = pacman.y;
            switch (pacman.direction) {
                case DIR_UP:
                    newY--;
                    break;
                case DIR_DOWN:
                    newY++;
                    break;
                case DIR_LEFT:
                    newX--;
                    break;
                case DIR_RIGHT:
                    newX++;
                    break;
                default:
                    break;
            }

            // Check if Pac-Man can move in the current direction
            if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && maze[newY][newX] != WALL) {
                pacman.x = newX;
                pacman.y = newY;
            }
        }

        // Handle pellet collection
        if (maze[pacman.y][pacman.x] == PELLET) {
            pacman.score += 10;
            maze[pacman.y][pacman.x] = EMPTY;
        } else if (maze[pacman.y][pacman.x] == POWER_PELLET) {
            pacman.score += 50;
            maze[pacman.y][pacman.x] = EMPTY;
        }

        moveTimer -= 1.0f; // Reset the timer (carry over any excess for smoother movement)
    }
}