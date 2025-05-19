#include "logo_animation.h"

// Static text for personal logo
static const char* personalText[] = {
    "Made by Kyle Gregory Ibo",
    "Cebu Technological University - Ginatilan Ext. Campus",
    "BIT - CompTech"
};

// Personal Logo Functions
// ----------------------------------------------------------------------------------------
void init_personal_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    anim->personalTimer = 0.0f;
    anim->personalAlpha = 0.0f;
    anim->currentCharIndex = 0;
    anim->currentLine = 0;
    anim->typingTimer = 0.0f;
    anim->spriteY = screenHeight / 2.0f - 50.0f; // Start above text
    anim->spriteVelocity = 0.0f;
    anim->cursorVisible = true;
    anim->cursorTimer = 0.0f;
}

bool update_personal_logo(LogoAnimation* anim) {
    anim->personalTimer += GetFrameTime();
    anim->typingTimer += GetFrameTime();
    anim->cursorTimer += GetFrameTime();

    // Fade-in (0-1s)
    if (anim->personalTimer < 1.0f) {
        anim->personalAlpha = anim->personalTimer / 1.0f;
    }
    // Main animation (1-10s, or until typing is complete with a pause)
    else if (anim->personalTimer < 10.0f) {
        anim->personalAlpha = 1.0f;
        // Typing effect (0.1s per character)
        if (anim->typingTimer >= 0.08f && anim->currentLine < 3) {
            anim->currentCharIndex++;
            anim->typingTimer = 0.0f;
            // Move to next line when current line is complete
            if (anim->currentCharIndex > (int)strlen(personalText[anim->currentLine])) {
                anim->currentLine++;
                anim->currentCharIndex = 0;
            }
        }
    }
    // Fade-out (10-11s, or after typing is complete)
    else if (anim->personalTimer < 11.0f) {
        // Ensure all text is typed before fading out
        if (anim->currentLine >= 3) {
            anim->personalAlpha = 1.0f - (anim->personalTimer - 10.0f) / 1.0f;
        } else {
            // If typing isn't complete, extend the main phase
            anim->personalTimer = 10.0f;
        }
    }

    // Cursor blink (0.5s interval)
    if (anim->cursorTimer >= 0.5f) {
        anim->cursorVisible = !anim->cursorVisible;
        anim->cursorTimer = 0.0f;
    }

    // Sprite bounce (simple physics)
    anim->spriteVelocity += 300.0f * GetFrameTime(); // Gravity
    anim->spriteY += anim->spriteVelocity * GetFrameTime();
    // Bounce when hitting bottom boundary
    if (anim->spriteY > GetScreenHeight() / 2.0f - 20.0f) {
        anim->spriteY = GetScreenHeight() / 2.0f - 20.0f;
        anim->spriteVelocity = -150.0f; // Bounce upward
    }

    // Return true when animation is complete (after 11 seconds and all text is typed)
    return anim->personalTimer >= 11.0f && anim->currentLine >= 3;
}

void render_personal_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font, Sound sfx) {
    ClearBackground(BLACK);

    int fontSize = 20; // Pixelated font size
    float lineSpacing = 30.0f;
    static int lastCharIndex = -1; // Track last character index for sound
    static int lastLine = -1;      // Track last line for sound

    // Render text lines with typing effect
    float startY = screenHeight / 2.0f + 20.0f;
    for (int line = 0; line <= anim->currentLine && line < 3; line++) {
        int charsToShow = (line == anim->currentLine) ? anim->currentCharIndex : (int)strlen(personalText[line]);
        char temp[256] = {0};
        strncpy(temp, personalText[line], charsToShow);
        int textWidth = MeasureTextEx(font, temp, fontSize, 1).x;
        Vector2 textPos = { screenWidth / 2.0f - textWidth / 2.0f, startY + line * lineSpacing };
        DrawTextEx(font, temp, textPos, fontSize, 1, Fade(WHITE, anim->personalAlpha));

        // Draw blinking cursor for current line
        if (line == anim->currentLine && anim->cursorVisible && anim->currentLine < 3) {
            char cursorText[256] = {0};
            strncpy(cursorText, personalText[line], charsToShow);
            strcat(cursorText, "_");
            textWidth = MeasureTextEx(font, cursorText, fontSize, 1).x;
            textPos.x = screenWidth / 2.0f - textWidth / 2.0f;
            DrawTextEx(font, cursorText, textPos, fontSize, 1, Fade(WHITE, anim->personalAlpha));
        }
    }

    // Play typing sound when a new character is rendered
    if ((anim->currentCharIndex != lastCharIndex || anim->currentLine != lastLine) && 
        anim->currentLine < 3 && anim->currentCharIndex <= (int)strlen(personalText[anim->currentLine])) {
        if (!IsSoundPlaying(sfx) && !soundMuted) {
            PlaySound(sfx);
        }
        lastCharIndex = anim->currentCharIndex;
        lastLine = anim->currentLine;
    }

    // Render bouncing "KGI" sprite (text-based placeholder)
    const char* kgiText = "Pac-Man Remake";
    int kgiFontSize = 40;
    int kgiWidth = MeasureTextEx(font, kgiText, kgiFontSize, 1).x;
    Vector2 kgiPos = { screenWidth / 2.0f - kgiWidth / 2.0f, anim->spriteY };
    DrawTextEx(font, kgiText, kgiPos, kgiFontSize, 1, Fade(YELLOW, anim->personalAlpha));

    // Render CRT scanlines
    for (int y = 0; y < screenHeight; y += 4) {
        DrawRectangle(0, y, screenWidth, 2, Fade(BLACK, 0.2f * anim->personalAlpha));
    }
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
                anim->framesCounter = 0;
                anim->lettersCount = 0;
                anim->topSideRecWidth = 16;
                anim->leftSideRecHeight = 16;
                anim->bottomSideRecWidth = 16;
                anim->rightSideRecHeight = 16;
                anim->alpha = 1.0f;
                anim->logoState = 0;
                return true;
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
    int fontSize = 40;
    Color textColor = Fade(YELLOW, anim->alphaGeneral);
    DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, fontSize, 1, textColor);
    textColor = Fade(WHITE, anim->alphaGeneral);
    DrawTextEx(font, "A Classic Remake", (Vector2){screenWidth / 2 - 70, screenHeight / 2}, fontSize, 1, textColor);
}