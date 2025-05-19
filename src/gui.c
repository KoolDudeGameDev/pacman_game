#include "gui.h"
#include "rendering.h"

// Static variables for menu animations
static float menuAnimTimer = 0.0f;
static float pacmanSpriteX = -32.0f;
static float pelletBlinkTimer = 0.0f;
static bool pelletVisible = true;
static float pacmanAnimTimer = 0.0f;
static int pacmanFrame = 0;
static float ghostAnimTimer = 0.0f;
static int ghostFrame = 0;

// Initializes GUI-related variables, such as selected menu options.
void init_gui(void) {
    // Initialize menu selection
    pauseSelectedOption = 0;
    pauseMenuState = PAUSE_MENU_MAIN;
}

// Handles input for the main menu (STATE_MENU), updating the selected option and triggering state changes.
// Parameters:
//   selectedOption - Pointer to the currently selected menu option (0: Start, 1: Highscores, etc.).
//   shouldExit - Pointer to the flag indicating if the game should exit.
void handle_menu_input(int *selectedOption, bool *shouldExit) {
    // Navigate menu options
    if (IsKeyPressed(KEY_DOWN)) {
        *selectedOption = (*selectedOption + 1) % 5;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume); // Apply sfxVolume to menu navigation sound
            PlaySound(sfx_menu_nav);
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        *selectedOption = (*selectedOption - 1 + 5) % 5;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume); // Apply sfxVolume to menu navigation sound
            PlaySound(sfx_menu_nav);
        }
    }
    if (IsKeyPressed(KEY_ENTER)) {
        if (*selectedOption == 0) {      // START
            init_maze();
            level = 1;                  // Reset level to 1 when starting a new game
            pacman.score = 0;           // Reset score for new game
            pacman.lives = 3;           // Reset lives for new game
            totalFruitsCollected = 0;   // Reset fruit count
            reset_game_state(true, STATE_READY);     // Initialize game state and play sfx_ready
            gameState = STATE_READY;
        } else if (*selectedOption == 1) { // HIGHSCORES
            gameState = STATE_HIGHSCORES;
            *selectedOption = 0;
        } else if (*selectedOption == 2) { // ABOUT
            gameState = STATE_ABOUT;
            *selectedOption = 0;    
        } else if (*selectedOption == 3) { // SETTINGS
            gameState = STATE_SETTINGS;
            *selectedOption = 0;
        } else {                        // EXIT
            *shouldExit = true;
        }
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume); // Apply sfxVolume to menu navigation sound
            PlaySound(sfx_menu_nav);
        }
    }
}

// Handles input for the high scores screen (STATE_HIGHSCORES), returning to the menu on key press.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_highscores_input(int *selectedOption) {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        gameState = STATE_MENU;
        *selectedOption = 1;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume); // Apply sfxVolume to menu navigation sound
            PlaySound(sfx_menu_nav);
        }
    }
}

// Handles input for the about screen (STATE_ABOUT), returning to the menu on key press.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_about_input(int *selectedOption) {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        gameState = STATE_MENU;
        *selectedOption = 2;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume); // Apply sfxVolume to menu navigation sound
            PlaySound(sfx_menu_nav);
        }
    }
}

// Handles input for the settings menu (STATE_SETTINGS), adjusting volume and mute settings.
// Parameters:
//   selectedOption - Pointer to the currently selected settings option (0: BG Music, 1: SFX, 2: Mute).
void handle_settings_input(int *selectedOption) {
    // Navigate settings menu
    if (IsKeyPressed(KEY_UP)) {
        *selectedOption = (*selectedOption - 1 + 3) % 3; // 3 settings
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
    }
    if (IsKeyPressed(KEY_DOWN)) {
        *selectedOption = (*selectedOption + 1) % 3;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
        switch (*selectedOption) {
            case 0: // BG Music Volume
                bgMusicVolume = fmax(0.0f, bgMusicVolume - 0.1f);
                SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                break;
            case 1: // SFX Volume (controls all sound effects)
                sfxVolume = fmax(0.0f, sfxVolume - 0.1f);
                // Update volume for all sound effects
                SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                break;
            case 2: // Mute Toggle
                soundMuted = !soundMuted;
                // Update volumes based on mute state
                SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                break;
        }
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
        switch (*selectedOption) {
            case 0: // BG Music Volume
                bgMusicVolume = fmin(1.0f, bgMusicVolume + 0.1f);
                SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                break;
            case 1: // SFX Volume (controls all sound effects)
                sfxVolume = fmin(1.0f, sfxVolume + 0.1f);
                // Update volume for all sound effects
                SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                break;
            case 2: // Mute Toggle
                soundMuted = !soundMuted;
                // Update volumes based on mute state
                SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                break;
        }
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        gameState = STATE_MENU;
        *selectedOption = 3; // Return to Settings option
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
    }
}

// Handles input for the pause menu (STATE_PAUSED), navigating options and settings.
// Parameters:
//   pausedThisFrame - Pointer to the flag tracking if pause was toggled this frame.
// Returns: true if the game should transition to another state (e.g., STATE_PLAYING or STATE_MENU), false otherwise.
bool handle_pause_input(bool *pausedThisFrame) {
    bool stateChanged = false;

    // Handle pause menu navigation
    if (!*pausedThisFrame) {
        if (pauseMenuState == PAUSE_MENU_MAIN) {
            // Navigate main pause menu
            if (IsKeyPressed(KEY_UP)) {
                pauseSelectedOption = (pauseSelectedOption - 1 + 4) % 4; // 4 options
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
            if (IsKeyPressed(KEY_DOWN)) {
                pauseSelectedOption = (pauseSelectedOption + 1) % 4;
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
            if (IsKeyPressed(KEY_ENTER)) {
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
                switch (pauseSelectedOption) {
                    case 0: // Resume
                        gameState = STATE_PLAYING;
                        playPacmanMove = true;
                        stateChanged = true;
                        break;
                    case 1: // Restart
                        reset_game_state(true, STATE_READY);
                        level = 1;
                        pacman.score = 0;
                        pacman.lives = 3;
                        init_maze();
                        gameState = STATE_READY;
                        pauseMenuState = PAUSE_MENU_MAIN;   // Reset pause menu state
                        pauseSelectedOption = 0;            // Reset selected option
                        stateChanged = true;
                        playPacmanMove = false;
                        break;
                    case 2: // Settings
                        pauseMenuState = PAUSE_MENU_SETTINGS;
                        pauseSelectedOption = 0; // Reset to first setting (BG Music)
                        break;
                    case 3: // Quit
                        stateChanged = true; // Transition handled by caller
                        break;
                }
            }
        } else if (pauseMenuState == PAUSE_MENU_SETTINGS) {
            // Navigate settings menu
            if (IsKeyPressed(KEY_UP)) {
                pauseSelectedOption = (pauseSelectedOption - 1 + 3) % 3; // 3 settings
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
            if (IsKeyPressed(KEY_DOWN)) {
                pauseSelectedOption = (pauseSelectedOption + 1) % 3;
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
            if (IsKeyPressed(KEY_LEFT)) {
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
                switch (pauseSelectedOption) {
                    case 0: // BG Music Volume
                        bgMusicVolume = fmax(0.0f, bgMusicVolume - 0.1f);
                        SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                        SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
                        break;
                    case 1: // SFX Volume (controls all sound effects)
                        sfxVolume = fmax(0.0f, sfxVolume - 0.1f);
                        // Update volume for all sound effects
                        SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                        break;
                    case 2: // Mute Toggle
                        soundMuted = !soundMuted;
                        // Update volumes based on mute state
                        SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                        SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                        SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                        break;
                }
            }
            if (IsKeyPressed(KEY_RIGHT)) {
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
                switch (pauseSelectedOption) {
                    case 0: // BG Music Volume
                        bgMusicVolume = fmin(1.0f, bgMusicVolume + 0.1f);
                        SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume * 0.3f); // Apply to background music
                        SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : sfxVolume * 0.3f);
                        break;
                    case 1: // SFX Volume (controls all sound effects)
                        sfxVolume = fmin(1.0f, sfxVolume + 0.1f);
                        // Update volume for all sound effects
                        SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                        break;
                    case 2: // Mute Toggle
                        soundMuted = !soundMuted;
                        // Update volumes based on mute state
                        SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                        SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                        SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
                        SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
                        break;
                }
            }
            if (IsKeyPressed(KEY_ENTER)) {
                pauseMenuState = PAUSE_MENU_MAIN;
                pauseSelectedOption = 2; // Return to Settings option
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
        }
    }

    // Toggle pause (only when not in settings to avoid accidental resume)
    if (IsKeyPressed(KEY_P) && pauseMenuState == PAUSE_MENU_MAIN) {
        gameState = STATE_PLAYING;
        *pausedThisFrame = true;
        playPacmanMove = true;
        stateChanged = true;
    }

    return stateChanged;
}

// Handles input for the game over screen (STATE_GAME_OVER), managing name input and returning to the menu.
// Parameters:
//   selectedOption - Pointer to the selected menu option to set when returning to the menu.
void handle_game_over_input(int *selectedOption) {
    // Name input
    if (!nameInputComplete) {
        int key = GetKeyPressed();
        while (key > 0) {
            // Accept A-Z keys
            if (key >= KEY_A && key <= KEY_Z && nameInputIndex < 3) {
                playerNameInput[nameInputIndex] = (char)key;
                nameInputIndex++;
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }

            // Backspace to erase last character
            if (key == KEY_BACKSPACE && nameInputIndex > 0) {
                nameInputIndex--;
                playerNameInput[nameInputIndex] = '_';
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }

            // Enter to confirm the name
            if (key == KEY_ENTER && nameInputIndex > 0) {
                nameInputComplete = true;
                playerNameInput[nameInputIndex] = '\0';
                // Update high scores now that name is entered
                check_and_update_high_scores(pacman.score);
                save_high_scores();
                if (!soundMuted) {
                    SetSoundVolume(sfx_menu_nav, sfxVolume);
                    PlaySound(sfx_menu_nav);
                }
            }
            key = GetKeyPressed();
        }
    }

    // Allow return to menu after name input with fade-out
    if (nameInputComplete && IsKeyPressed(KEY_ENTER)) {
        gameState = STATE_MENU;
        *selectedOption = 0;
        // Reset score, lives, and level for a new game
        pacman.score = 0;
        pacman.lives = 3;
        level = 1;
        pelletsEaten = 0;
        // Reset name input
        strncpy(playerNameInput, "AAA", sizeof(playerNameInput));
        playerNameInput[sizeof(playerNameInput) - 1] = '\0';
        nameInputIndex = 0;
        nameInputComplete = false;
        if (!soundMuted) {
            SetSoundVolume(sfx_menu_nav, sfxVolume);
            PlaySound(sfx_menu_nav);
        }
    }
}

void render_menu(int screenWidth, int screenHeight, Font font, int selectedOption, Texture2D pacmanSprite, Ghost* ghostArray) {
    ClearBackground(BLACK);

    // Update animation timers
    menuAnimTimer += GetFrameTime();
    pelletBlinkTimer += GetFrameTime();
    pacmanAnimTimer += GetFrameTime();
    ghostAnimTimer += GetFrameTime();

    // Blink pellets every 0.5s
    if (pelletBlinkTimer >= 0.5f) {
        pelletVisible = !pelletVisible;
        pelletBlinkTimer = 0.0f;
    }

    // Update Pac-Man animation (0.08s per frame, matches render_pacman)
    if (pacmanAnimTimer >= 0.08f) {
        pacmanFrame = (pacmanFrame + 1) % 3; // 3 frames: closed, half-open, fully open
        pacmanAnimTimer = 0.0f;
    }

    // Update ghost animation (0.2s per frame, matches render_ghosts)
    if (ghostAnimTimer >= 0.2f) {
        ghostFrame = (ghostFrame + 1) % 2;
        ghostAnimTimer = 0.0f;
    }

    // Move Pac-Man sprite
    pacmanSpriteX += 100.0f * GetFrameTime(); // Move right at 100px/s
    if (pacmanSpriteX > screenWidth + 32.0f) {
        pacmanSpriteX = -32.0f; // Reset to left
    }

    // Draw faded maze background
    for (int y = 0; y < screenHeight; y += 20) {
        for (int x = 0; x < screenWidth; x += 20) {
            if ((x / 20 + y / 20) % 2 == 0) {
                DrawRectangle(x, y, 10, 10, Fade(BLUE, 0.1f));
            }
            if (pelletVisible && (x / 20 + y / 20) % 4 == 0) {
                DrawCircle(x + 10, y + 10, 2, Fade(YELLOW, 0.5f));
            }
        }
    }

    // Draw Pac-Man sprite with eating animation
    float scaleFactor = 32.0f / 16.0f; // Scale to 32x32 pixels
    float pacmanXOffset = (pacmanFrame == 0) ? 0.0f : (pacmanFrame == 1) ? 18.0f : 36.0f;
    Rectangle pacmanSourceRec = { pacmanXOffset, 0.0f, 16.0f, 16.0f };
    Rectangle pacmanDestRec = { pacmanSpriteX, screenHeight / 4.0f - 16.0f, 32.0f, 32.0f };
    Vector2 pacmanOrigin = { 16.0f, 16.0f };
    DrawTexturePro(pacmanSprite, pacmanSourceRec, pacmanDestRec, pacmanOrigin, 0.0f, WHITE);

    // Draw ghost sprites trailing Pac-Man
    for (int i = 0; i < MAX_GHOSTS; i++) {
        float yOffset;
        switch (i) {
            case 0: yOffset = 64.0f; break; // Blinky (red)
            case 1: yOffset = 80.0f; break; // Pinky (pink)
            case 2: yOffset = 96.0f; break; // Inky (cyan)
            case 3: yOffset = 112.0f; break; // Clyde (orange)
            default: yOffset = 64.0f; break;
        }
        float xOffset = 4.0f + ghostFrame * 16.0f; // Right-facing sprite
        Rectangle ghostSourceRec = { xOffset, yOffset, 16.0f, 16.0f };
        Rectangle ghostDestRec = { pacmanSpriteX - 40.0f * (i + 1), screenHeight / 4.0f - 16.0f, 32.0f, 32.0f };
        DrawTexturePro(ghostArray[i].normalSprite[ghostFrame], ghostSourceRec, ghostDestRec, pacmanOrigin, 0.0f, WHITE);
    }

    // Draw menu items with pulsing effect
    const char *options[] = {"Start", "Highscores", "About", "Settings", "Exit"};
    float baseFontSize = 16.0f;
    float startY = screenHeight / 2.0f - 40.0f;
    for (int i = 0; i < 5; i++) {
        float scale = (i == selectedOption) ? 1.0f + 0.2f * sinf(menuAnimTimer * 4.0f) : 1.0f;
        float fontSize = baseFontSize * scale;
        Vector2 textSize = MeasureTextEx(font, options[i], fontSize, 1);
        Vector2 textPos = { screenWidth / 2.0f - textSize.x / 2.0f, startY + i * 30.0f };
        Color color = (i == selectedOption) ? YELLOW : WHITE;

        // Draw glow effect for selected option
        if (i == selectedOption) {
            DrawTextEx(font, options[i], textPos, fontSize, 1, Fade(YELLOW, 0.3f)); // Glow
        }
        DrawTextEx(font, options[i], textPos, fontSize, 1, color);
    }

    // Draw title
    Vector2 titleSize = MeasureTextEx(font, "Pac-Man", 20.0f, 1);
    DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2.0f - titleSize.x / 2.0f, screenHeight / 2.0f - 100.0f}, 20.0f, 1, YELLOW);

    // Draw navigation instructions
    Vector2 navSize = MeasureTextEx(font, "Use UP/DOWN to select, ENTER to confirm", 10.0f, 1);
    DrawTextEx(font, "Use UP/DOWN to select, ENTER to confirm", 
               (Vector2){screenWidth / 2.0f - navSize.x / 2.0f, screenHeight / 2.0f + 110.0f}, 10.0f, 1, GRAY);

    // Draw CRT scanlines
    for (int y = 0; y < screenHeight; y += 4) {
        DrawRectangle(0, y, screenWidth, 2, Fade(BLACK, 0.2f));
    }
}

// Renders the high scores screen (STATE_HIGHSCORES) with the top scores.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_highscores(int screenWidth, int screenHeight, Font font) {
    ClearBackground(BLACK);
    DrawTextEx(font, "High Scores", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 100}, 20.0f, 1, YELLOW);
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        char scoreText[32];
        sprintf(scoreText, "%d. %s - %d", i + 1, highscores[i].name, highscores[i].score);
        DrawTextEx(font, scoreText, (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 60 + i * 20}, 16.0f, 1, WHITE);
    }
    DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 100}, 10.0f, 1, GRAY);
}

// Renders the about screen (STATE_ABOUT) with game information.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_about(int screenWidth, int screenHeight, Font font) {
    ClearBackground(BLACK);
    DrawTextEx(font, "About Pac-Man Remake", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 100}, 20.0f, 1, YELLOW);
    DrawTextEx(font, "Developed by Kyle Ibo", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 40}, 16.0f, 1, WHITE);
    DrawTextEx(font, "Powered by Raylib", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 10}, 16.0f, 1, WHITE);
    DrawTextEx(font, "Version 1.0", (Vector2){screenWidth / 2 - 30, screenHeight / 2 + 20}, 16.0f, 1, WHITE);
    DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 80}, 10.0f, 1, GRAY);
}

// Renders the settings menu (STATE_SETTINGS) with volume sliders and mute toggle.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
//   selectedOption - Currently selected settings option.
void render_settings_menu(int screenWidth, int screenHeight, Font font, int selectedOption) {
    const char *options[] = {"BG MUSIC VOLUME", "SFX VOLUME", "MUTE"}; // Updated to "SFX VOLUME" for all sound effects
    int numOptions = 3;
    int fontSize = 16;
    int spacing = 30;
    int startY = screenHeight / 2 - (numOptions * spacing) / 2;
    int sliderWidth = fontSize * 6;
    int sliderHeight = (int)(fontSize * 0.3 + 0.5);

    // Draw "SETTINGS" title
    const char *title = "SETTINGS";
    Vector2 titleSize = MeasureTextEx(font, title, fontSize * 1.25, 2);
    DrawTextEx(font, title, (Vector2){screenWidth / 2 - titleSize.x / 2, startY - 60}, fontSize * 1.5, 2, YELLOW);

    // Draw settings options
    for (int i = 0; i < numOptions; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        Vector2 textSize = MeasureTextEx(font, options[i], fontSize, 2);
        float textX = screenWidth / 2 - textSize.x - 30;
        float sliderX = screenWidth / 2 + 10;
        float y = startY + i * spacing;

        // Draw option text
        DrawTextEx(font, options[i], (Vector2){textX, y}, fontSize, 2, color);

        // Draw sliders or mute status
        if (i < 2) {
            // Sliders for volumes
            float volume = (i == 0) ? bgMusicVolume : sfxVolume;
            DrawRectangle(sliderX, y + 2, sliderWidth, sliderHeight, GRAY);
            DrawRectangle(sliderX, y + 2, sliderWidth * volume, sliderHeight, color);
            char valueText[16];
            sprintf(valueText, "%.0f%%", volume * 100);
            Vector2 valueSize = MeasureTextEx(font, valueText, fontSize * 0.8, 2);
            DrawTextEx(font, valueText, (Vector2){sliderX + sliderWidth + 10, y}, fontSize * 0.8, 2, WHITE);
        } else {
            // Mute toggle
            const char *muteText = soundMuted ? "ON" : "OFF";
            Vector2 muteSize = MeasureTextEx(font, muteText, fontSize, 2);
            DrawTextEx(font, muteText, (Vector2){sliderX, y}, fontSize, 2, color);
        }
    }
}

// Renders the pause menu (STATE_PAUSED) with options to resume, restart, adjust settings, or quit.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
void render_pause_menu(int screenWidth, int screenHeight, Font font) {
    // Draw a semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    if (pauseMenuState == PAUSE_MENU_MAIN) {
        const char *options[] = {"RESUME", "RESTART", "SETTINGS", "QUIT"};
        int numOptions = 4;
        int fontSize = 16;
        int spacing = 30;
        int startY = screenHeight / 2 - (numOptions * spacing) / 2;

        // Draw "PAUSED" title
        const char *title = "PAUSED";
        Vector2 titleSize = MeasureTextEx(font, title, fontSize * 1.25, 2);
        DrawTextEx(font, title, (Vector2){screenWidth / 2 - titleSize.x / 2, startY - 60}, fontSize * 1.25, 2, YELLOW);

        // Draw menu options
        for (int i = 0; i < numOptions; i++) {
            Color color = (i == pauseSelectedOption) ? YELLOW : WHITE;
            Vector2 textSize = MeasureTextEx(font, options[i], fontSize, 2);
            DrawTextEx(font, options[i], (Vector2){screenWidth / 2 - textSize.x / 2, startY + i * spacing}, fontSize, 2, color);
        }
    } else if (pauseMenuState == PAUSE_MENU_SETTINGS) {
        const char *options[] = {"BG MUSIC VOLUME", "SFX VOLUME", "MUTE"}; // Updated to "SFX VOLUME" for all sound effects
        int numOptions = 3;
        int fontSize = 16;
        int spacing = 30;
        int startY = screenHeight / 2 - (numOptions * spacing) / 2;
        int sliderWidth = fontSize * 6;
        int sliderHeight = (int)(fontSize * 0.3 + 0.5);

        // Draw "SETTINGS" title
        const char *title = "PAUSE MENU - SETTINGS";
        Vector2 titleSize = MeasureTextEx(font, title, fontSize * 1.25, 2);
        DrawTextEx(font, title, (Vector2){screenWidth / 2 - titleSize.x / 2, startY - 60}, fontSize * 1.5, 2, YELLOW);

        // Draw settings options
        for (int i = 0; i < numOptions; i++) {
            Color color = (i == pauseSelectedOption) ? YELLOW : WHITE;
            Vector2 textSize = MeasureTextEx(font, options[i], fontSize, 2);
            float textX = screenWidth / 2 - textSize.x - 30;
            float sliderX = screenWidth / 2 + 10;
            float y = startY + i * spacing;

            // Draw option text
            DrawTextEx(font, options[i], (Vector2){textX, y}, fontSize, 2, color);

            // Draw sliders or mute status
            if (i < 2) {
                // Sliders for volumes
                float volume = (i == 0) ? bgMusicVolume : sfxVolume;
                DrawRectangle(sliderX, y + 2, sliderWidth, sliderHeight, GRAY);
                DrawRectangle(sliderX, y + 2, sliderWidth * volume, sliderHeight, color);
                char valueText[16];
                sprintf(valueText, "%.0f%%", volume * 100);
                Vector2 valueSize = MeasureTextEx(font, valueText, fontSize * 0.8, 2);
                DrawTextEx(font, valueText, (Vector2){sliderX + sliderWidth + 10, y}, fontSize * 0.8, 2, WHITE);
            } else {
                // Mute toggle
                const char *muteText = soundMuted ? "ON" : "OFF";
                Vector2 muteSize = MeasureTextEx(font, muteText, fontSize, 2);
                DrawTextEx(font, muteText, (Vector2){sliderX, y}, fontSize, 2, color);
            }
        }

        // Draw navigation instructions
        DrawTextEx(font, "Press ENTER to return to Pause Menu", 
                   (Vector2){screenWidth / 2 - 120, startY + numOptions * spacing + 20}, 10.0f, 1, GRAY);
    }
}

void render_level_complete(int screenWidth, int screenHeight, Font font, float animTimer, int currentLevel,
                           int eatenPellets, int eatenPowerPellets, int eatenGhosts,
                           int collectedFruits, int fruitPoints) {
    // Static "Level Complete!" text
    const char* levelCompleteText = "Level Complete!";
    Vector2 textSize = MeasureTextEx(font, levelCompleteText, 20.0f, 1);
    Vector2 textPos = { screenWidth / 2.0f - textSize.x / 2.0f, screenHeight / 2.0f - 80 };
    DrawTextEx(font, levelCompleteText, textPos, 20.0f, 1, YELLOW);

    // Fading "Preparing Level X..." subtitle
    float subtitleAlpha = (animTimer < 4.0f) ? (1.0f - animTimer / 4.0f) : 0.0f;
    char subtitle[32];
    snprintf(subtitle, sizeof(subtitle), "Preparing Level %d...", currentLevel);
    textSize = MeasureTextEx(font, subtitle, 16.0f, 1);
    textPos = (Vector2){ screenWidth / 2.0f - textSize.x / 2.0f, screenHeight / 2.0f - 40 };
    DrawTextEx(font, subtitle, textPos, 16.0f, 1, Fade(WHITE, subtitleAlpha));

    // Score breakdown with sequential fade-in
    float elapsedTime = 6.0f - animTimer; // Time since entering STATE_LEVEL_COMPLETE
    const float fadeDuration = 0.5f; // Duration of each fade-in
    float pelletsAlpha = 0.0f;
    float powerPelletsAlpha = 0.0f;
    float ghostsAlpha = 0.0f;
    float fruitsAlpha = 0.0f;
    float progressBarAlpha = 0.0f;
    float backgroundAlpha = 0.0f;

    // Calculate alpha for each element based on elapsed time
    if (elapsedTime >= 0.5f) {
        pelletsAlpha = (elapsedTime - 0.5f) / fadeDuration;
        backgroundAlpha = (elapsedTime - 0.5f) / fadeDuration;
        if (pelletsAlpha > 1.0f) pelletsAlpha = 1.0f;
        if (backgroundAlpha > 1.0f) backgroundAlpha = 1.0f;
    }
    if (elapsedTime >= 1.0f) {
        powerPelletsAlpha = (elapsedTime - 1.0f) / fadeDuration;
        if (powerPelletsAlpha > 1.0f) powerPelletsAlpha = 1.0f;
    }
    if (elapsedTime >= 1.5f) {
        ghostsAlpha = (elapsedTime - 1.5f) / fadeDuration;
        if (ghostsAlpha > 1.0f) ghostsAlpha = 1.0f;
    }
    if (elapsedTime >= 2.0f) {
        fruitsAlpha = (elapsedTime - 2.0f) / fadeDuration;
        if (fruitsAlpha > 1.0f) fruitsAlpha = 1.0f;
    }
    if (elapsedTime >= 2.5f) {
        progressBarAlpha = (elapsedTime - 2.5f) / fadeDuration;
        if (progressBarAlpha > 1.0f) progressBarAlpha = 1.0f;
    }

    // Score breakdown calculations
    int pelletsScore = eatenPellets * 10;
    int powerPelletsScore = eatenPowerPellets * 50;
    int ghostPoints = 0;
    for (int i = 1; i <= eatenGhosts; i++) {
        ghostPoints += 200 * (1 << (i - 1)); // 200, 400, 800, 1600
    }
    int fruitScore = collectedFruits * fruitPoints;

    // Measure text widths for score breakdown
    char pelletsText[32];
    char powerPelletsText[32];
    char ghostsText[32];
    char fruitsText[32];
    snprintf(pelletsText, sizeof(pelletsText), "Pellets: %d x 10 = %d", eatenPellets, pelletsScore);
    snprintf(powerPelletsText, sizeof(powerPelletsText), "Power Pellets: %d x 50 = %d", eatenPowerPellets, powerPelletsScore);
    snprintf(ghostsText, sizeof(ghostsText), "Ghosts Eaten: %d = %d", eatenGhosts, ghostPoints);
    snprintf(fruitsText, sizeof(fruitsText), "Fruits: %d x %d = %d", collectedFruits, fruitPoints, fruitScore);

    Vector2 pelletsSize = MeasureTextEx(font, pelletsText, 10.0f, 1);
    Vector2 powerPelletsSize = MeasureTextEx(font, powerPelletsText, 10.0f, 1);
    Vector2 ghostsSize = MeasureTextEx(font, ghostsText, 10.0f, 1);
    Vector2 fruitsSize = MeasureTextEx(font, fruitsText, 10.0f, 1);

    // Calculate maximum text width
    float maxTextWidth = pelletsSize.x;
    if (powerPelletsSize.x > maxTextWidth) maxTextWidth = powerPelletsSize.x;
    if (ghostsSize.x > maxTextWidth) maxTextWidth = ghostsSize.x;
    if (fruitsSize.x > maxTextWidth) maxTextWidth = fruitsSize.x;

    // Draw semi-transparent background for score breakdown and progress bar
    Rectangle backgroundRect = {
        screenWidth / 2.0f - (maxTextWidth / 2.0f + 20), // x: centered, max text width + 20 padding
        screenHeight / 2.0f - 10,                        // y: start 10 pixels above pellets text
        maxTextWidth + 40,                               // width: max text width + 20 padding on each side
        130                                              // height: 60 (text height) + 10 (gap) + 10 (bar height) + 30 padding
    };
    DrawRectangleRounded(backgroundRect, 0.2f, 20, Fade(DARKGRAY, backgroundAlpha * 0.7f));

    // Score breakdown with centered text
    float leftPadding = backgroundRect.x + 10; // 10 pixels padding from left edge
    DrawTextEx(font, pelletsText,
               (Vector2){ leftPadding, screenHeight / 2.0f + 0 }, 10.0f, 1, Fade(WHITE, pelletsAlpha));
    DrawTextEx(font, powerPelletsText,
               (Vector2){ leftPadding, screenHeight / 2.0f + 20 }, 10.0f, 1, Fade(WHITE, powerPelletsAlpha));
    DrawTextEx(font, ghostsText,
               (Vector2){ leftPadding, screenHeight / 2.0f + 40 }, 10.0f, 1, Fade(WHITE, ghostsAlpha));
    DrawTextEx(font, fruitsText,
               (Vector2){ leftPadding, screenHeight / 2.0f + 60 }, 10.0f, 1, Fade(WHITE, fruitsAlpha));

    // Progress bar for remaining time
    float progress = animTimer / 3.5f;  // Adjusted to fill over 3.5 seconds
    float barWidth = maxTextWidth;      // Match bar width to text width
    float barHeight = 10.0f;
    Rectangle barOutline = { screenWidth / 2.0f - barWidth / 2.0f, screenHeight / 2.0f + 100, barWidth, barHeight };
    Rectangle barFill = { barOutline.x, barOutline.y, barWidth * progress, barHeight };
    DrawRectangleRec(barOutline, Fade(WHITE, progressBarAlpha * 0.5f)); // Outline with half opacity
    DrawRectangleRec(barFill, Fade(YELLOW, progressBarAlpha));
}

// Renders the game over screen (STATE_GAME_OVER), including name input and high scores.
// Parameters:
//   screenWidth - Width of the screen in pixels.
//   screenHeight - Height of the screen in pixels.
//   font - Font used for rendering text.
//   gameOverFadeAlpha - Alpha value for fade-in/out effects.
void render_game_over(int screenWidth, int screenHeight, Font font, float gameOverFadeAlpha) {
    ClearBackground(BLACK);

    // Game Over text
    DrawTextEx(font, "Game Over",
            (Vector2){screenWidth / 2 - MeasureTextEx(font, "Game Over", 24.0f, 1).x / 2, screenHeight / 2 - 100},
            24.0f, 1, Fade(YELLOW, gameOverFadeAlpha));

    // Score and motivational message
    DrawTextEx(font, TextFormat("Final Score: %d", pacman.score),
            (Vector2){screenWidth / 2 - MeasureTextEx(font, TextFormat("Final Score: %d", pacman.score), 16.0f, 1).x / 2, screenHeight / 2 - 40},
            16.0f, 1, Fade(WHITE, gameOverFadeAlpha));
    DrawTextEx(font, gameOverMessages[selectedMessageIndex],
            (Vector2){screenWidth / 2 - MeasureTextEx(font, gameOverMessages[selectedMessageIndex], 12.0f, 1).x / 2, screenHeight / 2 - 10},
            12.0f, 1, Fade(WHITE, gameOverFadeAlpha));

    // Add delay and fade-in for name input
    if (!nameInputComplete) {
        DrawTextEx(font, "Enter Initials:",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "Enter Initials:", 16.0f, 1).x / 2, screenHeight / 2 + 20},
                16.0f, 1, Fade(YELLOW, gameOverFadeAlpha));
        char displayName[4] = {'_', '_', '_', '\0'};
        for (int i = 0; i < nameInputIndex; i++) {
            displayName[i] = playerNameInput[i];
        }
        // Blinking cursor at the active input slot
        if (((int)(GetTime() * 2.0f) % 2) == 0 && nameInputIndex < 3) {
            displayName[nameInputIndex] = '|';
        }
        DrawTextEx(font, displayName,
                (Vector2){screenWidth / 2 - MeasureTextEx(font, displayName, 16.0f, 1).x / 2, screenHeight / 2 + 50},
                16.0f, 1, Fade(WHITE, gameOverFadeAlpha));
        DrawTextEx(font, "Press ENTER to confirm",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "Press ENTER to confirm", 10.0f, 1).x / 2, screenHeight / 2 + 80},
                10.0f, 1, Fade(GRAY, gameOverFadeAlpha));

        // High score preview with placeholder
        DrawRectangleLines(screenWidth / 2 - 90, screenHeight / 2 + 100, 180, 130, Fade(YELLOW, gameOverFadeAlpha));
        DrawTextEx(font, "High Scores:",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "High Scores:", 16.0f, 1).x / 2, screenHeight / 2 + 110},
                16.0f, 1, Fade(YELLOW, gameOverFadeAlpha));
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            char* scoreDisplayName = (i == 0 && pacman.score > highscores[i].score) ? "YOU" : highscores[i].name;
            int displayScore = (i == 0 && pacman.score > highscores[i].score) ? pacman.score : highscores[i].score;
            DrawTextEx(font, TextFormat("%s: %d", scoreDisplayName, displayScore),
                    (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 130 + i * 20},
                    12.0f, 1, Fade((i == 0 && pacman.score > highscores[i].score) ? GREEN : WHITE, gameOverFadeAlpha));
        }
    } else {
        DrawTextEx(font, "Initials:",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "Initials:", 16.0f, 1).x / 2, screenHeight / 2 + 20},
                16.0f, 1, Fade(YELLOW, gameOverFadeAlpha));
        DrawTextEx(font, playerNameInput,
                (Vector2){screenWidth / 2 - MeasureTextEx(font, playerNameInput, 16.0f, 1).x / 2, screenHeight / 2 + 50},
                16.0f, 1, Fade(WHITE, gameOverFadeAlpha));
        // High score list with retro border
        DrawRectangleLines(screenWidth / 2 - 80, screenHeight / 2 + 100, 160, 120, Fade(YELLOW, gameOverFadeAlpha));
        DrawTextEx(font, "High Scores:",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "High Scores:", 16.0f, 1).x / 2, screenHeight / 2 + 110},
                16.0f, 1, Fade(YELLOW, gameOverFadeAlpha));
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            bool isPlayerScore = (strcmp(highscores[i].name, playerNameInput) == 0 && highscores[i].score == pacman.score);
            DrawTextEx(font, TextFormat("%s: %d", highscores[i].name, highscores[i].score),
                    (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 130 + i * 20},
                    12.0f, 1, Fade(isPlayerScore ? GREEN : WHITE, gameOverFadeAlpha));
        }
        DrawTextEx(font, "Press ENTER to Return to Menu",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "Press ENTER to Return to Menu", 10.0f, 1).x / 2, screenHeight / 2 + 230},
                10.0f, 1, Fade(GRAY, gameOverFadeAlpha));
    }

    // Error message for failed high score saving
    if (saveHighScoreFailed) {
        DrawTextEx(font, "Failed to save high score!",
                (Vector2){screenWidth / 2 - MeasureTextEx(font, "Failed to save high score!", 12.0f, 1).x / 2, screenHeight / 2 + 260},
                12.0f, 1, Fade(RED, gameOverFadeAlpha));
    }
}