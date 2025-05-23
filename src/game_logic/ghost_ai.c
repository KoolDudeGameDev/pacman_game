#include "game_logic.h"
#include "utils.h"

// Function to choose best direction toward target
static Direction choose_best_direction(int currentX, int currentY, int targetX, int targetY, Direction currentDir, bool canPassGate) {
    Direction possibleDirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    float distances[4] = {9999.0f, 9999.0f, 9999.0f, 9999.0f};
    int validDirs[4] = {-1, -1, -1, -1};
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

        // Check if the direction is valid using utility function
        if (IsTileWalkable(newGridX, newGridY, canPassGate)) {
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
                distances[validCount] = CalculateDistance(newGridX, newGridY, targetX, targetY);
                validCount++;
            }   
        }
    }

    // If no valid directions are found, try again but allow opposite direction
    if (validCount == 0) {
        for (int d = 0; d < 4; d++) {
            int newGridX = currentX;
            int newGridY = currentY;
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

            if (IsTileWalkable(newGridX, newGridY, canPassGate)) {
                validDirs[validCount] = d;
                distances[validCount] = CalculateDistance(newGridX, newGridY, targetX, targetY);
                validCount++;
            }
        }
    }

    if (validCount == 0) {
        return DIR_NONE;    // No valid directions, truly stuck
    }

    // Choose the direction that gets closest to target
    float minDist = 9999.0f;
    int bestDir = -1;
    for (int i = 0; i < validCount; i ++) {
        if (distances[i] < minDist) {
            minDist = distances[i];
            bestDir = validDirs[i];
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
        {16, 11},   // Ghost 1 (Pinky)
        {13, 11},   // Ghost 2 (Inky)
        {11, 12}    // Ghost 3 (Clyde)
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
        ghosts[i].speed = 100.0f + (level - 1) * 5.0f;   // Increase speed slightly with level
        ghosts[i].direction = DIR_UP;                    // Initial direction
        ghosts[i].scatterTargetX = scatterTargets[i][0];
        ghosts[i].scatterTargetY = scatterTargets[i][1];
        ghosts[i].stateTimer = i * 5.0f;                 // Staggered release times (0, 5, 10, 15 seconds)
        ghosts[i].frightenedBlinkTimer = 0.0f;           // Initialize blinking timer
        ghosts[i].stuckTimer = 0.0f;                     // Initialize stuck timer
        if (i == 0) {
            // Blinky starts outside the pen
            ghosts[i].state = GHOST_NORMAL;
            ghosts[i].gridX = 14;
            ghosts[i].gridY = 11 - 2;   // Above the pen
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

    /*
        Adjust scatter and chase durations based on level
            Level 1: Chase 20s, Scatter 7s
            Level 2: Chase 20s,  Scatter 5s
            Level 3+: Chase 30s, Scatter 3s
    */

    float chaseDuration = (level >= 3) ? 30.0f : 20.0f;
    float scatterDuration = (level == 1) ? 7.0f : (level == 2) ? 5.0f : 3.0f;

    static float chaseTimes[4];
    static float scatterTimes[4];
    static int phase = 0;   // Current phase (0-3)

    // Set durations for all phases
    for (int i = 0; i < 4; i ++) {
        chaseTimes[i] = (i == 3) ? 9999.0f : chaseDuration;     // Last phase is infinite chase
        scatterTimes[i] = (i == 3) ? 0.0f : scatterDuration;    // No scatter in last phase
    }

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
    static float collisionCooldown = 0.0f;      // Cooldown to prevent multiple collisions in single frame

    // Skip updates during ghost eaten animation
    if (gameState == STATE_GHOST_EATEN) {
        if (IsSoundPlaying(sfx_ghost_frightened) && !isFrightenedSoundPaused) {
            PauseSound(sfx_ghost_frightened);
            isFrightenedSoundPaused = true;
        }
        return;
    }

    // Update Chase/Scatter mode
    update_ghost_mode();

    // Update collision cooldown
    if (collisionCooldown > 0.0f) {
        collisionCooldown -= deltaTime;
    }
    
    // Manage the frightened sound based on ghost states
    bool anyFrightened = false;
    for (int i = 0; i < MAX_GHOSTS; i++) {
        if (ghosts[i].state == GHOST_FRIGHTENED && ghosts[i].stateTimer > 0.0f) {
            anyFrightened = true;
            break;
        }
    }

    if (anyFrightened && gameState == STATE_PLAYING) {
        if (!IsSoundPlaying(sfx_ghost_frightened) && !isFrightenedSoundPaused) {
            PlaySound(sfx_ghost_frightened);
        }
    } else {
        if (IsSoundPlaying(sfx_ghost_frightened)) {
            StopSound(sfx_ghost_frightened);
        }
        isFrightenedSoundPaused = false;
    }

    // Update power pellet timer
    if (powerPelletTimer > 0.0f) {
        powerPelletTimer -= deltaTime;
        if (powerPelletTimer < 0.0f) {
            powerPelletTimer = 0.0f;
        }
    }
    
    // Check for collisions first to handle multiple simultaneous ghost collisions
    bool ghostsToEat[MAX_GHOSTS] = {false};
    int numGhostsToEat = 0;
    if (collisionCooldown <= 0.0f) {
        for (int i = 0; i < MAX_GHOSTS; i ++) {
            if (ghosts[i].state == GHOST_FRIGHTENED && 
                CheckCollision(ghosts[i].x, ghosts[i].y, pacman.x, pacman.y, TILE_SIZE / 2.0f)) {
                ghostsToEat[i] = true;
                numGhostsToEat ++;
                printf("Ghost %d collision detected! eatenGhostCount: %d\n", i, eatenGhostCount + 1);
            }
        }
    }

    // Process eaten ghosts
    if (numGhostsToEat > 0) {
        for (int i = 0; i < MAX_GHOSTS; i ++) {
            if (ghostsToEat[i]) {
                ghosts[i].state = GHOST_RETURNING;
                eatenGhostCount ++;
                totalGhostsEaten ++;
                int points = 200 * (1 << (eatenGhostCount - 1));    // 200, 400, 800, 1600
                pacman.score += points;
                PlaySound(sfx_eat_ghost);
                // Debug print for the added score
                printf("Ghost %d eaten! Added %d points. Total score: %d (eatenGhostCount: %d)\n", 
                       i, points, pacman.score, eatenGhostCount);
                eatenGhostIndex = i;
                gameState = STATE_GHOST_EATEN;
                ghostEatenTimer = 1.0f;     // 1 sec pause
                collisionCooldown = 0.5f;   // Preven immediate re-collision
                break;
            }
        }
        return;     // Exit update to ensure animation state takes precedence
    }

    for (int i = 0; i < MAX_GHOSTS; i++) {
        float currentSpeed = ghosts[i].speed;    // Default speed

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
                    ghosts[i].y -= currentSpeed * deltaTime;
                    break;
                case DIR_DOWN:
                    ghosts[i].y += currentSpeed * deltaTime;
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
                ghosts[i].frightenedBlinkTimer = 0.0f;
            }
            currentSpeed *= 0.8f;   // Slower when frightened
        }

        // Handle returning state (ghost travels back to pen)
        if (ghosts[i].state == GHOST_RETURNING) {
            currentSpeed *= 1.5f;      // Faster speed when returning
            int penX = 14;
            int penY = 11;

            // Check if ghost has reached pen
            if (ghosts[i].gridX == penX && ghosts[i].gridY == penY) {
                ghosts[i].state = GHOST_PENNED;
                ghosts[i].stateTimer = 2.0f;        // Wait in pen for 2 secs before exiting
                ghosts[i].x = penX * TILE_SIZE + TILE_SIZE / 2.0f;
                ghosts[i].y = penY * TILE_SIZE + TILE_SIZE / 2.0f;
                ghosts[i].stuckTimer = 0.0f;        // Reset stuck timer
                printf("Ghost %d reached pen at (%d, %d)\n", i, penX, penY);
                continue;
            }

            // Check if ghost is centered in the current tile
            float centerX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
            float centerY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
            bool atCenter = fabs(ghosts[i].x - centerX) < 1.0f && fabs(ghosts[i].y - centerY) < 1.0f;

            if (atCenter) {
                ghosts[i].x = centerX;
                ghosts[i].y = centerY;

                // Choose direction to return to pen
                ghosts[i].direction = choose_best_direction(ghosts[i].gridX, ghosts[i].gridY, penX, penY, ghosts[i].direction, true);

                // Debug: Log the ghost's position and direction
                printf("Ghost %d at (%d, %d), Direction: %d\n", i, ghosts[i].gridX, ghosts[i].gridY, ghosts[i].direction);
                
                // Update stuck timer
                if (ghosts[i].direction == DIR_NONE) {
                    ghosts[i].stuckTimer += deltaTime;
                    // Debug: Log when the ghost is stuck
                    printf("Ghost %d is stuck at (%d, %d), Stuck Timer: %.2f\n", i, ghosts[i].gridX, ghosts[i].gridY, ghosts[i].stuckTimer);
                } else {
                    ghosts[i].stuckTimer = 0.0f; // Reset timer if moving
                }

                // If stuck for too long, force a random valid direction
                if (ghosts[i].stuckTimer > 1.0f) { // Stuck for 1 second
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

                        if (IsTileWalkable(newGridX, newGridY, true)) {
                            validDirs[validCount] = d;
                            validCount++;
                        }
                    }

                    if (validCount > 0) {
                        ghosts[i].direction = possibleDirs[validDirs[rand() % validCount]];
                        ghosts[i].stuckTimer = 0.0f; // Reset stuck timer
                        // Debug: Log when forcing a new direction
                        printf("Ghost %d was stuck, forcing new direction: %d\n", i, ghosts[i].direction);
                    }
                }
                
                // Update grid position based on direction, but only if the next tile is walkable
                int newGridX = ghosts[i].gridX;
                int newGridY = ghosts[i].gridY;
                switch (ghosts[i].direction) {
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

                if (IsTileWalkable(newGridX, newGridY, true)) {
                    ghosts[i].gridX = newGridX;
                    ghosts[i].gridY = newGridY;
                } else {
                    ghosts[i].direction = DIR_NONE;
                }
            }

            // Move ghost towards the center of the next tile
            float targetX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
            float targetY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
            float moveDist = currentSpeed * deltaTime;

            switch (ghosts[i].direction) {
                case DIR_UP:
                    ghosts[i].y -= moveDist;
                    if (ghosts[i].y < targetY) ghosts[i].y = targetY;
                    break;
                case DIR_DOWN:
                    ghosts[i].y += moveDist;
                    if (ghosts[i].y > targetY) ghosts[i].y = targetY;
                    break;
                case DIR_LEFT:
                    ghosts[i].x -= moveDist;
                    if (ghosts[i].x < targetX) ghosts[i].x = targetX;
                    break;
                case DIR_RIGHT:
                    ghosts[i].x += moveDist;
                    if (ghosts[i].x > targetX) ghosts[i].x = targetX;
                    break;
                default:
                    break;
            }
            continue;   // Skip normal movement logic while returning
        }

        // Check if ghost is centered in the current tile
        float centerX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        float centerY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        bool atCenter = fabs(ghosts[i].x - centerX) < 1.0f && fabs(ghosts[i].y - centerY) < 1.0f;

        // Determine if the ghost is still exiting the pen (not yet above the gate)
        bool isExitingPen = (ghosts[i].gridY >= 11 && ghosts[i].gridY <= 12 && ghosts[i].state == GHOST_NORMAL &&
            ghosts[i].gridX >= 13 && ghosts[i].gridX <= 14);

        if (atCenter) {
            // Snap position to center to prevent drift
            ghosts[i].x = centerX;
            ghosts[i].y = centerY;

            // If exiting pen, guide ghost to exit point (row 11, column 14)
            if (isExitingPen) {
                int exitX = 14;
                int exitY = 11;
                if (ghosts[i].gridY > 11) {
                    ghosts[i].direction = DIR_UP;
                    ghosts[i].gridY--;
                } else if (ghosts[i].gridX < exitX) {
                    ghosts[i].direction = DIR_RIGHT;
                    ghosts[i].gridX++;
                } else if (ghosts[i].gridX > exitX) {
                    ghosts[i].direction = DIR_LEFT;
                    ghosts[i].gridX--;
                } else {
                    ghosts[i].direction = DIR_UP;
                    ghosts[i].gridY--;
                }
                continue;
            }

            // Determine target
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

                    // Use utility function to check if the tile is walkable
                    if (IsTileWalkable(newGridX, newGridY, false)) {
                        // Avoid moving back in the opposite direction (prevents jittering)
                        Direction oppositeDir = DIR_NONE;
                        switch (ghosts[i].direction) {
                            case DIR_UP:    oppositeDir = DIR_DOWN; break;
                            case DIR_DOWN:  oppositeDir = DIR_UP; break;
                            case DIR_LEFT:  oppositeDir = DIR_RIGHT; break;
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
                    ghosts[i].direction = possibleDirs[validDirs[rand() % validCount]];
                } else {
                    ghosts[i].direction = DIR_NONE;
                }

            } else {
                
                // Normal behavior (Chase or Scatter)
                bool isBlinky = (i == 0);       // Blinky is ghost 0
                float pelletRatio = (float)remainingPelletCount / initialPelletCount;

                if (isBlinky && (level >= 3 || pelletRatio < 0.2f)) {
                    targetX = pacman.gridX;
                    targetY = pacman.gridY;
                } else if (ghostMode == MODE_SCATTER) {
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
                            float dist = CalculateDistance(ghosts[i].gridX, ghosts[i].gridY, pacman.gridX, pacman.gridY);
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
                ghosts[i].direction = choose_best_direction(ghosts[i].gridX, ghosts[i].gridY, targetX, targetY, ghosts[i].direction, false);
            }

            // Update grid position based on direction, but only if the next tile is walkable
            int newGridX = ghosts[i].gridX;
            int newGridY = ghosts[i].gridY;
            switch (ghosts[i].direction) {
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

            if (IsTileWalkable(newGridX, newGridY, false)) {
                ghosts[i].gridX = newGridX;
                ghosts[i].gridY = newGridY;
            } else {
                ghosts[i].direction = DIR_NONE;
            }
        }

        // Handle tunnel (rows 12 and 13, columns 0 and 27)
        if ((ghosts[i].gridY == 12 || ghosts[i].gridY == 13)) {
            if (ghosts[i].gridX <= 0 && ghosts[i].direction == DIR_LEFT) {
                ghosts[i].gridX = MAZE_WIDTH - 1;       // Teleport to right side
                ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
            } else if (ghosts[i].gridX >= MAZE_WIDTH - 1 && ghosts[i].direction == DIR_RIGHT) {
                ghosts[i].gridX = 0;                    // Teleport to left side
                ghosts[i].x = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
            }
        }

        // Move ghost towards the center of the next tile
        float targetX = ghosts[i].gridX * TILE_SIZE + TILE_SIZE / 2.0f;
        float targetY = ghosts[i].gridY * TILE_SIZE + TILE_SIZE / 2.0f;
        float moveDist = currentSpeed * deltaTime;

        switch (ghosts[i].direction) {
            case DIR_UP:
                ghosts[i].y -= moveDist;
                if (ghosts[i].y < targetY) ghosts[i].y = targetY;
                break;
            case DIR_DOWN:
                ghosts[i].y += moveDist;
                if (ghosts[i].y > targetY) ghosts[i].y = targetY;
                break;
            case DIR_LEFT:
                ghosts[i].x -= moveDist;
                if (ghosts[i].x < targetX) ghosts[i].x = targetX;
                break;
            case DIR_RIGHT:
                ghosts[i].x += moveDist;
                if (ghosts[i].x > targetX) ghosts[i].x = targetX;
                break;
            default:
                break;
        }
  
        // Collision with Pac-Man (handled above for frightened ghosts)
        if (collisionCooldown <= 0.0f && ghosts[i].state == GHOST_NORMAL &&
            CheckCollision(ghosts[i].x, ghosts[i].y, pacman.x, pacman.y, TILE_SIZE / 2.0f)) {
            gameState = STATE_DEATH_ANIM;
            deathAnimTimer = 1.0f;
            deathAnimFrame = 0;
            collisionCooldown = 1.0f; // Longer cooldown for death
            playPacmanMove = false;
            StopSound(sfx_pacman_move);
            break;
        }
    }

    // Boundary check
    for (int i = 0; i < MAX_GHOSTS; i++) {
        float minX = TILE_SIZE / 2.0f;
        float maxX = (MAZE_WIDTH - 1) * TILE_SIZE + TILE_SIZE / 2.0f;
        float minY = TILE_SIZE / 2.0f;
        float maxY = (MAZE_HEIGHT - 1) * TILE_SIZE + TILE_SIZE / 2.0f;

        if (ghosts[i].x < minX && ghosts[i].gridY != 12 && ghosts[i].gridY != 13) {
            ghosts[i].x = minX;
            ghosts[i].gridX = 0;
        }
        if (ghosts[i].x > maxX && ghosts[i].gridY != 12 && ghosts[i].gridY != 13) {
            ghosts[i].x = maxX;
            ghosts[i].gridX = MAZE_WIDTH - 1;
        }
        if (ghosts[i].y < minY) {
            ghosts[i].y = minY;
            ghosts[i].gridY = 0;
        }
        if (ghosts[i].y > maxY) {
            ghosts[i].y = maxY;
            ghosts[i].gridY = MAZE_HEIGHT - 1;
        }
    }
}