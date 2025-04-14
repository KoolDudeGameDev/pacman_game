#include "raylib.h"
#include "game_logic.h"

#include <stdlib.h>
#include <math.h>

// Function to choose best direction toward target
static Direction choose_best_direction(int currentX, int currentY, int targetX, int targetY, Direction currentDir, bool isExitingPen) {
    Direction possibleDirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    int validDirs[4] = {0};
    int validCount = 0;

    for (int d = 0; d < 4; d ++) {
        int newGridX = currentX;
        int newGridY = currentY;
        switch (possibleDirs[d]) {
            case DIR_UP:
                newGridY --;
                break;
            case DIR_DOWN:
                newGridY ++;
                break;
            case DIR_LEFT:
                newGridX --;
                break;
            case DIR_RIGHT:
                newGridX ++;
                break;
            default:
                break;
        }

        // Check if the direction is valid (not a wall, ghosts can pass through the gate when exiting)
        bool canPassGate = isExitingPen && newGridY == 11 && maze[newGridY][newGridX] == GHOST_GATE;
        if (newGridX >= 0 && newGridX < MAZE_WIDTH && newGridY >= 0 && newGridY < MAZE_HEIGHT) {
            if (maze[newGridY][newGridX] != WALL && (maze[newGridY][newGridX] != GHOST_GATE || canPassGate)) {
                // Avoid moving back in the opposite direction
                Direction oppositeDir = DIR_NONE;
                switch (currentDir) {
                    case DIR_UP: oppositeDir = DIR_DOWN; break;
                    case DIR_DOWN: oppositeDir = DIR_UP; break;
                    case DIR_LEFT: oppositeDir = DIR_RIGHT; break;
                    case DIR_RIGHT: oppositeDir = DIR_LEFT; break;
                    default: break;
                }
                if (possibleDirs[d] != oppositeDir || validCount == 0) {
                    validDirs[validCount] = d;
                    validCount++;
                }
            }
        }
    }

    if (validCount == 0) {
        return DIR_NONE; // No valid directions
    }

    // Choose the direction that gets closest to target
    float minDist = 9999.0f;
    int bestDir = -1;
    for (int d = 0; d < validCount; d++) {
        int newGridX = currentX;
        int newGridY = currentY;
        switch (possibleDirs[validDirs[d]]) {
            case DIR_UP:
                newGridY --;
                break;
            case DIR_DOWN:
                newGridY ++;
                break;
            case DIR_LEFT:
                newGridX --;
                break;
            case DIR_RIGHT:
                newGridX ++;
                break;
            default:
                break;
        }
        float dist = sqrtf(powf(newGridX - targetX, 2) + powf(newGridY - targetY, 2));
        if (dist < minDist) {
            minDist = dist;
            bestDir = validDirs[d];
        }
    }

    return bestDir >= 0 ? possibleDirs[bestDir] : DIR_NONE;
}

// Initialize Ghosts
// ----------------------------------------------------------------------------------------
void init_ghosts(void) {
    // Starting positions near the ghost pen (center of maze) 
    int startPositions[4][2] = {
        {13, 11},   // Ghost 0 (Blinky) - will start outside
        {14, 11},   // Ghost 1 (Pinky)
        {13, 12},   // Ghost 2 (Inky)
        {14, 12}    // Ghost 3 (Clyde)
    };

    // Scatter targets (corners of the maze)
    int scatterTargets[4][2] = {
        {MAZE_WIDTH - 2, 1},                // Blinky: Top-right
        {1, 1},                             // Pinky: Top-left
        {MAZE_WIDTH - 2, MAZE_HEIGHT - 2},  // Inky: Bottom-right
        {1, MAZE_HEIGHT - 2}                // Clyde: Bottom-left
    };

    for (int i = 0; i < MAX_GHOSTS; i++) {
        ghosts[i].gridX = startPositions[i][0];
        ghosts[i].gridY = startPositions[i][1];
        ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        ghosts[i].y = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        ghosts[i].speed = 100.0f;   // Slightly slower than Pac-Man
        ghosts[i].direction = DIR_UP;   // Exits pen
        ghosts[i].scatterTargetX = scatterTargets[i][0];
        ghosts[i].scatterTargetY = scatterTargets[i][1];
        ghosts[i].stateTimer = i * 5.0f;    // Staggered release times (0, 5, 10, 15 seconds)
        if (i == 0) {
            // Blinky starts outside the pen
            ghosts[i].state = GHOST_NORMAL;
            ghosts[i].gridX = 14;
            ghosts[i].gridY = 11 - 2; // Above the pen
            ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
            ghosts[i].y = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        } else {
            ghosts[i].state = GHOST_PENNED;
        }
    }
}

// Update Ghost Mode (Chase/Scatter switching)
// ----------------------------------------------------------------------------------------
void update_ghost_mode(void) {
    static float chaseTimes[] = {20.0f, 20.0f, 20.0f, 9999.0f};     // Chase durations
    static float scatterTimes[] = {7.0f, 7.0f, 5.0f, 5.0f};         // Scatter durations
    static int phase = 0;   // Current phase (0-3)

    modeTimer += GetFrameTime();

    float currentDuration = (ghostMode == MODE_CHASE) ? chaseTimes[phase] : scatterTimes[phase];
    if (modeTimer >= currentDuration) {
        modeTimer = 0.0f;
        ghostMode = (ghostMode == MODE_CHASE) ? MODE_SCATTER : MODE_CHASE;
        if (ghostMode == MODE_CHASE) {
            phase = (phase + 1) % 4;    // Moves to next phase
        }
    }
}

// Update Ghosts
// ----------------------------------------------------------------------------------------
void update_ghosts(void) {
    float deltaTime = GetFrameTime();

    // Update Chase/Scatter mode
    update_ghost_mode();

    for (int i = 0; i < MAX_GHOSTS; i++) {

        // Handle penned state
        if (ghosts[i].state == GHOST_PENNED) {
            ghosts[i].stateTimer -= deltaTime;
            if (ghosts[i].stateTimer <= 0.0f) {
                ghosts[i].state = GHOST_NORMAL;
                ghosts[i].direction = DIR_UP; // Move up to exit the pen
            }
            // Move up and down within the pen until released
            if (ghosts[i].y <= (11 * TILE_SIZE + TILE_SIZE / 2.0f)) {
                ghosts[i].direction = DIR_DOWN;
            } else if (ghosts[i].y >= (12 * TILE_SIZE + TILE_SIZE / 2.0f)) {
                ghosts[i].direction = DIR_UP;
            }

            switch (ghosts[i].direction) {
                case DIR_UP:
                    ghosts[i].y -= ghosts[i].speed * deltaTime;
                    break;
                case DIR_DOWN:
                    ghosts[i].y += ghosts[i].speed * deltaTime;
                    break;   
                default:
                    break;
            }
            continue;
        }

        // Update frightened timer
        if (ghosts[i].state == GHOST_FRIGHTENED) {
            ghosts[i].stateTimer -= deltaTime;
            if (ghosts[i].stateTimer <= 0.0f) {
                ghosts[i].state = GHOST_NORMAL;
            }
        }

        // Check if ghost is centered in the current tile
        float centerX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        float centerY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        bool atCenter = fabs(ghosts[i].x - centerX) < 1.0f && fabs(ghosts[i].y - centerY) < 1.0f;

        // Determine if the ghost is still exiting the pen (not yet above the gate)
        bool isExitingPen = (ghosts[i].gridY >= 11 && ghosts[i].gridY <= 12 && ghosts[i].state == GHOST_NORMAL);

        if (atCenter) {
            // Snap position to center to prevent drift
            ghosts[i].x = centerX;
            ghosts[i].y = centerY;

            // If still in the pen area, force move toward exit
            if (isExitingPen) {
                ghosts[i].direction = DIR_UP;
                ghosts[i].gridY--;
                continue;
            }

            // Determine target based on ghost type and mode
            int targetX, targetY;
            if (ghosts[i].state == GHOST_FRIGHTENED) {
                // Random movement: Choose a random valid direction
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

                    // Ensure the new position is within bounds and not a wall or ghost gate (unless exiting)
                    if (newGridX >= 0 && newGridX < MAZE_WIDTH && newGridY >= 0 && newGridY < MAZE_HEIGHT &&
                        (maze[newGridY][newGridX] != WALL && (maze[newGridY][newGridX] != GHOST_GATE))) {
                        // Avoid moving back in the opposite direction (prevents jittering)
                        Direction oppositeDir = DIR_NONE;
                        switch (ghosts[i].direction) {
                            case DIR_UP: oppositeDir = DIR_DOWN; break;
                            case DIR_DOWN: oppositeDir = DIR_UP; break;
                            case DIR_LEFT: oppositeDir = DIR_RIGHT; break;
                            case DIR_RIGHT: oppositeDir = DIR_LEFT; break;
                            default: break;
                        }
                        if (possibleDirs[d] != oppositeDir || validCount == 0) {
                            validDirs[validCount] = d;
                            validCount++;
                        }
                    }
                }

                if (validCount > 0) {
                    int choice = validDirs[rand() % validCount];
                    ghosts[i].direction = possibleDirs[choice];
                } else {
                    ghosts[i].direction = DIR_NONE;
                }

            } else {

                // Normal behavior (Chase or Scatter)
                if (ghostMode == MODE_SCATTER) {
                    targetX = ghosts[i].scatterTargetX;
                    targetY = ghosts[i].scatterTargetY;
                } else {    // MODE_CHASE
                    switch (i) {
                        case 0:     // Blinky: Direct chase
                            targetX = pacman.gridX;
                            targetY = pacman.gridY;
                            break;
                        
                        case 1:     // Pinky: Ambush (4 tiles ahead of Pac-Man)
                            targetX = pacman.gridX;
                            targetY = pacman.gridY;
                            switch (pacman.direction) {
                                case DIR_UP:
                                    targetY -= 4;
                                    break;
                                case DIR_DOWN:
                                    targetY += 4;
                                    break;
                                case DIR_LEFT:
                                    targetX -= 4;
                                    break;
                                case DIR_RIGHT:
                                    targetX += 4;
                                    break;
                                default:
                                    break;
                            }
                            break;

                        case 2:     // Inky: Complex targeting
                        {
                            // Target is 2 tiles ahead of Pac-Man
                            int intermediateX = pacman.gridX;
                            int intermediateY = pacman.gridY;
                            switch (pacman.direction) {
                                case DIR_UP:
                                    intermediateY -= 2;
                                    break;
                                case DIR_DOWN:
                                    intermediateY += 2;
                                    break;
                                case DIR_LEFT:
                                    intermediateX -= 2;
                                    break;
                                case DIR_RIGHT:
                                    intermediateX += 2;
                                    break;
                                default:
                                    break;
                            }
                            // Vector from Blinky to intermediate point
                            int vecX = intermediateX - ghosts[0].gridX;
                            int vecY = intermediateY - ghosts[0].gridY;
                            // Target is intermediate point + vector
                            targetX = intermediateX + vecX;
                            targetY = intermediateY + vecY;
                            break;
                        }
                        
                        case 3:     // Clyde: Chase or scatter based on distance
                        {
                            float dist = sqrtf(powf(ghosts[i].gridX - pacman.gridX, 2) + powf(ghosts[i].gridY - pacman.gridY, 2));
                            if (dist > 8) {
                                // Chase Pac-Man
                                targetX = pacman.gridX;
                                targetY = pacman.gridY;
                            } else {
                                // Scatter to bottom-left
                                targetX = ghosts[i].scatterTargetX;
                                targetY = ghosts[i].scatterTargetY;
                            }
                            break;
                        }

                        default:
                            targetX = pacman.gridX;
                            targetY = pacman.gridY;
                            break;
                    }
                }

                // Ensure target is within bounds
                targetX = (targetX < 0) ? 0 : (targetX >= MAZE_WIDTH ? MAZE_WIDTH - 1 : targetX);
                targetY = (targetY < 0) ? 0 : (targetY >= MAZE_HEIGHT ? MAZE_HEIGHT - 1 : targetY);

                // Choose the best direction toward the target
                ghosts[i].direction = choose_best_direction(ghosts[i].gridX, ghosts[i].gridY, targetX, targetY, ghosts[i].direction, isExitingPen);
            }

            // Update grid position based on direction
            switch (ghosts[i].direction) {
                case DIR_UP:
                    ghosts[i].gridY --;
                    break;
                case DIR_DOWN:
                    ghosts[i].gridY ++;
                    break;
                case DIR_LEFT:
                    ghosts[i].gridX --;
                    break;
                case DIR_RIGHT:
                    ghosts[i].gridX ++;
                    break;
                default:
                    break;
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

        // Boundary checks to keep ghosts within the maze
        float minX = TILE_SIZE / 2.0f;
        float maxX = (MAZE_WIDTH - 1) * TILE_SIZE + TILE_SIZE / 2.0f;
        float minY = TILE_SIZE / 2.0f;
        float maxY = (MAZE_HEIGHT - 1) * TILE_SIZE + TILE_SIZE / 2.0f;

        if (ghosts[i].x < minX) {
            ghosts[i].x = minX;
            ghosts[i].direction = DIR_NONE;
            ghosts[i].gridX = 0;
        }
        if (ghosts[i].x > maxX) {
            ghosts[i].x = maxX;
            ghosts[i].direction = DIR_NONE;
            ghosts[i].gridX = MAZE_WIDTH - 1;
        }
        if (ghosts[i].y < minY) {
            ghosts[i].y = minY;
            ghosts[i].direction = DIR_NONE;
            ghosts[i].gridY = 0;
        }
        if (ghosts[i].y > maxY) {
            ghosts[i].y = maxY;
            ghosts[i].direction = DIR_NONE;
            ghosts[i].gridY = MAZE_HEIGHT - 1;
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
                    int startX, startY;
                    find_pacman_start(&startX, &startY);
                    init_pacman(startX, startY);
                    init_ghosts();
                }
            }
        }
    }
}