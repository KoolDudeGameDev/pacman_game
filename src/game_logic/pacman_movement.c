#include "game_logic.h"

// Initialize Pac-Man with difficulty-based speed
// --------------------------------------------------------------------------------------------------------------------------
void init_pacman(int startX, int startY) {
    pacman.gridX = startX;
    pacman.gridY = startY;
    pacman.x = startX * TILE_SIZE + TILE_SIZE / 2.0f;
    pacman.y = startY * TILE_SIZE + TILE_SIZE / 2.0f;

    pacman.speed = 120.0f; // Pixels per second (approx 6 tiles/sec)
    pacman.score = 0;
    pacman.lives = 3;
    pacman.direction = DIR_RIGHT;
    pacman.nextDirection = DIR_RIGHT;
}

// Update position and handle pellet collection
void update_pacman(void) {
    float deltaTime = GetFrameTime();

    // Queue the next direction
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) pacman.nextDirection = DIR_RIGHT;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) pacman.nextDirection = DIR_LEFT;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) pacman.nextDirection = DIR_UP;
    if (IsKeyDown(KEY_DOWN) ||  IsKeyDown(KEY_S)) pacman.nextDirection = DIR_DOWN;

    // Check if Pac-Man is centered in the current tile
    float centerX = pacman.gridX * TILE_SIZE + TILE_SIZE / 2.0f;
    float centerY = pacman.gridY * TILE_SIZE + TILE_SIZE / 2.0f;
    bool atCenter = fabs(pacman.x - centerX) < 1.0f && fabs(pacman.y - centerY) < 1.0f;

    if (atCenter) {
        // Try to change direction
        int newGridX = pacman.gridX;
        int newGridY = pacman.gridY;
        switch (pacman.nextDirection) {
            case DIR_UP:
                newGridY--;
                break;
            case DIR_DOWN:
                newGridY++;
                break;
            case DIR_LEFT:
                newGridX--;
                break;
            case DIR_RIGHT:
                newGridX++;
                break;
            default:
                break;
        }

        // Check if the next direction is valid
        if (newGridX >= 0 && newGridX < MAZE_WIDTH && newGridY >= 0 && newGridY < MAZE_HEIGHT &&
            maze[newGridY][newGridX] != WALL) {
            pacman.direction = pacman.nextDirection;
            pacman.gridX = newGridX;
            pacman.gridY = newGridY;
        } else {
            // Try to continue in current direction
            newGridX = pacman.gridX;
            newGridY = pacman.gridY;
            switch (pacman.direction) {
                case DIR_UP:
                    newGridY--;
                    break;
                case DIR_DOWN:
                    newGridY++;
                    break;
                case DIR_LEFT:
                    newGridX--;
                    break;
                case DIR_RIGHT:
                    newGridX++;
                    break;
                default:
                    break;
            }

            if (newGridX >= 0 && newGridX < MAZE_WIDTH && newGridY >= 0 && newGridY < MAZE_HEIGHT &&
                maze[newGridY][newGridX] != WALL) {
                pacman.gridX = newGridX;
                pacman.gridY = newGridY;
            } else {
                pacman.direction = DIR_NONE; // Stop if blocked
            }
        }
    }

    // Handle tunnel (rows 12 and 13, columns 0 and 27)
    if ((pacman.gridY == 12 || pacman.gridY == 13)) {
        if (pacman.gridX <= 0 && pacman.direction == DIR_LEFT) {
            pacman.gridX = MAZE_WIDTH - 1;      // Teleport to right side
            pacman.x = pacman.gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        } else if (pacman.gridX >= MAZE_WIDTH -1 && pacman.direction == DIR_RIGHT) {
            pacman.gridX = 0;                   // Teleport to left side
            pacman.x = pacman.gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        }
    }

    // Move Pac-Man
    switch (pacman.direction) {
        case DIR_UP:
            pacman.y -= pacman.speed * deltaTime;
            break;
        case DIR_DOWN:
            pacman.y += pacman.speed * deltaTime;
            break;
        case DIR_LEFT:
            pacman.x -= pacman.speed * deltaTime;
            break;
        case DIR_RIGHT:
            pacman.x += pacman.speed * deltaTime;
            break;
        default:
            break;
    }

    // Update grid position
    pacman.gridX = (int)(pacman.x / TILE_SIZE);
    pacman.gridY = (int)(pacman.y / TILE_SIZE);

    // Handle pellet collection
    if (maze[pacman.gridY][pacman.gridX] == PELLET) {
        maze[pacman.gridY][pacman.gridX] = EMPTY;
        pacman.score += 10;
        pelletsEaten ++;
        update_pellet_count();
        if (is_maze_cleared()) {        // Check if maze is cleared after collecting pellet
            level ++;                   // Increment level
            gameState = STATE_LEVEL_COMPLETE;
            return;
        }
    } else if (maze[pacman.gridY][pacman.gridX] == POWER_PELLET) {
        maze[pacman.gridY][pacman.gridX] = EMPTY;
        pacman.score += 50;
        pelletsEaten ++;
        update_pellet_count();
        eatenGhostCount = 0;
        // Make ghosts frightened and synchronize their timers
        for (int i = 0; i < MAX_GHOSTS; i++) {
            if (ghosts[i].state == GHOST_NORMAL || ghosts[i].state == GHOST_FRIGHTENED) {
                ghosts[i].state = GHOST_FRIGHTENED;
                ghosts[i].stateTimer = 10.0f; // Frightened for 10 seconds
                ghosts[i].frightenedBlinkTimer = 0.0f;
            }
        }
        powerPelletTimer = 10.0f;
        if (is_maze_cleared()) {       
            level ++;                   
            gameState = STATE_LEVEL_COMPLETE;     
            return;
        }
    }
}