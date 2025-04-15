#include "logo_animation.h"

// KoolDude Logo Functions
// ----------------------------------------------------------------------------------------
void init_kool_dude_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    int fontSize = 80; // Scaled for 1280x720
    anim->devLogoPositionX = screenWidth / 2 - MeasureText("Kool_Dude", fontSize) / 2;
    anim->devLogoPositionY = screenHeight / 2 - fontSize / 2;
    anim->devLogoAlpha = 0.0f;
    anim->devFramesCounter = 0;
}

bool update_kool_dude_logo(LogoAnimation* anim) {
    anim->devFramesCounter++;

    // Fade in and out animation
    if (anim->devFramesCounter < 60) {
        anim->devLogoAlpha = (float)anim->devFramesCounter / 60.0f;
    } else if (anim->devFramesCounter < 180) {
        anim->devLogoAlpha = 1.0f;
    } else if (anim->devFramesCounter < 240) {
        anim->devLogoAlpha = 1.0f - ((float)(anim->devFramesCounter - 180) / 60.0f);
    }

    // Return true if the animation is complete (after 240 frames, ~4 seconds at 60 FPS)
    return anim->devFramesCounter >= 240;
}

void render_kool_dude_logo(const LogoAnimation* anim, Font font) {
    ClearBackground(BLACK);
    int fontSize = 80; // Scaled for 1280x720
    DrawTextEx(font, "Kool_Dude", (Vector2){anim->devLogoPositionX, anim->devLogoPositionY}, fontSize, 1, Fade(WHITE, anim->devLogoAlpha));
}

// xAI Dev Logo Functions
// ----------------------------------------------------------------------------------------
void init_dev_logo(LogoAnimation* anim) {
    anim->timer = 0.0f;
    anim->alphaGeneral = 0.0f;
    anim->scale = 1.0f; // Not used anymore, but kept for struct compatibility
}

bool update_dev_logo(LogoAnimation* anim) {
    anim->timer += 1.0f; // Increment as a frame counter (1 frame per update at 60 FPS)

    // Fade-in for first second (60 frames), stay visible for 2 seconds (120 frames), fade-out for 1 second (60 frames)
    if (anim->timer < 60) {
        anim->alphaGeneral = anim->timer / 60.0f;
    } else if (anim->timer < 180) {
        anim->alphaGeneral = 1.0f;
    } else if (anim->timer < 240) {
        anim->alphaGeneral = 1.0f - ((anim->timer - 180) / 60.0f);
    }

    // Return true if the animation is complete (after 240 frames, ~4 seconds at 60 FPS)
    return anim->timer >= 240;
}

void render_dev_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font) {
    ClearBackground(BLACK);
    int fontSize = 40; // Scaled for 1280x720
    int textWidth = MeasureText("xAI Dev Team", fontSize);
    Vector2 textPos = {screenWidth / 2 - textWidth / 2, screenHeight / 2};
    DrawTextEx(font, "xAI Dev Team", textPos, fontSize, 1, Fade(WHITE, anim->alphaGeneral));
}

// Raylib Logo Functions
// ----------------------------------------------------------------------------------------
void init_raylib_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    // Use the original 256x256 size, centered on the 1280x720 screen
    anim->logoPositionX = screenWidth / 2 - 128; // 128 = 256/2
    anim->logoPositionY = screenHeight / 2 - 128;
    anim->framesCounter = 0;
    anim->lettersCount = 0;
    anim->topSideRecWidth = 16;    // Original size, not scaled
    anim->leftSideRecHeight = 16;
    anim->bottomSideRecWidth = 16;
    anim->rightSideRecHeight = 16;
    anim->logoState = 0;
    anim->alpha = 1.0f;
}

bool update_raylib_logo(LogoAnimation* anim) {
    if (anim->logoState == 0) { // State 0: Small box blinking
        anim->framesCounter++;
        if (anim->framesCounter == 120) {
            anim->logoState = 1;
            anim->framesCounter = 0;
        }
    } else if (anim->logoState == 1) { // State 1: Top and left bars growing
        anim->topSideRecWidth += 4;
        anim->leftSideRecHeight += 4;
        if (anim->topSideRecWidth == 256) anim->logoState = 2;
    } else if (anim->logoState == 2) { // State 2: Bottom and right bars growing
        anim->bottomSideRecWidth += 4;
        anim->rightSideRecHeight += 4;
        if (anim->bottomSideRecWidth == 256) anim->logoState = 3;
    } else if (anim->logoState == 3) { // State 3: Letters appearing (one by one)
        anim->framesCounter++;
        if (anim->framesCounter / 12) {
            anim->lettersCount++;
            anim->framesCounter = 0;
        }
        if (anim->lettersCount >= 10) {
            anim->alpha -= 0.02f;
            if (anim->alpha <= 0.0f) {
                anim->alpha = 0.0f;
                // Reset logo variables for potential replay
                anim->framesCounter = 0;
                anim->lettersCount = 0;
                anim->topSideRecWidth = 16;
                anim->leftSideRecHeight = 16;
                anim->bottomSideRecWidth = 16;
                anim->rightSideRecHeight = 16;
                anim->alpha = 1.0f;
                anim->logoState = 0;
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

    if (anim->timer < 1.0f) {
        anim->alphaGeneral = anim->timer / 1.0f;
    } else if (anim->timer >= 1.0f && anim->timer < 2.0f) {
        anim->alphaGeneral = 1.0f;
    } else if (anim->timer >= 2.0f) {
        anim->alphaGeneral = 1.0f - (anim->timer - 2.0f) / 1.0f;
    }

    return anim->timer >= 3.0f;
}

void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font) {
    ClearBackground(BLACK);
    int fontSize = 40; // Scaled for 1280x720
    Color textColor = Fade(YELLOW, anim->alphaGeneral);
    DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, fontSize, 1, textColor);
    textColor = Fade(WHITE, anim->alphaGeneral);
    DrawTextEx(font, "A Classic Remake", (Vector2){screenWidth / 2 - 70, screenHeight / 2}, fontSize, 1, textColor);
}