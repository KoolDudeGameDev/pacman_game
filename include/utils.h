#ifndef UTILS_H
#define UTILS_H

#include "game_logic.h"

// Check if a tile is walkable (not a wall, or a ghost gate if specified)
bool IsTileWalkable(int x, int y, bool canPassGate);

// Calculate distance between points
float CalculateDistance(float x1, float y1, float x2, float y2);

// Check collision between two entities based on their positions and radius
bool CheckCollision(float x1, float y1, float x2, float y2, float radius);

#endif // UTILS_H