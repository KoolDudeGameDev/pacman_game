#include "logo_animation.h"
#include <math.h>

// KoolDude Logo Functions
// ----------------------------------------------------------------------------------------
void init_kool_dude_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    // Scale the font size for the new resolution (previously 50 on a 400x400 screen)
    int fontSize = 80; // Scaled up for 1280x720 (50 * 1280/400 ≈ 80)
    anim->devLogoPositionX = screenWidth / 2 - MeasureText("Kool_Dude", fontSize) / 2;
    anim->devLogoPositionY = screenHeight / 2 - fontSize / 2; // Center vertically
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

    // Debug print (optional, can remove if not needed)
    TraceLog(LOG_INFO, "KoolDude Logo - Frame: %d, Alpha: %f", anim->devFramesCounter, anim->devLogoAlpha);

    // Return true if the animation is complete (after 240 frames, ~4 seconds at 60 FPS)
    return anim->devFramesCounter >= 240;
}

void render_kool_dude_logo(const LogoAnimation* anim, Font font) {
    ClearBackground(BLACK);
    int fontSize = 80; // Match the font size used in init
    DrawTextEx(font, "Kool_Dude", (Vector2){anim->devLogoPositionX, anim->devLogoPositionY}, fontSize, 1, Fade(WHITE, anim->devLogoAlpha));
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

void render_dev_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font) {
    ClearBackground(BLACK);
    Color textColor = Fade(WHITE, anim->alphaGeneral);
    int baseFontSize = 40; // Scaled up from 20 (20 * 1280/400 ≈ 40)
    int fontSize = (int)(baseFontSize * anim->scale);
    int textWidth = MeasureText("xAI Dev Team", fontSize);
    Vector2 textPos = {screenWidth / 2 - textWidth / 2, screenHeight / 2};
    DrawTextEx(font, "xAI Dev Team", textPos, fontSize, 1, textColor);
}

// Raylib Logo Functions
// ----------------------------------------------------------------------------------------
void init_raylib_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    // Scale the logo size for the new resolution
    float scaleFactor = (float)screenWidth / 400.0f; // Based on the original 400x400 screen
    int logoSize = (int)(256 * scaleFactor); // Original size was 256x256
    anim->logoPositionX = screenWidth / 2 - logoSize / 2;
    anim->logoPositionY = screenHeight / 2 - logoSize / 2;
    anim->framesCounter = 0;
    anim->lettersCount = 0;
    anim->topSideRecWidth = (int)(16 * scaleFactor);    // Original was 16
    anim->leftSideRecHeight = (int)(16 * scaleFactor);
    anim->bottomSideRecWidth = (int)(16 * scaleFactor);
    anim->rightSideRecHeight = (int)(16 * scaleFactor);
    anim->logoState = 0;
    anim->alpha = 1.0f;
}

bool update_raylib_logo(LogoAnimation* anim) {
    float scaleFactor = (float)GetScreenWidth() / 400.0f; // Based on the original 400x400 screen
    if (anim->logoState == 0) {
        anim->framesCounter++;
        if (anim->framesCounter == 120) {
            anim->logoState = 1;
            anim->framesCounter = 0;
        }
    } else if (anim->logoState == 1) {
        anim->topSideRecWidth += (int)(4 * scaleFactor);
        anim->leftSideRecHeight += (int)(4 * scaleFactor);
        if (anim->topSideRecWidth >= (int)(256 * scaleFactor)) anim->logoState = 2;
    } else if (anim->logoState == 2) {
        anim->bottomSideRecWidth += (int)(4 * scaleFactor);
        anim->rightSideRecHeight += (int)(4 * scaleFactor);
        if (anim->bottomSideRecWidth >= (int)(256 * scaleFactor)) anim->logoState = 3;
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
    float scaleFactor = (float)GetScreenWidth() / 400.0f; // Based on the original 400x400 screen
    int logoSize = (int)(256 * scaleFactor);
    int textFontSize = (int)(50 * scaleFactor); // Scaled up from 50

    if (anim->logoState == 0) {
        if ((anim->framesCounter / 15) % 2) {
            DrawRectangle(anim->logoPositionX, anim->logoPositionY, (int)(16 * scaleFactor), (int)(16 * scaleFactor), BLACK);
        }
    } else if (anim->logoState == 1) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, (int)(16 * scaleFactor), BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, (int)(16 * scaleFactor), anim->leftSideRecHeight, BLACK);
    } else if (anim->logoState == 2) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, (int)(16 * scaleFactor), BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, (int)(16 * scaleFactor), anim->leftSideRecHeight, BLACK);
        DrawRectangle(anim->logoPositionX + anim->topSideRecWidth - (int)(16 * scaleFactor), anim->logoPositionY, (int)(16 * scaleFactor), anim->rightSideRecHeight, BLACK);
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + anim->leftSideRecHeight - (int)(16 * scaleFactor), anim->bottomSideRecWidth, (int)(16 * scaleFactor), BLACK);
    } else if (anim->logoState == 3) {
        DrawRectangle(anim->logoPositionX, anim->logoPositionY, anim->topSideRecWidth, (int)(16 * scaleFactor), Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + (int)(16 * scaleFactor), (int)(16 * scaleFactor), anim->leftSideRecHeight - (int)(32 * scaleFactor), Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX + anim->topSideRecWidth - (int)(16 * scaleFactor), anim->logoPositionY + (int)(16 * scaleFactor), (int)(16 * scaleFactor), anim->rightSideRecHeight - (int)(32 * scaleFactor), Fade(BLACK, anim->alpha));
        DrawRectangle(anim->logoPositionX, anim->logoPositionY + anim->leftSideRecHeight - (int)(16 * scaleFactor), anim->bottomSideRecWidth, (int)(16 * scaleFactor), Fade(BLACK, anim->alpha));
        DrawRectangle(GetScreenWidth() / 2 - (int)(112 * scaleFactor), GetScreenHeight() / 2 - (int)(112 * scaleFactor), (int)(224 * scaleFactor), (int)(224 * scaleFactor), Fade(RAYWHITE, anim->alpha));
        DrawText(TextSubtext("raylib", 0, anim->lettersCount), GetScreenWidth() / 2 - (int)(44 * scaleFactor), GetScreenHeight() / 2 + (int)(48 * scaleFactor), textFontSize, Fade(BLACK, anim->alpha));
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
    int fontSize = 40; // Scaled up from 20 (20 * 1280/400 ≈ 40)
    Color textColor = Fade(YELLOW, anim->alphaGeneral);
    DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, fontSize, 1, textColor);
    textColor = Fade(WHITE, anim->alphaGeneral);
    DrawTextEx(font, "A Classic Remake", (Vector2){screenWidth / 2 - 70, screenHeight / 2}, fontSize, 1, textColor);
}