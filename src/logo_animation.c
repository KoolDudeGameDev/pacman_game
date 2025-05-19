#include "logo_animation.h"

// Static text for personal logo
static const char* personalText[] = {
    "Made by Kyle Gregory Ibo",
    "Cebu Technological University - Ginatilan Ext. Campus",
    "BIT - CompTech"
};

// Static text for game logo
static const char* gameText[] = {
    "Pac-Man",
    "A Classic Remake"
};

// Personal Logo Functions
// ----------------------------------------------------------------------------------------
void init_personal_logo(LogoAnimation* anim, int screenWidth, int screenHeight) {
    anim->personalTimer = 0.0f;
    anim->personalAlpha = 0.0f;
    anim->personalCharIndex = 0;
    anim->personalLine = 0;
    anim->personalTypingTimer = 0.0f;
    anim->spriteY = screenHeight / 2.0f - 50.0f; // Start above text
    anim->spriteVelocity = 0.0f;
    anim->cursorVisible = true;
    anim->cursorTimer = 0.0f;
}

bool update_personal_logo(LogoAnimation* anim) {
    anim->personalTimer += GetFrameTime();
    anim->personalTypingTimer += GetFrameTime();
    anim->cursorTimer += GetFrameTime();

    // Fade-in (0-1s)
    if (anim->personalTimer < 1.0f) {
        anim->personalAlpha = anim->personalTimer / 1.0f;
    }
    // Main animation (1-9s, typing until ~7.88s, then pause)
    else if (anim->personalTimer < 9.0f) {
        anim->personalAlpha = 1.0f;
        // Typing effect (0.08s per character)
        if (anim->personalTypingTimer >= 0.08f && anim->personalLine < 3) {
            anim->personalCharIndex++;
            anim->personalTypingTimer = 0.0f;
            // Move to next line when current line is complete
            if (anim->personalCharIndex > (int)strlen(personalText[anim->personalLine])) {
                anim->personalLine++;
                anim->personalCharIndex = 0;
            }
        }
        // Stop cursor after typing is complete
        if (anim->personalLine >= 3) {
            anim->cursorVisible = false;
        }
    }
    // Fade-out (9-10s)
    else if (anim->personalTimer < 10.0f) {
        // Ensure all text is typed before fading out
        if (anim->personalLine >= 3) {
            anim->personalAlpha = 1.0f - (anim->personalTimer - 9.0f) / 1.0f;
        } else {
            // If typing isn't complete, extend the main phase
            anim->personalTimer = 9.0f;
        }
    }

    // Cursor blink (0.5s interval, only if typing is ongoing)
    if (anim->cursorTimer >= 0.5f && anim->personalLine < 3) {
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

    // Return true when animation is complete (after 10 seconds and all text is typed)
    return anim->personalTimer >= 10.0f && anim->personalLine >= 3;
}

void render_personal_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font, Sound sfx) {
    ClearBackground(BLACK);

    int fontSize = 20; // Pixelated font size
    float lineSpacing = 30.0f;
    static int lastCharIndex = -1; // Track last character index for sound
    static int lastLine = -1;      // Track last line for sound

    // Render text lines with typing effect
    float startY = screenHeight / 2.0f + 20.0f;
    for (int line = 0; line <= anim->personalLine && line < 3; line++) {
        int charsToShow = (line == anim->personalLine) ? anim->personalCharIndex : (int)strlen(personalText[line]);
        char temp[256] = {0};
        strncpy(temp, personalText[line], charsToShow);
        int textWidth = MeasureTextEx(font, temp, fontSize, 1).x;
        Vector2 textPos = { screenWidth / 2.0f - textWidth / 2.0f, startY + line * lineSpacing };
        DrawTextEx(font, temp, textPos, fontSize, 1, Fade(WHITE, anim->personalAlpha));

        // Draw blinking cursor for current line
        if (line == anim->personalLine && anim->cursorVisible && anim->personalLine < 3) {
            char cursorText[256] = {0};
            strncpy(cursorText, personalText[line], charsToShow);
            strcat(cursorText, "_");
            textWidth = MeasureTextEx(font, cursorText, fontSize, 1).x;
            textPos.x = screenWidth / 2.0f - textWidth / 2.0f;
            DrawTextEx(font, cursorText, textPos, fontSize, 1, Fade(WHITE, anim->personalAlpha));
        }
    }

    // Play typing sound when a new character is rendered
    if ((anim->personalCharIndex != lastCharIndex || anim->personalLine != lastLine) && 
        anim->personalLine < 3 && anim->personalCharIndex <= (int)strlen(personalText[anim->personalLine])) {
        if (!IsSoundPlaying(sfx) && !soundMuted) {
            PlaySound(sfx);
        }
        lastCharIndex = anim->personalCharIndex;
        lastLine = anim->personalLine;
    }

    // Render bouncing "KGI" sprite (text-based placeholder)
    const char* kgiText = "KGI";
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
    anim->pacmanTextScale = 1.0f;
    anim->remakeTextAlpha = 0.0f;
    anim->borderColorTimer = 0.0f;
    anim->pacmanSpriteX = -32.0f; // Start off-screen
    anim->promptBlinkTimer = 0.0f;
    anim->promptVisible = true;
    anim->enterPressed = false;
    anim->enterPressedTime = 0.0f;
    anim->ghostAnimTimer = 0.0f;
    anim->ghostCurrentFrame = 0;
}

bool update_game_logo(LogoAnimation* anim) {
    anim->timer += GetFrameTime();
    anim->borderColorTimer += GetFrameTime();
    anim->promptBlinkTimer += GetFrameTime();
    anim->ghostAnimTimer += GetFrameTime();

    // Fade-in (0-1s)
    if (anim->timer < 1.0f) {
        anim->alphaGeneral = anim->timer / 1.0f;
        anim->pacmanTextScale = 1.0f + 0.1f * sinf(anim->timer * 4.0f);
        anim->remakeTextAlpha = (anim->timer - 0.5f) / 0.5f; // Start fading at 0.5s
        if (anim->remakeTextAlpha < 0.0f) anim->remakeTextAlpha = 0.0f;
    }
    // Main animation (until Enter is pressed)
    else if (!anim->enterPressed) {
        anim->alphaGeneral = 1.0f;
        anim->pacmanTextScale = 1.0f + 0.1f * sinf(anim->timer * 4.0f);
        anim->remakeTextAlpha = 1.0f;
        // Move Pac-Man sprite
        anim->pacmanSpriteX += 100.0f * GetFrameTime(); // Move right
        if (anim->pacmanSpriteX > GetScreenWidth() + 32.0f) {
            anim->pacmanSpriteX = -32.0f; // Reset to left
        }
        // Blink prompt (0.5s interval)
        if (anim->promptBlinkTimer >= 0.5f) {
            anim->promptVisible = !anim->promptVisible;
            anim->promptBlinkTimer = 0.0f;
        }
    }
    // Fade-out (1s after Enter is pressed)
    else {
        anim->alphaGeneral = 1.0f - (anim->timer - anim->enterPressedTime) / 1.0f;
        anim->pacmanTextScale = 1.0f;
        anim->remakeTextAlpha = anim->alphaGeneral;
        if (anim->timer >= anim->enterPressedTime + 1.0f) {
            return true; // Animation complete
        }
    }

    // Cycle border color every 0.5s
    if (anim->borderColorTimer >= 0.5f) {
        anim->borderColorTimer = 0.0f;
    }

    // Update ghost animation (0.2s per frame, matches render_ghosts)
    if (anim->ghostAnimTimer >= 0.2f) {
        anim->ghostCurrentFrame = (anim->ghostCurrentFrame + 1) % 2;
        anim->ghostAnimTimer = 0.0f;
    }

    return false; // Continue until Enter is pressed and fade-out completes
}

void render_game_logo(const LogoAnimation* anim, int screenWidth, int screenHeight, Font font, Texture2D pacmanSprite, Ghost* ghosts, Sound sfxEnter, Sound sfxBgm) {
    ClearBackground(BLACK);

    // Draw faded maze background (simplified pattern)
    for (int y = 0; y < screenHeight; y += 20) {
        for (int x = 0; x < screenWidth; x += 20) {
            if ((x / 20 + y / 20) % 2 == 0) {
                DrawRectangle(x, y, 10, 10, Fade(BLUE, 0.1f * anim->alphaGeneral));
            }
        }
    }

    // Draw box with animated border
    int boxWidth = 400;
    int boxHeight = 120;
    int boxX = screenWidth / 2 - boxWidth / 2;
    int boxY = screenHeight / 2 - boxHeight / 2;
    Color borderColor;
    float colorPhase = anim->borderColorTimer / 0.5f;
    if (colorPhase < 0.33f) {
        borderColor = YELLOW;
    } else if (colorPhase < 0.66f) {
        borderColor = WHITE;
    } else {
        borderColor = RED;
    }
    DrawRectangleLinesEx((Rectangle){boxX - 4, boxY - 4, boxWidth + 8, boxHeight + 8}, 4, Fade(borderColor, anim->alphaGeneral));
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f * anim->alphaGeneral));

    // Draw title text
    int pacmanFontSize = 48;
    const char* pacmanText = "Pac-Man";
    Vector2 pacmanTextSize = MeasureTextEx(font, pacmanText, pacmanFontSize, 1);
    Vector2 pacmanTextPos = { screenWidth / 2 - pacmanTextSize.x / 2, boxY + 20 };
    DrawTextEx(font, pacmanText, pacmanTextPos, pacmanFontSize, 1, Fade(YELLOW, anim->alphaGeneral * anim->pacmanTextScale));

    int remakeFontSize = 24;
    const char* remakeText = "A Classic Remake";
    Vector2 remakeTextSize = MeasureTextEx(font, remakeText, remakeFontSize, 1);
    Vector2 remakeTextPos = { screenWidth / 2 - remakeTextSize.x / 2, boxY + 80 };
    DrawTextEx(font, remakeText, remakeTextPos, remakeFontSize, 1, Fade(WHITE, anim->alphaGeneral * anim->remakeTextAlpha));

    // Draw Pac-Man sprite with eating animation
    float scaleFactor = 32.0f / 16.0f; // Scale sprite to 32x32 pixels
    static float pacmanAnimTimer = 0.0f;
    static int pacmanFrame = 0;
    pacmanAnimTimer += GetFrameTime();
    if (pacmanAnimTimer >= 0.08f) { // Match render_pacman animation speed
        pacmanFrame = (pacmanFrame + 1) % 3; // 3 frames: closed, half-open, fully open
        pacmanAnimTimer = 0.0f;
    }
    float pacmanXOffset = (pacmanFrame == 0) ? 0.0f : (pacmanFrame == 1) ? 18.0f : 36.0f;
    Rectangle pacmanSourceRec = { pacmanXOffset, 0.0f, 16.0f, 16.0f };
    Rectangle pacmanDestRec = { anim->pacmanSpriteX, boxY - 32, 32.0f, 32.0f };
    Vector2 pacmanOrigin = { 16.0f, 16.0f };
    DrawTexturePro(pacmanSprite, pacmanSourceRec, pacmanDestRec, pacmanOrigin, 0.0f, Fade(WHITE, anim->alphaGeneral));

    // Draw ghost sprites trailing Pac-Man
    for (int i = 0; i < MAX_GHOSTS; i++) {
        // Ghost sprite coordinates (Blinky: y=64, Pinky: y=80, Inky: y=96, Clyde: y=112)
        float yOffset;
        switch (i) {
            case 0: yOffset = 64.0f; break; // Blinky (red)
            case 1: yOffset = 80.0f; break; // Pinky (pink)
            case 2: yOffset = 96.0f; break; // Inky (cyan)
            case 3: yOffset = 112.0f; break; // Clyde (orange)
            default: yOffset = 64.0f; break;
        }
        // Use right-facing sprite (x=4 or x=20 based on frame)
        float xOffset = 4.0f + anim->ghostCurrentFrame * 16.0f;
        Rectangle ghostSourceRec = { xOffset, yOffset, 16.0f, 16.0f };
        // Position ghosts trailing Pac-Man (40px apart)
        Rectangle ghostDestRec = { anim->pacmanSpriteX - 40.0f * (i + 1), boxY - 32, 32.0f, 32.0f };
        DrawTexturePro(ghosts[i].normalSprite[anim->ghostCurrentFrame], ghostSourceRec, ghostDestRec, pacmanOrigin, 0.0f, Fade(WHITE, anim->alphaGeneral));
    }

    // Draw prompt
    if (anim->promptVisible) {
        const char* promptText = "Press Enter to Start";
        int promptFontSize = 20;
        Vector2 promptTextSize = MeasureTextEx(font, promptText, promptFontSize, 1);
        Vector2 promptTextPos = { screenWidth / 2 - promptTextSize.x / 2, boxY + boxHeight + 40 };
        DrawTextEx(font, promptText, promptTextPos, promptFontSize, 1, Fade(WHITE, anim->alphaGeneral));
    }

    // Draw CRT scanlines
    for (int y = 0; y < screenHeight; y += 4) {
        DrawRectangle(0, y, screenWidth, 2, Fade(BLACK, 0.2f * anim->alphaGeneral));
    }

    // Play background jingle
    if (!IsSoundPlaying(sfxBgm) && !soundMuted) {
        PlaySound(sfxBgm);
    }
}