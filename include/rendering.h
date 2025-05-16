#ifndef RENDERING_H
#define RENDERING_H

#include "raylib.h"
#include "game_logic.h"

// Texture Management
// ----------------------------------------------------------------------------------------
// Loads ghost textures and assigns them to the ghost array, handling transparency.
// Parameters:
//   ghostArray - Array of Ghost structures to assign textures to.
void LoadGhostTextures(Ghost *ghostArray);

// Unloads ghost textures to free memory.
// Parameters:
//   ghostArray - Array of Ghost structures whose textures are to be unloaded.
void UnloadGhostTextures(Ghost *ghostArray);

// Utility Functions
// ----------------------------------------------------------------------------------------
// Draws an arc for maze wall corners.
// Parameters:
//   center - Center point of the arc.
//   radius - Radius of the arc.
//   startAngle - Starting angle in degrees.
//   endAngle - Ending angle in degrees.
//   segments - Number of line segments to approximate the arc.
//   thickness - Thickness of the arc line.
//   color - Color of the arc.
void DrawArc(Vector2 center, float radius, float startAngle, float endAngle, int segments, float thickness, Color color);

// Rendering Functions
// ----------------------------------------------------------------------------------------
// Renders the maze, including walls, pellets, power pellets, and ghost gate.
// Parameters:
//   offsetX - X offset for rendering the maze in pixels.
//   offsetY - Y offset for rendering the maze in pixels.
void render_maze(int offsetX, int offsetY);

// Renders Pac-Man at his current position with animation.
// Parameters:
//   offsetX - X offset for rendering Pac-Man in pixels.
//   offsetY - Y offset for rendering Pac-Man in pixels.
void render_pacman(int offsetX, int offsetY);

// Renders Pac-Man's death animation.
// Parameters:
//   offsetX - X offset for rendering the death animation in pixels.
//   offsetY - Y offset for rendering the death animation in pixels.
void render_pacman_death(int offsetX, int offsetY);

// Renders all ghosts with their respective states and animations.
// Parameters:
//   offsetX - X offset for rendering ghosts in pixels.
//   offsetY - Y offset for rendering ghosts in pixels.
void render_ghosts(int offsetX, int offsetY);

// Renders the fruit if active at its current position.
// Parameters:
//   offsetX - X offset for rendering the fruit in pixels.
//   offsetY - Y offset for rendering the fruit in pixels.
void render_fruit(int offsetX, int offsetY);

#endif // RENDERING_H