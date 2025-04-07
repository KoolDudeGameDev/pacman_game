#ifndef LOGO_ANIMATION_H
#define LOGO_ANIMATION_H

#include "raylib.h"

// Structure to hold logo animation state
typedef struct {

    // General logo variables
    int logoPositionX;
    int logoPositionY;

    // KoolDude logo (Developer logo)
    int devLogoPositionX;
    int devLogoPositionY;
    float devLogoAlpha;
    int devFramesCounter;

    // Raylib logo
    int framesCounter;
    int lettersCount;
    int topSideRecWidth;
    int leftSideRecHeight;
    int bottomSideRecWidth;
    int rightSideRecHeight;
    int logoState;
    float alpha;

    // xAI Dev and Game logo
    float timer;   // Timer for xAI Dev and Game logo
    float alphaGeneral;   // Alpha value for fade-in/fade-out
    float scale;   // Scale value for pulsing animation
} LogoAnimation;

// Functions for KoolDude logo
void init_kool_dude_logo(LogoAnimation* anim, int screenWidth, int screenHeight);
bool update_kool_dude_logo(LogoAnimation* anim);
void render_kool_dude_logo(const LogoAnimation* anim);

// Functions for xAI Dev logo
void init_dev_logo(LogoAnimation* anim);
bool update_dev_logo(LogoAnimation* anim);
void render_dev_logo(const LogoAnimation* anim, int screenWidth, int screenHeight);

// Functions for Raylib logo
void init_raylib_logo(LogoAnimation* anim, int screenWidth, int screenHeight);
bool update_raylib_logo(LogoAnimation* anim);
void render_raylib_logo(const LogoAnimation* anim);

// Functions for Game logo
void init_game_logo(LogoAnimation* anim);
bool update_game_logo(LogoAnimation* anim);
void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight);

#endif