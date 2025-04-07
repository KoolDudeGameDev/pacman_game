/*
Easy: Simple maze, slower Pac-Man speed.
Medium: More walls, medium speed.
Hard: Complex maze, faster speed.
*/

#include "raylib.h"
#include "game_logic.h"

int maze[MAZE_HEIGHT][MAZE_WIDTH];
Player pacman;
GameState gameState = STATE_KOOLDUDE_LOGO; // Initial state
Difficulty selectedDifficulty = DIFFICULTY_EASY; // Default difficulty

// Function Definitions
// --------------------------------------------------------------------------------------------------------------------------

// Initialize maze
void init_maze(void) {
    // Clear maze
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            maze[y][x] = EMPTY;
        }
    }

    //Define maze layouts for each difficulty
    int easy_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
        {1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
        {1, 2, 1, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 2, 1},
        {1, 2, 2, 2, 3, 2, 2, 1, 2, 1},
        {1, 2, 1, 1, 2, 1, 2, 1, 2, 1},
        {1, 2, 1, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int medium_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 2, 2, 1, 2, 2, 1, 2, 2, 1},
        {1, 2, 1, 1, 2, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 3, 2, 2, 1, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 2, 1},
        {1, 2, 2, 1, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 2, 1, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int hard_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 2, 1, 2, 1, 2, 1, 2, 2, 1},
        {1, 2, 1, 1, 2, 1, 1, 1, 2, 1},
        {1, 2, 2, 1, 2, 2, 2, 1, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 3, 1, 2, 1, 2, 1},
        {1, 2, 1, 1, 2, 1, 2, 1, 2, 1},
        {1, 2, 1, 2, 1, 2, 1, 2, 2, 1},
        {1, 2, 2, 1, 2, 1, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Copy appropriate maze based on difficulty
    int (*selected_maze)[MAZE_WIDTH] = easy_maze; // Default to easy
    if (selectedDifficulty == DIFFICULTY_MEDIUM) {
        selected_maze = medium_maze;
    } else if (selectedDifficulty == DIFFICULTY_HARD) {
        selected_maze = hard_maze;
    }

    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            maze[y][x] = selected_maze [y][x];
        }
    }
}

// Initialize Pac-Man with difficulty-based speed
// --------------------------------------------------------------------------------------------------------------------------

void init_pacman(void) {
    pacman.x = 1;
    pacman.y = 1;
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
            break;
    }
}

// Update position and handle pellet collection
void update_pacman(void) {
    int newX = pacman.x;
    int newY = pacman.y;

    // Handle input
    if (IsKeyDown(KEY_RIGHT)) newX ++;
    if (IsKeyDown(KEY_LEFT)) newX --;
    if (IsKeyDown(KEY_UP)) newY --;
    if (IsKeyDown(KEY_DOWN)) newY ++;

    // Check wall collision
    if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && maze[newX][newY] != WALL) {
        pacman.x = newX;
        pacman.y = newY;

        // Check pellet collision
        if (maze[newX][newY] == PELLET) {
            pacman.score += 10;
            maze[newX][newY] = EMPTY;
        } else if (maze[newX][newY == POWER_PELLET]) {
            pacman.score += 50;
            maze[newX][newY] = EMPTY;
            // Power pellet effect for ghosts

        }
    }
}

