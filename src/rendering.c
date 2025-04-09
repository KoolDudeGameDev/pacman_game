#include "raylib.h"
#include "game_logic.h"
#include "rendering.h"

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

    // Second pass: Draw walls with double lines and rounded corners
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] != WALL) continue;

            int pixelX = offsetX + x * TILE_SIZE;
            int pixelY = offsetY + y * TILE_SIZE;
            const int lineThickness = 2;
            const int gap = 2;

            // Check neighboring tiles to determine wall connections
            bool up = (y > 0 && maze[y - 1][x] == WALL);
            bool down = (y < MAZE_HEIGHT - 1 && maze[y + 1][x] == WALL);
            bool left = (x > 0 && maze[y][x - 1] == WALL);
            bool right = (x < MAZE_WIDTH - 1 && maze[y][x + 1] == WALL);

            // Draw vertical double lines if there's a vertical connection
            if (up || down) {
                // Outer line (left side of the wall tile)
                DrawRectangle(pixelX + gap, pixelY, lineThickness, TILE_SIZE, RED);
                // Inner line (right side of the wall tile)
                DrawRectangle(pixelX + TILE_SIZE - gap - lineThickness, pixelY, lineThickness, TILE_SIZE, ORANGE);
            }

            // Draw horizontal double lines if there's a horizontal connection
            if (left || right) {
                // Outer line (top side of the wall tile)
                DrawRectangle(pixelX, pixelY + gap, TILE_SIZE, lineThickness, BLUE);
                // Inner line (bottom side of the wall tile)
                DrawRectangle(pixelX, pixelY + TILE_SIZE - gap - lineThickness, TILE_SIZE, lineThickness, BLUE);
            }

            // Draw rounded corners and ends
            // Top-left corner or end
            if (!up && !left) {
                DrawCircle(pixelX + gap + lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE); // Outer
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE); // Inner (diagonal opposite)
            }
            // Top-right corner or end
            if (!up && !right) {
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE); // Outer
                DrawCircle(pixelX + gap + lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE); // Inner (diagonal opposite)
            }
            // Bottom-left corner or end
            if (!down && !left) {
                DrawCircle(pixelX + gap + lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE); // Outer
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE); // Inner (diagonal opposite)
            }
            // Bottom-right corner or end
            if (!down && !right) {
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE); // Outer
                DrawCircle(pixelX + gap + lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE); // Inner (diagonal opposite)
            }

            // Draw corner arcs for inner corners
            // Top-left inner corner
            if (up && left && (y > 0 && x > 0 && maze[y - 1][x - 1] != WALL)) {
                DrawCircle(pixelX + gap + lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE);
            }
            // Top-right inner corner
            if (up && right && (y > 0 && x < MAZE_WIDTH - 1 && maze[y - 1][x + 1] != WALL)) {
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + gap + lineThickness, lineThickness, BLUE);
            }
            // Bottom-left inner corner
            if (down && left && (y < MAZE_HEIGHT - 1 && x > 0 && maze[y + 1][x - 1] != WALL)) {
                DrawCircle(pixelX + gap + lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE);
            }
            // Bottom-right inner corner
            if (down && right && (y < MAZE_HEIGHT - 1 && x < MAZE_WIDTH - 1 && maze[y + 1][x + 1] != WALL)) {
                DrawCircle(pixelX + TILE_SIZE - gap - lineThickness, pixelY + TILE_SIZE - gap - lineThickness, lineThickness, BLUE);
            }
        }
    }
}

void render_pacman(int offsetX, int offsetY) {
    int posX = offsetX + pacman.x * TILE_SIZE + TILE_SIZE / 2;
    int posY = offsetY + pacman.y * TILE_SIZE + TILE_SIZE / 2;
    DrawCircle(posX, posY, TILE_SIZE / 2 - 2, YELLOW);
}