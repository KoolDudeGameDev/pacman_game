#ifndef RENDERING_H
#define RENDERING_H

#include "game_logic.h"

void DrawArc(Vector2 center, float radius, float startAngle, float endAngle, int segments, float thickness, Color color);
void render_maze(int offsetX, int offsetY);
void render_pacman(int offsetX, int offsetY);
void render_ghosts(int offsetX, int offsetY);

#endif