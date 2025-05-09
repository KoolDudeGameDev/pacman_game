#ifndef RENDERING_H
#define RENDERING_H

#include "raylib.h"
#include "game_logic.h"

// Loads ghost textures and assigns them to the ghost array, handling transparency.
// Parameters:
//   ghostArray - Array of Ghost structures to assign textures to.
void LoadGhostTextures(Ghost *ghostArray);

// Unloads ghost textures to free memory.
// Parameters:
//   ghostArray - Array of Ghost structures whose textures are to be unloaded.
void UnloadGhostTextures(Ghost *ghostArray);

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

// Renders the maze, including walls, pellets, power pellets, and ghost gate.
// Parameters:
//   offsetX - X offset for rendering the maze in pixels.
//   offsetY - Y offset for rendering the maze in pixels.
void render_maze(int offsetX, int offsetY);

void render_pacman(int offsetX, int offsetY);
void render_pacman_death(int offsetX, int offsetY);
void render_ghosts(int offsetX, int offsetY);

// Renders the pause menu (Resume, Restart, Settings, Quit).
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_pause_menu(int screenWidth, int screenHeight, Font font);

// Renders the settings menu with volume sliders and mute toggle.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_settings_menu(int screenWidth, int screenHeight, Font font, int selectedOption);

#endif // RENDERING_H