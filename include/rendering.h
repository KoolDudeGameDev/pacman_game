#ifndef RENDERING_H
#define RENDERING_H

#include "raylib.h"
#include "game_logic.h"

// Function to draw an arc (for maze walls)
void DrawArc(Vector2 center, float radius, float startAngle, float endAngle, int segments, float thickness, Color color);

// Load textures for ghosts
void LoadGhostTextures(Ghost *ghostArray);

// Unload textures for ghosts
void UnloadGhostTextures(Ghost *ghostArray);

// Rendering functions
void render_maze(int offsetX, int offsetY);
void render_pacman(int offsetX, int offsetY);
void render_ghosts(int offsetX, int offsetY);

#endif // RENDERING_H