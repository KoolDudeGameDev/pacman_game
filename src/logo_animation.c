#include "logo_animation.h"

#define PI 3.14159265359f

// KoolDude Logo Functions
// ----------------------------------------------------------------------------------------

void init_kool_dude_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    anim->devLogoPositionX = screenWidth / 2 - MeasureText("Kool_Dude", 50) / 2;
    anim->devLogoPositionY = screenHeight / 2 - 25; // Adjust for font size
    anim->devLogoAlpha = 0.0f;
    anim->devFramesCounter = 0;
}

bool update_kool_dude_logo(LogoAnimation* anim) {
    anim->devFramesCounter ++;

    // Fade in and out animation
    if (anim->devFramesCounter < 60) {
        anim->devLogoAlpha = (float)anim->devFramesCounter / 60.0f;
    } else if (anim->devFramesCounter < 180) {
        anim->devLogoAlpha = 1.0f;
    } else if (anim->devFramesCounter < 240) {
        anim->devLogoAlpha = ((float)(anim->devFramesCounter - 180) / 60.0f);
    }

    // Return true if animation is complete(after 240 frames,  ~4 seconds at 60fps)
    return anim->devFramesCounter >= 240;
}

void render_kool_dude_logo(const LogoAnimation* anim) {
    ClearBackground(BLACK);
    DrawText("Kool_Dude", anim->devLogoPositionX, anim->devLogoPositionY, 59, Fade(WHITE, anim->devLogoAlpha));
}

// xAI Dev Logo Functions
// ----------------------------------------------------------------------------------------
void init_dev_logo(LogoAnimation* anim) {
    anim->timer = 0.0f;
    anim->alphaGeneral = 0.0f;
    anim->scale = 1.0f;
}

bool update_dev_logo(LogoAnimation* anim) {
    anim->timer += GetFrameTime();

    // Fade-in for first second, stay visible, then fade-out
    if (anim->timer < 1.0f) {
        anim->alphaGeneral = anim->timer / 1.0f;
    } else if (anim->timer >= 1.0f && anim->timer < 2.0f) {
        anim->alphaGeneral = 1.0f;
    } else if (anim->timer >= 2.0f && anim->timer < 3.0f) {
        anim->alphaGeneral = 1.0f - (anim->timer - 2.0f) / 1.0f;
    }

    // Scaling animation: oscillate between 0.9 and 1.1
    anim->scale = 1.0f + 0.1f * sinf(anim->timer * 2.0f * PI);

    // Return true if the animation is complete (after 3 seconds)
    return anim->timer >= 3.0f;
}

void render_dev_logo(const LogoAnimation* anim, int screenWidth, int screenHeight) {
    ClearBackground(BLACK);
    Color textColor = Fade(WHITE, anim->alphaGeneral);
    int fontSize = (int)(20 * anim->scale);
    int textWidth = MeasureText("xAI Dev Team", fontSize);
    DrawText("xAI Dev Team", screenWidth / 2 - textWidth / 2, screenHeight / 2, fontSize, textColor);
}

// Raylib Logo Functions
// ----------------------------------------------------------------------------------------
void init_raylib_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    anim->logoPositionX = screenWidth / 2 - 128;
    anim->logoPositionY = screenHeight / 2 - 128;
    anim->framesCounter = 0;
    anim->lettersCount = 0;
    anim->topSideRecWidth = 16;
    anim->leftSideRecHeight = 16;
    anim->bottomSideRecWidth = 16;
    anim->rightSideRecHeight = 16;
    anim->logoState = 0;
    anim->alpha = 1.0f;
}

bool update_raylib_logo(LogoAnimation* anim) {
    if (anim->logoState == 0) {
        anim->framesCounter++;
        if (anim->framesCounter == 120) {
            anim->logoState = 1;
            anim->framesCounter = 0;
        }
    } else if (anim->logoState == 1) {
        anim->topSideRecWidth += 4;
        anim->leftSideRecHeight += 4;
        if (anim->topSideRecWidth == 256) anim->logoState = 2;
    } else if (anim->logoState == 2) {
        anim->bottomSideRecWidth += 4;
        anim->rightSideRecHeight += 4;
        if (anim->bottomSideRecWidth == 256) anim->logoState = 3;
    } else if (anim->logoState == 3) {
        anim->framesCounter++;
        if (anim->framesCounter / 12) {
            anim->lettersCount++;
            anim->framesCounter = 0;
        }
        if (anim->lettersCount >= 10) {
            anim->alpha -= 0.02f;
            if (anim->alpha <= 0.0f) {
                anim->alpha = 0.0f;
                return true; // Animation complete
            }
        }
    }
    return false;
}

void render_raylib_logo(const LogoAnimation* anim) {
    ClearBackground(RAYWHITE);

    if (anim->logoState == 0) {
        if ((anim->framesCounter / 15) % 2) {
            DrawRectangle(anim->logoPositionX, anim->logoPositionY, 16, 16, BLACK);
        }
    } else if (anim->logoState == 1) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, 16, BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, 16, anim->leftSideRecHeight, BLACK);
    } else if (anim->logoState == 2) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, 16, BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, 16, anim->leftSideRecHeight, BLACK);
        DrawRectangle(anim->logoPositionX + 240, anim->logoPositionY, 16, anim->rightSideRecHeight, BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + 240, anim->bottomSideRecWidth, 16, BLACK);
    } else if (anim->logoState == 3) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, 16, Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + 16, 16, anim->leftSideRecHeight - 32, Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX + 240, anim->logoPositionY + 16, 16, anim->rightSideRecHeight - 32, Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + 240, anim->bottomSideRecWidth, 16, Fade(BLACK, anim->alpha));
        DrawRectangle(GetScreenWidth() / 2 - 112, GetScreenHeight() / 2 - 112, 224, 224, Fade(RAYWHITE, anim->alpha));
        DrawText(TextSubtext("raylib", 0, anim->lettersCount), GetScreenWidth() / 2 - 44, GetScreenHeight() / 2 + 48, 50, Fade(BLACK, anim->alpha));
    }
}

// Game Logo Functions
// ----------------------------------------------------------------------------------------
void init_game_logo(LogoAnimation* anim) {
    anim->timer = 0.0f;
    anim->alphaGeneral = 0.0f;
    anim->scale = 1.0f;
}

bool update_game_logo(LogoAnimation* anim) {
    anim->timer += GetFrameTime();

    if (anim-> timer < 1.0f) {
        anim->alphaGeneral = anim->timer / 1.0f;
    } else if (anim->timer >= 1.0f && anim->timer < 2.0f) {
        anim->alphaGeneral = 1.0f;
    } else if (anim->timer >= 2.0f) {
        anim->alphaGeneral = 1.0f - (anim->timer - 2.0f) / 1.0f;
    }

    return anim->timer >= 3.0f;
}

void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight) {
    ClearBackground(BLACK);
    Color textColor = Fade(YELLOW, anim->alphaGeneral);
    DrawText("Pac-Man", screenWidth / 2 - 50, screenHeight / 2 - 40, 20, textColor);
    textColor = Fade(WHITE, anim->alphaGeneral);
    DrawText("A Classic Remake", screenWidth / 2 - 70, screenHeight / 2, 20, textColor);
}