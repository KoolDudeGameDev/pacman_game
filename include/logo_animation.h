#ifndef LOGO_ANIMATION_H
#define LOGO_ANIMATION_H

#include "all_libs.h"
#include "raylib.h"
#include "game_logic.h"

// Structure to hold logo animation state
typedef struct {
    // Personal logo
    float personalTimer;    // Timer for personal logo animation
    float personalAlpha;    // Alpha for fade-in/fade-out
    int personalCharIndex;  // Current character being typed
    int personalLine;       // Current line being typed (0, 1, or 2)
    float personalTypingTimer; // Timer for typing speed
    float spriteY;          // Y position of KGI sprite for bounce
    float spriteVelocity;   // Velocity for sprite bounce
    bool cursorVisible;     // Blinking cursor visibility
    float cursorTimer;      // Timer for cursor blinking
    
    // Raylib logo
    int logoPositionX;
    int logoPositionY;
    int framesCounter;
    int lettersCount;
    int topSideRecWidth;
    int leftSideRecHeight;
    int bottomSideRecWidth;
    int rightSideRecHeight;
    int logoState;
    float alpha;

    // Game logo
    float timer;            // Timer for game logo
    float alphaGeneral;     // Alpha for overall fade
    float pacmanTextScale;  // Scale for pulsing "Pac-Man" text
    float remakeTextAlpha;  // Alpha for "A Classic Remake" fade-in
    float borderColorTimer; // Timer for border color cycling
    float pacmanSpriteX;    // X position of animated Pac-Man sprite
    float promptBlinkTimer; // Timer for blinking "Press Enter to Start"
    bool promptVisible;     // Visibility of prompt text
    bool enterPressed;      // Flag to track if Enter was pressed
    float enterPressedTime; // Time when Enter was pressed
    float ghostAnimTimer;   // Timer for ghost animation
    int ghostCurrentFrame;  // Current frame for ghost animation
} LogoAnimation;

// Functions for Personal logo
void init_personal_logo(LogoAnimation* anim, int screenWidth, int screenHeight);
bool update_personal_logo(LogoAnimation* anim);
void render_personal_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font, Sound sfx);

// Functions for Raylib logo
void init_raylib_logo(LogoAnimation* anim, int screenWidth, int screenHeight);
bool update_raylib_logo(LogoAnimation* anim);
void render_raylib_logo(const LogoAnimation* anim);

// Functions for Game logo
void init_game_logo(LogoAnimation* anim);
bool update_game_logo(LogoAnimation* anim);
void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font, Texture2D pacmanSprite, Ghost* ghosts, Sound sfxEnter, Sound sfxBgm);

#endif