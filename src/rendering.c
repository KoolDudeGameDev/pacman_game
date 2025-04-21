#include "game_logic.h"
#include "rendering.h"

void DrawArc(Vector2 center, float radius, float startAngle, float endAngle, int segments, float thickness, Color color) {
    // Convert angles to radians
    float startRad = startAngle * DEG2RAD;
    float endRad = endAngle * DEG2RAD;
    float angleStep = (endRad - startRad) / segments;

    // Calculate points along the arc and draw lines between them
    for (int i = 0; i < segments; i ++) {
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

void render_maze(int offsetX, int offsetY) {

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
                    DrawCircle(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2, 5, ORANGE); // Larger power pellets
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
    const Color wallColor = { 0, 120, 215, 255 }; /// Light Blue

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
            float outerOffset = outerRadius;    // Center the arc at the corner adjusted by radius

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
}

void render_pacman(int offsetX, int offsetY) {
    DrawCircle(pacman.x + offsetX, pacman.y + offsetY, TILE_SIZE / 2.0f, YELLOW);
}

void render_ghosts(int offsetX, int offsetY) {
    for (int i = 0; i < MAX_GHOSTS; i ++) {
        Color ghostColor;
        Color LightPink = { 255, 182, 193, 255 }; 
        Color customCyan = { 0, 255, 255, 255 };
        if (ghosts[i].state == GHOST_FRIGHTENED) {
            ghostColor = BLUE;
        } else {
            switch (i) {
            case 0: ghostColor = RED; break;        // Blinky
            case 1: ghostColor = LightPink; break;  // Pinky
            case 2: ghostColor = customCyan; break; // Inky
            case 3: ghostColor = ORANGE; break;     // Clyde
            default: ghostColor = RED; break;
            }
        }
        DrawCircle(ghosts[i].x + offsetX, ghosts[i].y + offsetY, TILE_SIZE / 2.0f, ghostColor);
    }
}