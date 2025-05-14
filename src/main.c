#include "game_logic.h"
#include "logo_animation.h"
#include "rendering.h"

int main(void) {
    // Seed random number generator
    SetRandomSeed((unsigned int)time(NULL));

    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Pacman v1.0");

    SetTargetFPS(60);

    // Load a font
    Font font = LoadFont("assets/fonts/Emulogic-zrEw.ttf");

    // Load sound effects
    InitAudioDevice();
    sfx_menu = LoadSound("assets/sounds/pacman_menu.mp3");
    sfx_menu_nav = LoadSound("assets/sounds/pacman_menu_nav.wav");
    sfx_ready = LoadSound("assets/sounds/pacman_beginning.wav");
    sfx_pacman_move = LoadSound("assets/sounds/pacman_move.mp3");;
    sfx_pacman_chomp = LoadSound("assets/sounds/pacman_chomp.wav");
    sfx_pacman_death = LoadSound("assets/sounds/pacman_death.wav");
    sfx_eat_fruit= LoadSound("assets/sounds/pacman_eatfruit.wav");
    sfx_eat_ghost= LoadSound("assets/sounds/pacman_eatghost.wav");
    sfx_ghost_frightened = LoadSound("assets/sounds/pacman_ghost_frightened.wav");
    sfx_level_complete = LoadSound("assets/sounds/pacman_level_complete.mp3");
    sfx_extra_life = LoadSound("assets/sounds/pacman_extralife.wav");
    sfx_game_over = LoadSound("assets/sounds/pacman_game_over.mp3");

    // Set initial volumes
    SetSoundVolume(sfx_menu, bgMusicVolume);
    SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
    SetSoundVolume(sfx_ready, pacmanSfxVolume);
    SetSoundVolume(sfx_pacman_move, pacmanSfxVolume);
    SetSoundVolume(sfx_pacman_chomp, pacmanSfxVolume);
    SetSoundVolume(sfx_pacman_death, pacmanSfxVolume);
    SetSoundVolume(sfx_eat_fruit, pacmanSfxVolume);
    SetSoundVolume(sfx_eat_ghost, pacmanSfxVolume);
    SetSoundVolume(sfx_ghost_frightened, pacmanSfxVolume);
    SetSoundVolume(sfx_level_complete, pacmanSfxVolume);
    SetSoundVolume(sfx_extra_life, pacmanSfxVolume);
    SetSoundVolume(sfx_game_over, pacmanSfxVolume);
    
    // Calculate maze dimensions and offsets
    const int mazePixelWidth = MAZE_WIDTH * TILE_SIZE;              // 560px
    const int mazePixelHeight = MAZE_HEIGHT * TILE_SIZE;            // 620px
    const int mazeOffsetX = (screenWidth - mazePixelWidth) / 2;     // 360px (180px on each side)
    const int mazeOffsetY = (screenHeight - mazePixelHeight) / 2;   // 50px (top and bottom)

    // Menu variables
    int selectedOption = 0;
    bool shouldExit = false;

    static bool isMenuLoopPlaying = false;
    bool pausedThisFrame = false;       // Tracks if pause was toggled this frame
    
    // Logo animation state
    LogoAnimation logoAnim;
    init_kool_dude_logo(&logoAnim, screenWidth, screenHeight);      // First state

    // Load ghost textures
    LoadGhostTextures(ghosts);

    // Initialize fruit
    init_fruit();

    // Load high scores
    load_high_scores();

    // Fade to black transition variables
    float transitionAlpha = 0.0f;
    bool fadingOut = false;
    GameState nextState = STATE_KOOLDUDE_LOGO;
    GameState prevState = STATE_MENU;

    // Disable ESC key from closing the window
    SetExitKey(KEY_ONE);

    // Game Loop
    // ----------------------------------------------------------------------------------------
    while (!(shouldExit || WindowShouldClose())) {

        // Toggle fullscreen with F11
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        DrawFPS(10,10);

        // Fade to black transition
        if (fadingOut) {
            transitionAlpha += 0.05f;
            if (transitionAlpha >= 1.0f) {
                transitionAlpha = 1.0f;
                fadingOut = false;
                gameState = nextState;
                transitionAlpha = 1.0f;

                // Perform actions after fade-out
                if (gameState == STATE_READY && nextState == STATE_READY) {
                    init_maze();
                    reset_game_state(true, STATE_READY);
                    if (prevState == STATE_LEVEL_COMPLETE || prevState == STATE_MENU) {
                        pelletsEaten = 0;
                    }
                }

                // Handle quit from pause menu
                if (gameState == STATE_MENU && prevState == STATE_PAUSED) {
                    StopSound(sfx_pacman_move);
                    StopSound(sfx_pacman_chomp);
                    StopSound(sfx_ghost_frightened);
                    reset_game_state(true, STATE_MENU);
                    level = 1;
                    pacman.score = 0;
                    pacman.lives = 3;
                    init_maze();
                    pauseMenuState = PAUSE_MENU_MAIN;
                    pauseSelectedOption = 0;
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu, bgMusicVolume);
                        PlaySound(sfx_menu);
                    }
                }

                // Stop menu loop sound when exiting STATE_MENU
                if (prevState == STATE_MENU && isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }
            }
        } else if (transitionAlpha > 0.0f) {
            transitionAlpha -= 0.05f;
            if (transitionAlpha < 0.0f) {
                transitionAlpha = 0.0f;
            }
        }

        switch (gameState) {
            case STATE_KOOLDUDE_LOGO:
                if (update_kool_dude_logo(&logoAnim)) {
                    fadingOut = true;
                    nextState = STATE_DEV_LOGO;
                    init_dev_logo(&logoAnim);
                }
                break;
            
            case STATE_DEV_LOGO:
                if (update_dev_logo(&logoAnim)) {
                    fadingOut = true;
                    nextState = STATE_RAYLIB_LOGO;
                    init_raylib_logo(&logoAnim, screenWidth, screenHeight);
                }
                break;

            case STATE_RAYLIB_LOGO:
                if (update_raylib_logo(&logoAnim)) {
                    fadingOut = true;
                    nextState = STATE_LOGO;
                    init_game_logo(&logoAnim);
                }
                break;

            case STATE_LOGO:
                if (update_game_logo(&logoAnim)) {
                    fadingOut = true;
                    nextState = STATE_MENU;
                }
                break;

            case STATE_MENU:
                // Start menu loop sound
                if (!IsSoundPlaying(sfx_menu)) {
                    SetSoundVolume(sfx_menu, bgMusicVolume);
                    PlaySound(sfx_menu);
                    isMenuLoopPlaying = true;
                }

                // Navigate menu options
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedOption = (selectedOption + 1) % 5;
                    PlaySound(sfx_menu_nav);
                }
                if (IsKeyPressed(KEY_UP)) {
                    selectedOption = (selectedOption - 1 + 5) % 5;
                    PlaySound(sfx_menu_nav);
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedOption == 0) {      // START
                        init_maze();
                        level = 1;                  // Reset level to 1 when starting a new game
                        pacman.score = 0;           // Reset score for new game
                        pacman.lives = 3;           // Reset lives for new game
                        totalFruitsCollected = 0;   // Reset fruit count
                        reset_game_state(true, STATE_READY);     // Initialize game state and play sfx_ready                      
                    } else if (selectedOption == 1) { // HIGHSCORES
                        gameState = STATE_HIGHSCORES;
                        selectedOption = 0;
                    } else if (selectedOption == 2) { // ABOUT
                        gameState = STATE_ABOUT;
                        selectedOption = 0;    
                    } else if (selectedOption == 3) { // SETTINGS
                        gameState = STATE_SETTINGS;
                        selectedOption = 0;
                    } else {                        // EXIT
                        shouldExit = true;
                        fadingOut = true;
                        nextState = STATE_MENU;
                    }
                    PlaySound(sfx_menu_nav);
                }
                break;

            case STATE_HIGHSCORES:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    gameState = STATE_MENU;
                    selectedOption = 1;
                    PlaySound(sfx_menu_nav);
                }
                break;

            case STATE_ABOUT:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    gameState = STATE_MENU;
                    selectedOption = 2;
                    PlaySound(sfx_menu_nav);
                }
                break;

            case STATE_SETTINGS:
                // Navigate settings menu
                if (IsKeyPressed(KEY_UP)) {
                    selectedOption = (selectedOption - 1 + 3) % 3; // 3 settings
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                        PlaySound(sfx_menu_nav);
                    }
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedOption = (selectedOption + 1) % 3;
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                        PlaySound(sfx_menu_nav);
                    }
                }
                if (IsKeyPressed(KEY_LEFT)) {
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                        PlaySound(sfx_menu_nav);
                    }
                    switch (selectedOption) {
                        case 0: // BG Music Volume
                            bgMusicVolume = fmax(0.0f, bgMusicVolume - 0.1f);
                            SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                            break;
                        case 1: // Pac-Man SFX Volume
                            pacmanSfxVolume = fmax(0.0f, pacmanSfxVolume - 0.1f);
                            break;
                        case 2: // Mute Toggle
                            soundMuted = !soundMuted;
                            SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                            break;
                    }
                }
                if (IsKeyPressed(KEY_RIGHT)) {
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                        PlaySound(sfx_menu_nav);
                    }
                    switch (selectedOption) {
                        case 0: // BG Music Volume
                            bgMusicVolume = fmin(1.0f, bgMusicVolume + 0.1f);
                            SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                            break;
                        case 1: // Pac-Man SFX Volume
                            pacmanSfxVolume = fmin(1.0f, pacmanSfxVolume + 0.1f);
                            break;
                        case 2: // Mute Toggle
                            soundMuted = !soundMuted;
                            SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                            break;
                    }
                }
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    gameState = STATE_MENU;
                    selectedOption = 3; // Return to Settings option
                    PlaySound(sfx_menu_nav);
                }
                break;
            
            case STATE_READY:
                // Ensure menu loop stops when entering READY
                if (isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }

                if (!IsSoundPlaying(sfx_ready)) {
                    PlaySound(sfx_ready);
                }

                readyTimer -= GetFrameTime();
                if (readyTimer <= 0.0f) {
                    gameState = STATE_PLAYING;
                    readyTimer = 0.0f;
                    StopSound(sfx_ready);
                }
                break;
            
            case STATE_PLAYING:
                update_pacman();
                update_ghosts();
                update_fruit();

                if (!soundMuted && !IsSoundPlaying(sfx_pacman_move)) {
                    SetSoundVolume(sfx_pacman_move, pacmanSfxVolume);
                    PlaySound(sfx_pacman_move);
                }
                if (powerPelletTimer > 0.0f) {
                    powerPelletTimer -= GetFrameTime();
                    if (powerPelletTimer < 0.0f) {
                        powerPelletTimer = 0.0f;
                    }
                }
                //printf("pelletsEaten = %d, fruit.active = %d\n", pelletsEaten, fruit.active); // Debug print
        
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                    gameState = STATE_PAUSED;
                }
                break;

            case STATE_PAUSED:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                    gameState = STATE_PLAYING;
                }

                // Handle pause menu navigation
                if (!pausedThisFrame) {
                    if (pauseMenuState == PAUSE_MENU_MAIN) {
                        // Navigate main pause menu
                        if (IsKeyPressed(KEY_UP)) {
                            pauseSelectedOption = (pauseSelectedOption - 1 + 4) % 4; // 4 options
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                        if (IsKeyPressed(KEY_DOWN)) {
                            pauseSelectedOption = (pauseSelectedOption + 1) % 4;
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                        if (IsKeyPressed(KEY_ENTER)) {
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                            switch (pauseSelectedOption) {

                                case 0: // Resume
                                    gameState = STATE_PLAYING;
                                    if (!soundMuted && !IsSoundPlaying(sfx_pacman_move)) {
                                        SetSoundVolume(sfx_pacman_move, pacmanSfxVolume);
                                        PlaySound(sfx_pacman_move);
                                    }
                                    break;

                                case 1: // Restart
                                    reset_game_state(true, STATE_READY);
                                    level = 1;
                                    pacman.score = 0;
                                    pacman.lives = 3;
                                    init_maze();
                                    gameState = STATE_READY;
                                    break;

                                case 2: // Settings
                                    pauseMenuState = PAUSE_MENU_SETTINGS;
                                    pauseSelectedOption = 0; // Reset to first setting (BG Music)
                                    break;

                                case 3: // Quit
                                    fadingOut = true;
                                    nextState = STATE_MENU;
                                    prevState = STATE_PAUSED;
                                    break;
                            }
                        }
                    } else if (pauseMenuState == PAUSE_MENU_SETTINGS) {
                        // Navigate settings menu
                        if (IsKeyPressed(KEY_UP)) {
                            pauseSelectedOption = (pauseSelectedOption - 1 + 3) % 3; // 3 settings
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                        if (IsKeyPressed(KEY_DOWN)) {
                            pauseSelectedOption = (pauseSelectedOption + 1) % 3;
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                        if (IsKeyPressed(KEY_LEFT)) {
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                            switch (pauseSelectedOption) {

                                case 0: // BG Music Volume
                                    bgMusicVolume = fmax(0.0f, bgMusicVolume - 0.1f);
                                    SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                                    break;

                                case 1: // Pac-Man SFX Volume
                                    pacmanSfxVolume = fmax(0.0f, pacmanSfxVolume - 0.1f);
                                    break;

                                case 2: // Mute Toggle
                                    soundMuted = !soundMuted;
                                    SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                                    break;
                            }
                        }
                        if (IsKeyPressed(KEY_RIGHT)) {
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                            switch (pauseSelectedOption) {

                                case 0: // BG Music Volume
                                    bgMusicVolume = fmin(1.0f, bgMusicVolume + 0.1f);
                                    SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume);
                                    break;

                                case 1: // Pac-Man SFX Volume
                                    pacmanSfxVolume = fmin(1.0f, pacmanSfxVolume + 0.1f);
                                    break;
                                    
                                case 2: // Mute Toggle
                                    soundMuted = !soundMuted;
                                    SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume);
                                    break;
                            }
                        }
                        if (IsKeyPressed(KEY_ESCAPE)) {
                            pauseMenuState = PAUSE_MENU_MAIN;
                            pauseSelectedOption = 2; // Return to Settings option
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                    }
                }
                pausedThisFrame = false;

                // Toggle pause (only when not in settings to avoid accidental resume)
                if (IsKeyPressed(KEY_P) && pauseMenuState == PAUSE_MENU_MAIN) {
                    gameState = STATE_PLAYING;
                    pausedThisFrame = true;
                    if (!soundMuted && !IsSoundPlaying(sfx_pacman_move)) {
                        SetSoundVolume(sfx_pacman_move, pacmanSfxVolume);
                        PlaySound(sfx_pacman_move);
                    }
                }
                break;

            case STATE_GHOST_EATEN:
                ghostEatenTimer -= GetFrameTime();
                if (ghostEatenTimer <= 0.0f) {
                    eatenGhostIndex = -1;   // Reset for next ghost
                    gameState = STATE_PLAYING;
                    // Resume frightened sound if any ghost is still frightened
                    if (isFrightenedSoundPaused) {
                        bool anyFrightened = false;
                        for (int i = 0; i < MAX_GHOSTS; i++) {
                            if (ghosts[i].state == GHOST_FRIGHTENED && ghosts[i].stateTimer > 0.0f) {
                                anyFrightened = true;
                                break;
                            }
                        }
                        if (anyFrightened) {
                            ResumeSound(sfx_ghost_frightened);
                            isFrightenedSoundPaused = false;
                        } else {
                            StopSound(sfx_ghost_frightened);
                            isFrightenedSoundPaused = false;
                        }
                    }
                }
                break;

            case STATE_DEATH_ANIM:
                deathAnimTimer -= GetFrameTime();
                if (deathAnimTimer <= 0.0f) {
                    reset_game_state(false, STATE_READY);
                    deathAnimTimer = 0.0f;
                } else {
                    // Update death animation frame based on time
                    // Total animation duration is 6 secs
                    float frameDuration = 6.0f / PACMAN_DEATH_FRAMES;
                    int newFrame = (int)((6.0f - deathAnimTimer) / frameDuration);
                    deathAnimFrame = (newFrame < PACMAN_DEATH_FRAMES) ? newFrame : (PACMAN_DEATH_FRAMES - 1);
                }
                break;

            case STATE_LEVEL_COMPLETE:
                // Wait for a brief moment before transitioning
                deathAnimTimer -= GetFrameTime();   // Reuse for timing
                if (deathAnimTimer <= 0.0f) {
                    prevState = STATE_LEVEL_COMPLETE;
                    fadingOut = true;
                    nextState = STATE_READY;
                    deathAnimTimer = 6.0f;      // Reset timer
                    reset_game_state(true, STATE_READY);
                }
                break;

            case STATE_GAME_OVER:
                // Particles background
                if (gameOverAnimActive) {
                    gameOverAnimTimer += GetFrameTime();
                    if (gameOverAnimTimer >= 2.0f) {
                        gameOverAnimActive = false;
                    }
                }
                // Update particles
                for (int i = 0; i < MAX_PARTICLES; i++) {
                    if (gameOverParticles[i].active) {
                        gameOverParticles[i].position.x += gameOverParticles[i].velocity.x * GetFrameTime();
                        gameOverParticles[i].position.y += gameOverParticles[i].velocity.y * GetFrameTime();
                        gameOverParticles[i].lifetime -= GetFrameTime();
                        if (gameOverParticles[i].lifetime <= 0) {
                            gameOverParticles[i].active = false;
                        }
                    }
                }

                // Name input
                if (!nameInputComplete) {
                    int key = GetKeyPressed();
                    while (key > 0) {
                        // Accept A-Z keys
                        if (key >= KEY_A && key <= KEY_Z && nameInputIndex < 3) {
                            playerNameInput[nameInputIndex] = (char)key;
                            nameInputIndex ++;
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }

                        // Backspace to erase last character
                        if (key == KEY_BACKSPACE && nameInputIndex > 0) {
                            nameInputIndex --;
                            playerNameInput[nameInputIndex] = '_';
                            if (!soundMuted) {
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
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
                                SetSoundVolume(sfx_menu_nav, pacmanSfxVolume);
                                PlaySound(sfx_menu_nav);
                            }
                        }
                        key = GetKeyPressed();
                    }   
                }    

                // Allow return to menu after name input
                if (nameInputComplete && IsKeyPressed(KEY_ENTER)) {
                    fadingOut = true;
                    nextState = STATE_MENU;
                    prevState = STATE_GAME_OVER;
                    selectedOption = 0;
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
                }
                break;
        
            default:
                break;
        }

        // Start Rendering
        // ----------------------------------------------------------------------------------------
        BeginDrawing();

        // Render lives as Pac-Man sprites
        float scaleFactor = (float)TILE_SIZE / 16.0f;
        float scaledWidth = 16.0f * scaleFactor;
        float scaledHeight = 16.0f * scaleFactor;

        Rectangle sourceRec = { 18.0f, 0.0f, 16.0f, 16.0f};
        Vector2 origin = { scaledWidth / 2.0f, scaledHeight / 2.0f};
        float livesStartX = mazeOffsetX + mazePixelWidth - 80;

        // Render collected fruits
        Rectangle fruitSourceRec = { 35.0f, 48.0f, 16.0f, 16.0f};
        float fruitsStartX = mazeOffsetX + 50;

        switch (gameState) {
            case STATE_KOOLDUDE_LOGO:
                render_kool_dude_logo(&logoAnim, font);
                break;

            case STATE_DEV_LOGO:
                render_dev_logo(&logoAnim, screenWidth, screenHeight, font);
                break;

            case STATE_RAYLIB_LOGO:
                render_raylib_logo(&logoAnim);
                break;

            case STATE_LOGO:
                render_game_logo(&logoAnim, screenWidth, screenHeight, font);
                break;

            case STATE_MENU:
                ClearBackground(BLACK);
                DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 100}, 20.0f, 1, YELLOW);
                DrawTextEx(font, "Start", (Vector2){screenWidth / 2 - 20, screenHeight / 2 - 40}, 16.0f, 1, selectedOption == 0 ? YELLOW : WHITE);
                DrawTextEx(font, "Highscores", (Vector2){screenWidth / 2 - 40, screenHeight / 2 - 10}, 16.0f, 1, selectedOption == 1 ? YELLOW : WHITE);
                DrawTextEx(font, "About", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 20}, 16.0f, 1, selectedOption == 2 ? YELLOW : WHITE);
                DrawTextEx(font, "Settings", (Vector2){screenWidth / 2 - 30, screenHeight / 2 + 50}, 16.0f, 1, selectedOption == 3 ? YELLOW : WHITE);
                DrawTextEx(font, "Exit", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 80}, 16.0f, 1, selectedOption == 4 ? YELLOW : WHITE);
                DrawTextEx(font, "Use UP/DOWN to select, ENTER to confirm", (Vector2){screenWidth / 2 - 120, screenHeight / 2 + 110}, 10.0f, 1, GRAY);
                break;

                case STATE_HIGHSCORES:
                ClearBackground(BLACK);
                DrawTextEx(font, "High Scores", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 100}, 20.0f, 1, YELLOW);
                for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                    char scoreText[32];
                    sprintf(scoreText, "%d. %s - %d", i + 1, highscores[i].name, highscores[i].score);
                    DrawTextEx(font, scoreText, (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 60 + i * 20}, 16.0f, 1, WHITE);
                }
                DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 100}, 10.0f, 1, GRAY);
                break;

            case STATE_ABOUT:
                ClearBackground(BLACK);
                DrawTextEx(font, "About Pac-Man Remake", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 100}, 20.0f, 1, YELLOW);
                DrawTextEx(font, "Developed by Kyle Ibo", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 40}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Powered by Raylib", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Version 1.0", (Vector2){screenWidth / 2 - 30, screenHeight / 2 + 20}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 80}, 10.0f, 1, GRAY);
                break;

            case STATE_SETTINGS:
                ClearBackground(BLACK);
                render_settings_menu(screenWidth, screenHeight, font, selectedOption);
                break;

            case STATE_READY:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "READY!", (Vector2){screenWidth / 2 - 35, mazeOffsetY + (14 * TILE_SIZE + 3)}, 16.0f, 1, YELLOW);
                DrawTextEx(font, "Score: ", (Vector2){mazeOffsetX + 10, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", pacman.score), (Vector2){mazeOffsetX + 30, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Level: ", (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", level), (Vector2){mazeOffsetX + mazePixelWidth - 70, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "High Score: ", (Vector2){screenWidth / 2 - 70, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", highscores[0].score), (Vector2){screenWidth / 2 - 50, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 170, screenHeight - 40}, 16.0f, 1, WHITE);
                // Draw lives as Pac-Man sprites
                for (int i = 0; i < pacman.lives; i++) {
                    Rectangle destRec = {
                        livesStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
                }

                // Draw collected fruits
                for (int i = 0; i < totalFruitsCollected; i++) {
                    Rectangle destRec = {
                        fruitsStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30, // Same row as lives
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(fruit.sprite, fruitSourceRec, destRec, origin, 0.0f, WHITE);
                }
                break;


            case STATE_PLAYING:
            case STATE_GHOST_EATEN:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Score: ", (Vector2){mazeOffsetX + 10, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", pacman.score), (Vector2){mazeOffsetX + 30, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Level: ", (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", level), (Vector2){mazeOffsetX + mazePixelWidth - 70, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "High Score: ", (Vector2){screenWidth / 2 - 70, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", highscores[0].score), (Vector2){screenWidth / 2 - 50, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 170, screenHeight - 40}, 16.0f, 1, WHITE);
                
                // Draw lives as Pac-Man sprites
                for (int i = 0; i < pacman.lives; i++) {
                    Rectangle destRec = {
                        livesStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
                }

                // Draw collected fruits
                for (int i = 0; i < totalFruitsCollected; i++) {
                    Rectangle destRec = {
                        fruitsStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30, // Same row as lives
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(fruit.sprite, fruitSourceRec, destRec, origin, 0.0f, WHITE);
                }
                break;
            
            case STATE_PAUSED:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Score: ", (Vector2){mazeOffsetX + 10, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", pacman.score), (Vector2){mazeOffsetX + 30, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Level: ", (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", level), (Vector2){mazeOffsetX + mazePixelWidth - 70, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "High Score: ", (Vector2){screenWidth / 2 - 70, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", highscores[0].score), (Vector2){screenWidth / 2 - 50, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 170, screenHeight - 40}, 16.0f, 1, WHITE);
                
                // Draw lives as Pac-Man sprites
                for (int i = 0; i < pacman.lives; i++) {
                    Rectangle destRec = {
                        livesStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
                }

                // Draw collected fruits
                for (int i = 0; i < totalFruitsCollected; i++) {
                    Rectangle destRec = {
                        fruitsStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30, // Same row as lives
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(fruit.sprite, fruitSourceRec, destRec, origin, 0.0f, WHITE);
                }

                // Draw semi-transparent black overlay
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 150});
                
                // Render pause or settings menu
                if (pauseMenuState == PAUSE_MENU_MAIN) {
                    render_pause_menu(screenWidth, screenHeight, font);
                } else {
                    render_settings_menu(screenWidth, screenHeight, font, pauseSelectedOption);
                }

                break;

            case STATE_DEATH_ANIM:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman_death(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Score: ", (Vector2){mazeOffsetX + 10, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", pacman.score), (Vector2){mazeOffsetX + 30, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 170, screenHeight - 40}, 16.0f, 1, WHITE);

                // Draw lives as Pac-Man sprites
                for (int i = 0; i < pacman.lives; i++) {
                    Rectangle destRec = {
                        livesStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
                }

                // Draw collected fruits
                for (int i = 0; i < totalFruitsCollected; i++) {
                    Rectangle destRec = {
                        fruitsStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30, // Same row as lives
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(fruit.sprite, fruitSourceRec, destRec, origin, 0.0f, WHITE);
                }
                break;

            case STATE_LEVEL_COMPLETE:
                ClearBackground(BLACK);
                
                // Fade-out-to-black background effect
                float bgAlpha = deathAnimTimer / 6.0f; // 1.0 (fully visible) to 0.0 (black) over 6 seconds
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 1.0f - bgAlpha));
            
                // Render maze, Pac-Man, and ghosts
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
            
                // Static "Level Complete!" text
                const char* levelCompleteText = "Level Complete!";
                Vector2 textSize = MeasureTextEx(font, levelCompleteText, 20.0f, 1);
                Vector2 textPos = { screenWidth / 2.0f - textSize.x / 2.0f, screenHeight / 2.0f - 80 };
                DrawTextEx(font, levelCompleteText, textPos, 20.0f, 1, YELLOW);
            
                // Fading "Preparing Level X..." subtitle
                float subtitleAlpha = (deathAnimTimer < 4.0f) ? (1.0f - deathAnimTimer / 4.0f) : 0.0f;
                char subtitle[32];
                snprintf(subtitle, sizeof(subtitle), "Preparing Level %d...", level);
                textSize = MeasureTextEx(font, subtitle, 16.0f, 1);
                textPos = (Vector2){ screenWidth / 2.0f - textSize.x / 2.0f, screenHeight / 2.0f - 40 };
                DrawTextEx(font, subtitle, textPos, 16.0f, 1, Fade(WHITE, subtitleAlpha));
            
                // Score breakdown with sequential fade-in
                float elapsedTime = 6.0f - deathAnimTimer; // Time since entering STATE_LEVEL_COMPLETE
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
                int pelletsScore = pelletsEaten * 10;
                int powerPelletsScore = powerPelletsEaten * 50;
                int ghostPoints = 0;
                for (int i = 1; i <= totalGhostsEaten; i++) {
                    ghostPoints += 200 * (1 << (i - 1)); // 200, 400, 800, 1600
                }
                int fruitScore = totalFruitsCollected * fruit.points;

                // Measure text widths for score breakdown
                char pelletsText[32];
                char powerPelletsText[32];
                char ghostsText[32];
                char fruitsText[32];
                snprintf(pelletsText, sizeof(pelletsText), "Pellets: %d x 10 = %d", pelletsEaten, pelletsScore);
                snprintf(powerPelletsText, sizeof(powerPelletsText), "Power Pellets: %d x 50 = %d", powerPelletsEaten, powerPelletsScore);
                snprintf(ghostsText, sizeof(ghostsText), "Ghosts Eaten: %d = %d", totalGhostsEaten, ghostPoints);
                snprintf(fruitsText, sizeof(fruitsText), "Fruits: %d x %d = %d", totalFruitsCollected, fruit.points, fruitScore);

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
                float progress = deathAnimTimer / 3.5f;
                float barWidth = maxTextWidth; // Match bar width to text width
                float barHeight = 10.0f;
                Rectangle barOutline = { screenWidth / 2.0f - barWidth / 2.0f, screenHeight / 2.0f + 100, barWidth, barHeight };
                Rectangle barFill = { barOutline.x, barOutline.y, barWidth * progress, barHeight };
                DrawRectangleRec(barOutline, Fade(WHITE, progressBarAlpha * 0.5f)); // Outline with half opacity
                DrawRectangleRec(barFill, Fade(YELLOW, progressBarAlpha));
            
                // Standard HUD elements
                DrawTextEx(font, "Score: ", (Vector2){mazeOffsetX + 10, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", pacman.score), (Vector2){mazeOffsetX + 40, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "High Score: ", (Vector2){screenWidth / 2 - 70, 10}, 16.0f, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", highscores[0].score), (Vector2){screenWidth / 2 - 50, 25}, 16.0f, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 170, screenHeight - 40}, 16.0f, 1, WHITE);
            
                // Draw lives as Pac-Man sprites
                for (int i = 0; i < pacman.lives; i++) {
                    Rectangle destRec = {
                        livesStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(pacman.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
                }
            
                // Draw collected fruits
                for (int i = 0; i < totalFruitsCollected; i++) {
                    Rectangle destRec = {
                        fruitsStartX + (i * (scaledWidth + 5)),
                        screenHeight - 30,
                        scaledWidth,
                        scaledHeight
                    };
                    DrawTexturePro(fruit.sprite, fruitSourceRec, destRec, origin, 0.0f, WHITE);
                }

                printf("pelletsEaten: %d, powerPelletsEaten: %d, totalGhostsEaten: %d, remainingPelletCount: %d\n",
                    pelletsEaten, powerPelletsEaten, totalGhostsEaten, remainingPelletCount);
                break;

            case STATE_GAME_OVER:
                ClearBackground(BLACK);
                
                // Particle effects
                for (int i = 0; i < MAX_PARTICLES; i++) {
                    if (gameOverParticles[i].active) {
                        float alpha = gameOverParticles[i].lifetime / 3.0f;
                        Color particleColor = (Color){
                            (unsigned char)(255 - (i % 3) * 20), // Slight yellow/orange variation
                            (unsigned char)(255 - (i % 3) * 50),
                            0,
                            (unsigned char)(alpha * 255)
                        };
                        DrawCircleV(gameOverParticles[i].position, 3.0f, particleColor);
                    }
                }
                
                // Retro animated Game Over
                float scale = 1.0f + 0.3f * sinf(gameOverAnimTimer * PI * 2.0f); // Pulsate forever
                float alpha = gameOverAnimActive ? 1.0f - (gameOverAnimTimer / 2.0f) : 1.0f;
                Color textColor = (Color){
                    (unsigned char)(255 * (1.0f - gameOverAnimTimer / 4.0f)),
                    (unsigned char)(gameOverAnimTimer * 128.0f / 2.0f),
                    0,
                    (unsigned char)(alpha * 255)
                };
                DrawTextEx(font, "Game Over", 
                        (Vector2){screenWidth / 2 - MeasureTextEx(font, "Game Over", 24.0f * scale, 1).x / 2, screenHeight / 2 - 100}, 
                        24.0f * scale, 1, textColor);
                
                // Score and motivational message
                float messageScale = 1.0f + 0.1f * sinf(gameOverAnimTimer * PI * 3.0f); // Bounce effect
                DrawTextEx(font, TextFormat("Final Score: %d", pacman.score), 
                        (Vector2){screenWidth / 2 - MeasureTextEx(font, TextFormat("Final Score: %d", pacman.score), 16.0f, 1).x / 2, screenHeight / 2 - 40}, 
                        16.0f, 1, WHITE);
                DrawTextEx(font, gameOverMessages[selectedMessageIndex], 
                        (Vector2){screenWidth / 2 - MeasureTextEx(font, gameOverMessages[selectedMessageIndex], 12.0f * messageScale, 1).x / 2, screenHeight / 2 - 10}, 
                        12.0f * messageScale, 1, WHITE);
                
                // Name input and high score preview
                if (!nameInputComplete) {
                    DrawTextEx(font, "Enter Initials:", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "Enter Initials:", 16.0f, 1).x / 2, screenHeight / 2 + 20}, 
                            16.0f, 1, YELLOW);
                    char displayName[4] = {'_', '_', '_', '\0'};
                    for (int i = 0; i < nameInputIndex; i++) {
                        displayName[i] = playerNameInput[i];
                    }
                    // Blinking cursor
                    if (((int)(gameOverAnimTimer * 2.0f) % 2) == 0 && nameInputIndex < 3) {
                        displayName[nameInputIndex] = '|';
                    }
                    DrawTextEx(font, displayName, 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, displayName, 16.0f, 1).x / 2, screenHeight / 2 + 50}, 
                            16.0f, 1, WHITE);
                    DrawTextEx(font, "Press ENTER to confirm", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "Press ENTER to confirm", 10.0f, 1).x / 2, screenHeight / 2 + 80}, 
                            10.0f, 1, GRAY);
                    
                    // High score preview with placeholder
                    DrawRectangleLines(screenWidth / 2 - 80, screenHeight / 2 + 100, 160, 120, YELLOW); // Retro border
                    DrawTextEx(font, "High Scores:", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "High Scores:", 16.0f, 1).x / 2, screenHeight / 2 + 110}, 
                            16.0f, 1, YELLOW);
                    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                        char* displayName = (i == 0 && pacman.score > highscores[i].score) ? "YOU" : highscores[i].name;
                        int displayScore = (i == 0 && pacman.score > highscores[i].score) ? pacman.score : highscores[i].score;
                        DrawTextEx(font, TextFormat("%s: %d", displayName, displayScore), 
                                (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 130 + i * 20}, 
                                12.0f, 1, (i == 0 && pacman.score > highscores[i].score) ? GREEN : WHITE);
                    }
                } else {
                    DrawTextEx(font, "Initials:", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "Initials:", 16.0f, 1).x / 2, screenHeight / 2 + 20}, 
                            16.0f, 1, YELLOW);
                    DrawTextEx(font, playerNameInput, 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, playerNameInput, 16.0f, 1).x / 2, screenHeight / 2 + 50}, 
                            16.0f, 1, WHITE);
                    // High score list with retro border
                    DrawRectangleLines(screenWidth / 2 - 80, screenHeight / 2 + 100, 160, 120, YELLOW);
                    DrawTextEx(font, "High Scores:", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "High Scores:", 16.0f, 1).x / 2, screenHeight / 2 + 110}, 
                            16.0f, 1, YELLOW);
                    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                        bool isPlayerScore = (strcmp(highscores[i].name, playerNameInput) == 0 && highscores[i].score == pacman.score);
                        DrawTextEx(font, TextFormat("%s: %d", highscores[i].name, highscores[i].score), 
                                (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 130 + i * 20}, 
                                12.0f, 1, isPlayerScore ? GREEN : WHITE);
                    }
                    DrawTextEx(font, "Press ENTER to Return to Menu", 
                            (Vector2){screenWidth / 2 - MeasureTextEx(font, "Press ENTER to Return to Menu", 10.0f, 1).x / 2, screenHeight / 2 + 230}, 
                            10.0f, 1, GRAY);
                }
                break;

            default:
                break;
        }

        // Apply the fade-to-black transition
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));

        EndDrawing();
    }

    // Unload resources
    UnloadSound(sfx_menu);
    UnloadSound(sfx_menu_nav);
    UnloadSound(sfx_ready);
    UnloadSound(sfx_pacman_move);
    UnloadSound(sfx_pacman_chomp);
    UnloadSound(sfx_pacman_death);
    UnloadSound(sfx_eat_fruit);
    UnloadSound(sfx_eat_ghost);
    UnloadSound(sfx_ghost_frightened);
    UnloadSound(sfx_level_complete);
    UnloadSound(sfx_extra_life);
    UnloadSound(sfx_game_over);
    UnloadFont(font);
    UnloadGhostTextures(ghosts);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}