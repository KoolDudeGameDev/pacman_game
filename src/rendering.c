#include "raylib.h"
#include "rendering.h"

extern Player pacman;

void render_maze(void) {
    for (int y = 0; y < MAZE_HEIGHT; y ++) {
        for (int x = 0; x < MAZE_WIDTH; x ++) {
            int posX = x * TILE_SIZE;
            int posY = y * TILE_SIZE;

            switch (maze[y][x]) {
                case WALL:
                    DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, BLUE);
                    break;
                case PELLET:
                    DrawCircle(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2, 5, YELLOW);
                    break;
                case POWER_PELLET:
                    DrawCircle(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2, 10, ORANGE);
                    break;
                default:
                    break;
            }
        }
    }
}

void render_pacman(void) {
    int posX = pacman.x * TILE_SIZE + TILE_SIZE / 2;
    int posY = pacman.y * TILE_SIZE + TILE_SIZE / 2;
    DrawCircle(posX, posY, 15, YELLOW);
}