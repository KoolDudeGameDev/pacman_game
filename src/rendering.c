#include "rendering.h"

// Texture Management
// ----------------------------------------------------------------------------------------
// Loads ghost textures and assigns them to the ghost array, also setting Pac-Man and fruit sprites.
void LoadGhostTextures(Ghost *ghostArray) {
    // Load the sprite sheet as an image
    Image spriteImage = LoadImage("assets/sprites/pacman_general_sprites.png");

    // Replace black (RGB 0,0,0) with transparent (RGBA 0,0,0,0)
    Color black = { 0, 0, 0, 255 };         // Black color (opaque)
    Color transparent = { 0, 0, 0, 0 };     // Transparent color
    ImageColorReplace(&spriteImage, black, transparent);

    // Convert the modified image to a texture
    spriteSheet = LoadTextureFromImage(spriteImage);

    // Unload the image
    UnloadImage(spriteImage);

    // Assign Pac-Man sprite
    pacman.sprite = spriteSheet;

    // Assign fruit sprite
    fruit.sprite = spriteSheet;
    
    // Assign Ghost sprites
    for (int i = 0; i < MAX_GHOSTS; i++) {
        ghostArray[i].normalSprite[0] = spriteSheet;
        ghostArray[i].normalSprite[1] = spriteSheet;
        ghostArray[i].frightenedSprite = spriteSheet;
        ghostArray[i].eyeballSprite = spriteSheet;
        ghostArray[i].animTimer = 0.0f;
        ghostArray[i].currentFrame = 0;
    }
}

// Unloads the sprite sheet texture to free memory.
void UnloadGhostTextures(Ghost *ghostArray) {
    UnloadTexture(spriteSheet);
}

// Utility Functions
// ----------------------------------------------------------------------------------------
// Draws an arc for maze wall corners, used in render_maze.
void DrawArc(Vector2 center, float radius, float startAngle, float endAngle, int segments, float thickness, Color color) {
    // Convert angles to radians
    float startRad = startAngle * DEG2RAD;
    float endRad = endAngle * DEG2RAD;
    float angleStep = (endRad - startRad) / segments;

    // Calculate points along the arc and draw lines between them
    for (int i = 0; i < segments; i++) {
        float angle1 = startRad + i * angleStep;
        float angle2 = startRad + (i + 1) * angleStep;

        Vector2 point1 = (Vector2) {
            center.x + radius * cosf(angle1),
            center.y + radius * sinf(angle1)
        };

        Vector2 point2 = (Vector2) {
            center.x + radius * cosf(angle2),
            center.y + radius * sinf(angle2)
        };

        DrawLineEx(point1, point2, thickness, color);
    }
}

// Rendering Functions
// ----------------------------------------------------------------------------------------
// Renders the maze, including walls, pellets, power pellets, ghost gate, and active fruit.
void render_maze(int offsetX, int offsetY) {
    // Update global blink timer
    blinkTimer += GetFrameTime();
    if (blinkTimer >= 1.0f) { // Reset every 1 second (0.5s on, 0.5s off)
        blinkTimer = 0.0f;
    }
    bool powerPelletVisible = (blinkTimer < 0.5f); // Visible for first 0.5 seconds

    // First pass: Draw pellets, power pellets, and ghost gate
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            int posX = offsetX + x * TILE_SIZE;
            int posY = offsetY + y * TILE_SIZE;

            switch (maze[y][x]) {
                case PELLET:
                    DrawCircle(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2, 2, YELLOW); // Smaller pellets
                    break;
                case POWER_PELLET:
                    if (powerPelletVisible) {
                        DrawCircle(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2, 5, ORANGE); // Larger power pellets
                    }
                    break;
                case GHOST_GATE:
                    DrawRectangle(posX, posY + TILE_SIZE / 2 - 2, TILE_SIZE, 4, WHITE); // Horizontal gate
                    break;
                default:
                    break;
            }
        }
    }

    // Second pass: Draw walls as single outlines with curved corners
    const int lineThickness = 1;
    const Color wallColor = { 0, 120, 215, 255 }; // Light Blue

    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] != WALL) continue;

            int pixelX = offsetX + x * TILE_SIZE;
            int pixelY = offsetY + y * TILE_SIZE;

            // Check neighboring tiles to determine wall connections
            bool up = (y > 0 && maze[y - 1][x] == WALL);
            bool down = (y < MAZE_HEIGHT - 1 && maze[y + 1][x] == WALL);
            bool left = (x > 0 && maze[y][x - 1] == WALL);
            bool right = (x < MAZE_WIDTH - 1 && maze[y][x + 1] == WALL);

            float outerRadius = TILE_SIZE / 2.0f; // Radius for outer arcs

            // Draw outline lines only on sides facing non-wall tiles

            // Top side: draw if there's no wall above
            if (!up) {
                int startX = pixelX;
                int endX = pixelX + TILE_SIZE;
                // Adjust start/end to connect with corners
                if (!left) startX += outerRadius;
                if (!right) endX -= outerRadius;
                DrawLineEx((Vector2){startX, pixelY}, (Vector2){endX, pixelY}, lineThickness, wallColor);
            }

            // Bottom side: draw if there's no wall below
            if (!down) {
                int startX = pixelX;
                int endX = pixelX + TILE_SIZE;
                // Adjust start/end to connect with corners
                if (!left) startX += outerRadius;
                if (!right) endX -= outerRadius;
                DrawLineEx((Vector2){startX, pixelY + TILE_SIZE}, (Vector2){endX, pixelY + TILE_SIZE}, lineThickness, wallColor);
            }

            // Left side: draw if there's no wall to the left
            if (!left) {
                int startY = pixelY;
                int endY = pixelY + TILE_SIZE;
                // Adjust start/end to connect with corners
                if (!up) startY += outerRadius;
                if (!down) endY -= outerRadius;
                DrawLineEx((Vector2){pixelX, startY}, (Vector2){pixelX, endY}, lineThickness, wallColor);
            }

            // Right side: draw if there's no wall to the right
            if (!right) {
                int startY = pixelY;
                int endY = pixelY + TILE_SIZE;
                // Adjust start/end to connect with corners
                if (!up) startY += outerRadius;
                if (!down) endY -= outerRadius;
                DrawLineEx((Vector2){pixelX + TILE_SIZE, startY}, (Vector2){pixelX + TILE_SIZE, endY}, lineThickness, wallColor);
            }

            // Draw curved corners for outer ends
            float outerOffset = outerRadius; // Center the arc at the corner adjusted by radius

            // Top-left corner or end
            if (!up && !left) {
                DrawArc((Vector2){pixelX + outerOffset, pixelY + outerOffset}, outerRadius, 180, 270, 16, lineThickness, wallColor);
            }
            // Top-right corner or end
            if (!up && !right) {
                DrawArc((Vector2){pixelX + TILE_SIZE - outerOffset, pixelY + outerOffset}, outerRadius, 270, 360, 16, lineThickness, wallColor);
            }
            // Bottom-left corner or end
            if (!down && !left) {
                DrawArc((Vector2){pixelX + outerOffset, pixelY + TILE_SIZE - outerOffset}, outerRadius, 90, 180, 16, lineThickness, wallColor);
            }
            // Bottom-right corner or end
            if (!down && !right) {
                DrawArc((Vector2){pixelX + TILE_SIZE - outerOffset, pixelY + TILE_SIZE - outerOffset}, outerRadius, 0, 90, 16, lineThickness, wallColor);
            }
        }
    }

    // Render fruit if active
    if (fruit.active) {
        //printf("Rendering fruit at gridX: %d, gridY: %d\n", fruit.gridX, fruit.gridY);
        float scaleFactor = (float)TILE_SIZE / 16.0f;
        float scaledWidth = 16.0f * scaleFactor;
        float scaledHeight = 16.0f * scaleFactor;
        Rectangle sourceRec = { 35.0f, 48.0f, 16.0f, 16.0f };

        float tileCenterX = fruit.gridX * TILE_SIZE + offsetX + (TILE_SIZE / 2.0f);
        float tileCenterY = fruit.gridY * TILE_SIZE + offsetY + (TILE_SIZE / 2.0f);

        // Position the sprite so its center is at the tile's center
        Rectangle destRec = {
            tileCenterX - (scaledWidth / 2.0f),  // Top-left x
            tileCenterY - (scaledHeight / 2.0f), // Top-left y
            scaledWidth,
            scaledHeight
        };
        Vector2 origin = { 0.0f, 0.0f }; // Origin at top-left for precise positioning
        DrawTexturePro(fruit.sprite, sourceRec, destRec, origin, 0.0f, WHITE); 
    }  
}

// Renders Pac-Man with mouth animation based on direction.
void render_pacman(int offsetX, int offsetY) {
    // Mouth animation
    static float animTimer = 0.0f;
    static int currentFrame = 0;
    animTimer += GetFrameTime();
    if (animTimer >= 0.1f) {
        currentFrame = (currentFrame + 1) % 3;      // 3 frames: closed, half-open, fully open
        animTimer = 0.0f;
    } 

    // Define source rectangle
    Rectangle sourceRec = { currentFrame * 18.0f, 0.0f, 16.0f, 16.0f};

    // Define destination rectangle
    float scaleFactor = (float)TILE_SIZE / 16.0f; // Scale 16x16 sprite to 20x20
    float scaledWidth = 16.0f * scaleFactor;
    float scaledHeight = 16.0f * scaleFactor;

    Rectangle destRec = {
        pacman.x + offsetX, 
        pacman.y + offsetY, 
        scaledWidth,
        scaledHeight
    };

    float rotation = 0.0f;
    switch (pacman.direction) {
        case DIR_RIGHT: rotation = 0.0f; break;
        case DIR_LEFT: rotation = 180.0f; break;
        case DIR_UP: rotation = 270.0f; break;
        case DIR_DOWN: rotation = 90.0f; break;
        default: break;
    }

    Vector2 origin = { scaledWidth / 2.0f, scaledHeight / 2.0f };
    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, rotation, WHITE);
}

// Renders Pac-Man's death animation with shrinking effect.
void render_pacman_death(int offsetX, int offsetY) {
    // Define source rectangle for death animation starting at x=36.0f
    Rectangle sourceRec = { (deathAnimFrame * 16.0f) + 36.0f, 0.0f, 16.0f, 16.0f};

    // Define destination rectangle with shrinking effect
    float scaleFactor = (float)TILE_SIZE / 16.0f * (deathAnimTimer / 2.0f);     // Shrinks over 2 seconds
    float scaledWidth = 16.0f * scaleFactor;
    float scaledHeight = 16.0f * scaleFactor;

    Rectangle destRec = {
        pacman.x + offsetX,
        pacman.y + offsetY,
        scaledWidth,
        scaledHeight
    };

    Vector2 origin = { scaledWidth / 2.0f, scaledHeight / 2.0f};
    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
}

// Renders all ghosts based on their state (normal, frightened, returning) and direction.
void render_ghosts(int offsetX, int offsetY) {
    for (int i = 0; i < MAX_GHOSTS; i++) {
        // Update animation timer for normal ghost animation
        ghosts[i].animTimer += GetFrameTime();
        if (ghosts[i].animTimer >= 0.2f) {      // Switch frames every 0.2 seconds
            ghosts[i].currentFrame = (ghosts[i].currentFrame + 1) % 2;
            ghosts[i].animTimer = 0.0f;
        }

        // Update frightened blink timer for ghost
        if (ghosts[i].state == GHOST_FRIGHTENED) {
            ghosts[i].frightenedBlinkTimer += GetFrameTime();
            if (ghosts[i].frightenedBlinkTimer >= 0.4f) { // Reset every 0.4 seconds (0.2s per texture)
                ghosts[i].frightenedBlinkTimer = 0.0f;
            }
        }

        // Determine if we should use the white frightened sprite
        bool useWhiteFrightenedSprite = (ghosts[i].frightenedBlinkTimer < 0.2f); // Use white sprite for first 0.2 seconds

        // Adjust scale to fit within maze tiles (TILE_SIZE = 20)
        float ghostScaleFactor = (float)TILE_SIZE / 16.0f; // Scale 16x16 sprite to 20x20
        float scaledWidth = 16.0f * ghostScaleFactor;
        float scaledHeight = 16.0f * ghostScaleFactor;

        // Destination rectangle, centered on ghost's position
        Rectangle destRec = {
            ghosts[i].x + offsetX,
            ghosts[i].y + offsetY,
            scaledWidth,
            scaledHeight
        };

        Vector2 origin = { scaledWidth / 2.0f, scaledHeight / 2.0f };

        // If in STATE_GHOST_EATEN and this is the eaten ghost, show the score sprite
        if (gameState == STATE_GHOST_EATEN && i == eatenGhostIndex) {
            // Calculate points based on eatenGhostCount (from ghost_ai.c: 200, 400, 800, 1600)
            int points = 200 * (1 << (eatenGhostCount - 1));
            Rectangle scoreSourceRec;

            // Map points to sprite coordinates
            switch (points) {
                case 200:  scoreSourceRec = (Rectangle){ 4.0f, 128.0f, 16.0f, 16.0f }; break;
                case 400:  scoreSourceRec = (Rectangle){ 20.0f, 128.0f, 16.0f, 16.0f }; break;
                case 800:  scoreSourceRec = (Rectangle){ 36.0f, 128.0f, 16.0f, 16.0f }; break;
                case 1600: scoreSourceRec = (Rectangle){ 52.0f, 128.0f, 16.0f, 16.0f }; break;
                default:   scoreSourceRec = (Rectangle){ 4.0f, 128.0f, 16.0f, 16.0f }; break;
            }

            DrawTexturePro(spriteSheet, scoreSourceRec, destRec, origin, 0.0f, WHITE);
            continue; // Skip rendering the ghost itself while showing the score
        }
        
        // Define source rectangle based on ghost's type and state
        Rectangle sourceRec;
        Texture2D texture;
        
        if (ghosts[i].state == GHOST_RETURNING) {
            float xOffset;
            switch (ghosts[i].direction) {
                case DIR_RIGHT: xOffset = 132.0f + ghosts[i].currentFrame * 16.0f; break;       
                case DIR_LEFT: xOffset = 148.0f + ghosts[i].currentFrame * 16.0f; break; 
                case DIR_UP: xOffset = 164.0f + ghosts[i].currentFrame * 16.0f; break;  
                case DIR_DOWN: xOffset = 180.0f + ghosts[i].currentFrame * 16.0f; break;
                default: xOffset = 132.0f + ghosts[i].currentFrame * 16.0f; break;  
            }
            sourceRec = (Rectangle){ xOffset, 80.0f, 16.0f, 16.0f };
            texture = ghosts[i].eyeballSprite;
        } else if (ghosts[i].state == GHOST_FRIGHTENED) {
            // Default to blue frightened sprite
            float xOffset = 132.0f; // Blue frightened sprite
            // Blinking effect when frightened timer is low (last 2 seconds)
            if (ghosts[i].stateTimer <= 2.0f) {
                xOffset = useWhiteFrightenedSprite ? 164.0f : 132.0f; // Switch between white and blue
            }
            sourceRec = (Rectangle){ xOffset, 64.0f, 16.0f, 16.0f };
            texture = ghosts[i].frightenedSprite;
        } else {
            // Normal sprite based on ghost type (Blinky, Pinky, Inky, Clyde)
            float xOffset;
            float yOffset;
            switch (i) {
                case 0: yOffset = 64.0f; break;  // Blinky (red)
                case 1: yOffset = 80.0f; break;  // Pinky (pink)
                case 2: yOffset = 96.0f; break;  // Inky (cyan)
                case 3: yOffset = 112.0f; break; // Clyde (orange)
                default: yOffset = 64.0f; break;
            }

            // Select xOffset based on direction and animation frame
            switch (ghosts[i].direction) {
                case DIR_RIGHT: xOffset = 4.0f + ghosts[i].currentFrame * 16.0f; break;       
                case DIR_LEFT: xOffset = 36.0f + ghosts[i].currentFrame * 16.0f; break; 
                case DIR_UP: xOffset = 68.0f + ghosts[i].currentFrame * 16.0f; break;  
                case DIR_DOWN: xOffset = 100.0f + ghosts[i].currentFrame * 16.0f; break;
                default: xOffset = 4.0f + ghosts[i].currentFrame * 16.0f; break;              
            }
            sourceRec = (Rectangle){ xOffset, yOffset, 16.0f, 16.0f };
            texture = ghosts[i].normalSprite[ghosts[i].currentFrame];
        }
        DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

// Renders the fruit if active, centered at its grid position.
void render_fruit(int offsetX, int offsetY) {
    if (fruit.active) {
        //printf("Rendering fruit at gridX: %d, gridY: %d\n", fruit.gridX, fruit.gridY);
        float scaleFactor = (float)TILE_SIZE / 16.0f;
        float scaledWidth = 16.0f * scaleFactor;
        float scaledHeight = 16.0f * scaleFactor;
        Rectangle sourceRec = { 35.0f, 48.0f, 16.0f, 16.0f };

        float tileCenterX = fruit.gridX * TILE_SIZE + offsetX + (TILE_SIZE / 2.0f);
        float tileCenterY = fruit.gridY * TILE_SIZE + offsetY + (TILE_SIZE / 2.0f);

        // Position the sprite so its center is at the tile's center
        Rectangle destRec = {
            tileCenterX - (scaledWidth / 2.0f),  // Top-left x
            tileCenterY - (scaledHeight / 2.0f), // Top-left y
            scaledWidth,
            scaledHeight
        };
        Vector2 origin = { 0.0f, 0.0f }; // Origin at top-left for precise positioning
        DrawTexturePro(fruit.sprite, sourceRec, destRec, origin, 0.0f, WHITE); 
    }  
}