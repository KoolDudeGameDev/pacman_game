#ifndef LOGO_ANIMATION_H
#define LOGO_ANIMATION_H

#include "all_libs.h"
#include "raylib.h"
#include "game_logic.h"

// Structure to hold logo animation state
typedef struct {
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
    float timer;        // Timer for Game logo
    float alphaGeneral; // Alpha value for fade-in/fade-out
    float scale;        // Scale value for pulsing animation

    // Personal logo
    float personalTimer;           // Timer for personal logo animation
    float personalAlpha;           // Alpha for fade-in/fade-out
    int currentCharIndex;          // Current character being typed
    int currentLine;               // Current line being typed (0, 1, or 2)
    float typingTimer;             // Timer for typing speed
    float spriteY;                 // Y position of KGI sprite for bounce
    float spriteVelocity;          // Velocity for sprite bounce
    bool cursorVisible;            // Blinking cursor visibility
    float cursorTimer;             // Timer for cursor blinking
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
void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font);

#endif