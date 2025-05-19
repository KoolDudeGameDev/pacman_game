#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include "game_logic.h"

// Initializes GUI-related variables, such as selected menu options.
void init_gui(void);

// Handles input for the main menu (STATE_MENU), updating the selected option and triggering state changes.
// Parameters:
//   selectedOption - Pointer to the currently selected menu option (0: Start, 1: Highscores, etc.).
//   shouldExit - Pointer to the flag indicating if the game should exit.
void handle_menu_input(int *selectedOption, bool *shouldExit);

// Handles input for the high scores screen (STATE_HIGHSCORES), returning to the menu on key press.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_highscores_input(int *selectedOption);

// Handles input for the about screen (STATE_ABOUT), returning to the menu on key press.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_about_input(int *selectedOption);

// Handles input for the settings menu (STATE_SETTINGS), adjusting volume and mute settings.
// Parameters:
//   selectedOption - Pointer to the currently selected settings option (0: BG Music, 1: SFX, 2: Mute).
void handle_settings_input(int *selectedOption);

// Handles input for the pause menu (STATE_PAUSED), navigating options and settings.
// Parameters:
//   pausedThisFrame - Pointer to the flag tracking if pause was toggled this frame.
// Returns: true if the game should transition to another state (e.g., STATE_PLAYING or STATE_MENU), false otherwise.
bool handle_pause_input(bool *pausedThisFrame);

// Handles input for the game over screen (STATE_GAME_OVER), managing name input and returning to the menu.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_game_over_input(int *selectedOption);

void render_menu(int screenWidth, int screenHeight, Font font, int selectedOption, Texture2D pacmanSprite, Ghost* ghostArray);

// Renders the high scores screen (STATE_HIGHSCORES) with the top scores.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_highscores(int screenWidth, int screenHeight, Font font);

// Renders the about screen (STATE_ABOUT) with game information.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_about(int screenWidth, int screenHeight, Font font);

// Renders the settings menu (STATE_SETTINGS) with volume sliders and mute toggle.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
//   selectedOption - Currently selected settings option.
void render_settings_menu(int screenWidth, int screenHeight, Font font, int selectedOption);

// Renders the pause menu (STATE_PAUSED) with options to resume, restart, adjust settings, or quit.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_pause_menu(int screenWidth, int screenHeight, Font font);

// Renders the level complete screen with score breakdown and progress bar.
void render_level_complete(int screenWidth, int screenHeight, Font font, float animTimer, int currentLevel,
                           int eatenPellets, int eatenPowerPellets, int eatenGhosts,
                           int collectedFruits, int fruitPoints);

// Renders the game over screen (STATE_GAME_OVER), including name input and high scores.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
//   gameOverFadeAlpha - Alpha value for fade-in/out effects.
void render_game_over(int screenWidth, int screenHeight, Font font, float gameOverFadeAlpha);

#endif // GUI_H