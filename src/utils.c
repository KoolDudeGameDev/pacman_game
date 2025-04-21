#include "utils.h"
#include "game_logic.h"

bool IsTileWalkable(int x, int y, bool canPassGate) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
        return false;
    }
    return maze[y][x] != WALL && (maze[y][x] != GHOST_GATE || canPassGate);
}

float CalculateDistance(float x1, float y1, float x2, float y2) {
    return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

bool CheckCollision(float x1, float y1, float x2, float y2, float radius) {
    float dist = CalculateDistance(x1, y1, x2, y2);
    return dist < radius;
}