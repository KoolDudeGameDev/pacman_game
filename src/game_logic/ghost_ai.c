#include "raylib.h"
#include "game_logic.h"

#include <stdlib.h>
#include <math.h>

// Initialize Ghosts
void init_ghosts(void) {
    // Starting posistions near the ghost pen (center of maze) 
    int startPositions[4][2] = {
        {13, 11},   // Ghost 1
        {14, 11},   // Ghost 2
        {13, 12},   // Ghost 3
        {14, 12}    // Ghost 4
    };

    for (int i = 0; i < MAX_GHOSTS; i ++) {
        ghosts[i].gridX = startPositions[i][0];
        ghosts[i].gridY = startPositions[i][1];
        ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        ghosts[i].y = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        ghosts[i].speed = 100.0f; // Slightly slower than Pac_Man
        ghosts[i].direction = (Direction)(rand() % 4 + 1);
        ghosts[i].state = GHOST_NORMAL;
        ghosts[i].stateTimer = 0.0f;
    }
}

// Update Ghosts
void update_ghosts(void) {
    float deltaTime = GetFrameTime();

    for (int i = 0; i < MAX_GHOSTS; i++) {
        // Update frightened timer
        if (ghosts[i].state == GHOST_FRIGHTENED) {
            ghosts[i].stateTimer -= deltaTime;
            if (ghosts[i].stateTimer <= 0.0f) {
                ghosts[i].state = GHOST_NORMAL;
            }
        }

        // Move only when centered
        float centerX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        float centerY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        bool atCenter = fabs(ghosts[i].x - centerX) < 0.1f && fabs(ghosts[i].y - centerY) < 0.1f;

        if (atCenter) {
            // Simple AI: Chase Pac-Man in normal state, random in frightened
            Direction possibleDirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
            int validDirs[4] = {0};
            int validCount = 0;

            for (int d = 0; d < 4; d++) {
                int newGridX = ghosts[i].gridX;
                int newGridY = ghosts[i].gridY;
                switch (possibleDirs[d]) {
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
                    maze[newGridY][newGridX] != WALL && maze[newGridY][newGridX] != GHOST_GATE) {
                    validDirs[validCount] = d;
                    validCount++;
                }
            }

            if (validCount > 0) {
                int choice;
                if (ghosts[i].state == GHOST_FRIGHTENED) {
                    // Random direction
                    choice = validDirs[rand() % validCount];
                } else {
                    // Chase Pac-Man: Choose direction closest to Pac-Man
                    float minDist = 9999.0f;
                    int bestDir = -1;
                    for (int d = 0; d < validCount; d++) {
                        int newGridX = ghosts[i].gridX;
                        int newGridY = ghosts[i].gridY;
                        switch (possibleDirs[validDirs[d]]) {
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
                        float dist = sqrtf(powf(newGridX - pacman.gridX, 2) + powf(newGridY - pacman.gridY, 2));
                        if (dist < minDist) {
                            minDist = dist;
                            bestDir = validDirs[d];
                        }
                    }
                    choice = bestDir;
                }

                ghosts[i].direction = possibleDirs[choice];
                switch (ghosts[i].direction) {
                    case DIR_UP:
                        ghosts[i].gridY--;
                        break;
                    case DIR_DOWN:
                        ghosts[i].gridY++;
                        break;
                    case DIR_LEFT:
                        ghosts[i].gridX--;
                        break;
                    case DIR_RIGHT:
                        ghosts[i].gridX++;
                        break;
                    default:
                        break;
                }
            }
        }

        // Move ghost
        switch (ghosts[i].direction) {
            case DIR_UP:
                ghosts[i].y -= ghosts[i].speed * deltaTime;
                break;
            case DIR_DOWN:
                ghosts[i].y += ghosts[i].speed * deltaTime;
                break;
            case DIR_LEFT:
                ghosts[i].x -= ghosts[i].speed * deltaTime;
                break;
            case DIR_RIGHT:
                ghosts[i].x += ghosts[i].speed * deltaTime;
                break;
            default:
                break;
        }

        // Check collision with Pac-Man
        float dist = sqrtf(powf(ghosts[i].x - pacman.x, 2) + powf(ghosts[i].y - pacman.y, 2));
        if (dist < TILE_SIZE / 2.0f) {
            if (ghosts[i].state == GHOST_FRIGHTENED) {
                ghosts[i].state = GHOST_EATEN;
                pacman.score += 200;
                ghosts[i].gridX = 14;
                ghosts[i].gridY = 11;
                ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
                ghosts[i].y = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
                ghosts[i].state = GHOST_NORMAL;
            } else if (ghosts[i].state == GHOST_NORMAL) {
                pacman.lives--;
                if (pacman.lives <= 0) {
                    gameState = STATE_GAME_OVER;
                } else {
                    // Reset positions
                    init_pacman();
                    init_ghosts();
                }
            }
        }
    }
}