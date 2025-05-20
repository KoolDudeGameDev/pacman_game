#include "game_logic.h"
#include "logo_animation.h"
#include "rendering.h"
#include "gui.h"

int main(void) {
    // Seed random number generator
    SetRandomSeed((unsigned int)time(NULL));

    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Pacman v1.0");
    Image icon = LoadImage("pac-man-logo-icon.ico");
    SetWindowIcon(icon);
    
    SetTargetFPS(60);

    // Load a font
    Font font = LoadFont("assets/fonts/Emulogic-zrEw.ttf");

    // Load sound effects
    InitAudioDevice();
    sfx_menu = LoadSound("assets/sounds/pacman_menu.mp3");
    sfx_menu_nav = LoadSound("assets/sounds/pacman_menu_nav.wav");
    sfx_ready = LoadSound("assets/sounds/pacman_beginning.wav");
    sfx_pacman_move = LoadSound("assets/sounds/pacman_move.mp3");
    sfx_pacman_chomp = LoadSound("assets/sounds/pacman_chomp.wav");
    sfx_pacman_death = LoadSound("assets/sounds/pacman_death.wav");
    sfx_eat_fruit = LoadSound("assets/sounds/pacman_eatfruit.wav");
    sfx_eat_ghost = LoadSound("assets/sounds/pacman_eatghost.wav");
    sfx_ghost_frightened = LoadSound("assets/sounds/pacman_ghost_frightened.wav");
    sfx_level_complete = LoadSound("assets/sounds/pacman_level_complete.mp3");
    sfx_extra_life = LoadSound("assets/sounds/pacman_extralife.wav");
    sfx_game_over = LoadSound("assets/sounds/pacman_game_over.mp3");

    // Set initial volumes
    SetSoundVolume(sfx_menu, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
    SetSoundVolume(sfx_menu_nav, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_ready, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_pacman_move, soundMuted ? 0.0f : bgMusicVolume * 0.3f);
    SetSoundVolume(sfx_pacman_chomp, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_pacman_death, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_eat_fruit, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_eat_ghost, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_ghost_frightened, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_level_complete, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_extra_life, soundMuted ? 0.0f : sfxVolume);
    SetSoundVolume(sfx_game_over, soundMuted ? 0.0f : sfxVolume);
    
    // Calculate maze dimensions and offsets
    const int mazePixelWidth = MAZE_WIDTH * TILE_SIZE;              // 560px
    const int mazePixelHeight = MAZE_HEIGHT * TILE_SIZE;            // 620px
    const int mazeOffsetX = (screenWidth - mazePixelWidth) / 2;     // 360px (180px on each side)
    const int mazeOffsetY = (screenHeight - mazePixelHeight) / 2;   // 50px (top and bottom)

    // Menu variables
    int selectedOption = 0;
    bool shouldExit = false;

    static bool isMenuLoopPlaying = false;
    bool pausedThisFrame = false;                   // Tracks if pause was toggled this frame
    static bool levelCompleteSfxStopped = false;    // Flag to track if we've stopped SFX in STATE_LEVEL_COMPLETE
    static bool deathSfxPlayed = false;            // Flag to track if death SFX has played in STATE_DEATH_ANIM
    
    // Logo animation state
    LogoAnimation logoAnim;
    init_personal_logo(&logoAnim, screenWidth, screenHeight);       // First state

    // Load ghost textures
    LoadGhostTextures(ghosts);

    // Initialize fruit
    init_fruit();

    // Load high scores
    load_high_scores();

    // Initialize GUI variables
    init_gui();

    // Fade to black transition variables
    float transitionAlpha = 0.0f;
    bool fadingOut = false;
    GameState nextState = STATE_PERSONAL_LOGO;
    GameState prevState = STATE_MENU;

    // Game over fade transition
    float gameOverFadeAlpha = 0.0f; // Controls fade-in/out for game over screen
    bool gameOverFadingIn = true;   // True during fade-in, false during fade-out

    // Disable ESC key from closing the window
    SetExitKey(KEY_F10);

    // Game Loop
    // ----------------------------------------------------------------------------------------
    while (!(shouldExit || WindowShouldClose())) {

        // Toggle fullscreen with F11
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Handle Enter key to skip animations
        if (IsKeyPressed(KEY_ENTER)) {
            switch (gameState) {
                case STATE_PERSONAL_LOGO:
                    logoAnim.skip = true;
                    fadingOut = true;
                    nextState = STATE_RAYLIB_LOGO;
                    init_raylib_logo(&logoAnim, screenWidth, screenHeight);
                    if (!soundMuted) {
                        PlaySound(sfx_menu_nav);
                    }
                    break;
                case STATE_RAYLIB_LOGO:
                    logoAnim.skip = true;
                    fadingOut = true;
                    nextState = STATE_LOGO;
                    init_game_logo(&logoAnim);
                    if (!soundMuted) {
                        PlaySound(sfx_menu_nav);
                    }
                    break;
                case STATE_LOGO:
                    if (!logoAnim.enterPressed) {
                        logoAnim.skip = true;
                        if (!soundMuted) {
                            PlaySound(sfx_menu_nav);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        if (IsKeyPressed(KEY_ZERO)) { // Press '0' to trigger state personal
            TraceLog(LOG_INFO, "Forcing transition to STATE_PERSONAL_LOGO");
            fadingOut = true;
            nextState = STATE_PERSONAL_LOGO;
            prevState = gameState;
        }

        if (IsKeyPressed(KEY_NINE)) { // Press '9' to trigger state logo
            TraceLog(LOG_INFO, "Forcing transition to STATE_LOGO");
            fadingOut = true;
            nextState = STATE_LOGO;
            prevState = gameState;
        }

        if (IsKeyPressed(KEY_ONE)) { // Press '1' to trigger state menu
            TraceLog(LOG_INFO, "Forcing transition to STATE_MENU");
            fadingOut = true;
            nextState = STATE_MENU;
            prevState = gameState;
        }

        if (IsKeyPressed(KEY_TWO)) { // Press '2' to trigger state level complete
            TraceLog(LOG_INFO, "Forcing transition to STATE_LEVEL_COMPLETE");
            fadingOut = true;
            nextState = STATE_LEVEL_COMPLETE;
            prevState = gameState;
        }

         if (IsKeyPressed(KEY_THREE)) { // Press '3' to trigger state game over
            TraceLog(LOG_INFO, "Forcing transition to STATE_GAME_OVER");
            fadingOut = true;
            nextState = STATE_GAME_OVER;
            prevState = gameState;
        }

        // Handle Enter key for STATE_LOGO
        if (gameState == STATE_LOGO && IsKeyPressed(KEY_ENTER) && !logoAnim.enterPressed) {
            logoAnim.enterPressed = true;
            logoAnim.enterPressedTime = logoAnim.timer;
            if (!soundMuted) {
                PlaySound(sfx_menu_nav);
            }
        }

        // Fade to black transition for state changes
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
                    if (prevState == STATE_MENU) {
                        reset_game_state(true, STATE_READY);    // Only reset for new game from menu
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
                    if (!soundMuted) {
                        SetSoundVolume(sfx_menu, bgMusicVolume * 0.3f);
                        PlaySound(sfx_menu);
                    }
                    playPacmanMove = false;
                    StopSound(sfx_pacman_move);
                }

                // Initialize game over state
                if (gameState == STATE_GAME_OVER) {
                    gameOverFadeAlpha = 0.0f;
                    gameOverFadingIn = true;
                    select_game_over_message();
                    if (!soundMuted) {
                        SetSoundVolume(sfx_game_over, sfxVolume);
                        PlaySound(sfx_game_over);
                    }
                    playPacmanMove = false;
                    StopSound(sfx_pacman_move);
                }

                // Reset game over fade when returning to menu
                if (gameState == STATE_MENU && prevState == STATE_GAME_OVER) {
                    gameOverFadeAlpha = 0.0f;
                    gameOverFadingIn = true;
                    StopSound(sfx_game_over);
                }

                // Stop menu loop sound when exiting STATE_MENU
                if (prevState == STATE_MENU && isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }

                // Stop logo background sound when exiting STATE_LOGO
                if (prevState == STATE_LOGO) {
                    StopSound(sfx_ready);
                }
            }
        } else if (transitionAlpha > 0.0f) {
            transitionAlpha -= 0.05f;
            if (transitionAlpha < 0.0f) {
                transitionAlpha = 0.0f;
            }
        }

        // Handle game over fade-in
        if (gameState == STATE_GAME_OVER && gameOverFadingIn) {
            gameOverFadeAlpha += 0.05f;
            if (gameOverFadeAlpha >= 1.0f) {
                gameOverFadeAlpha = 1.0f;
                gameOverFadingIn = false;
            }
        }

        switch (gameState) {
            case STATE_PERSONAL_LOGO:
                if (update_personal_logo(&logoAnim)) {
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
                if (!IsSoundPlaying(sfx_menu) && !soundMuted) {
                    StopSound(sfx_game_over);
                    SetSoundVolume(sfx_menu, bgMusicVolume * 0.3f);
                    PlaySound(sfx_menu);
                    isMenuLoopPlaying = true;
                }
                handle_menu_input(&selectedOption, &shouldExit);
                if (shouldExit) {
                    fadingOut = true;
                    nextState = STATE_MENU;
                }
                break; 

            case STATE_HIGHSCORES:
                handle_highscores_input(&selectedOption);
                break;

            case STATE_ABOUT:
                handle_about_input(&selectedOption);
                break;

            case STATE_SETTINGS:
                handle_settings_input(&selectedOption);
                break;
            
            case STATE_READY:
                // Ensure menu loop stops when entering READY
                if (isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }

                // Stop movement sound
                StopSound(sfx_pacman_move);
                playPacmanMove = false;

                readyTimer -= GetFrameTime();
                if (readyTimer <= 0.0f) {
                    gameState = STATE_PLAYING;
                    readyTimer = 0.0f;
                    StopSound(sfx_ready);
                    playPacmanMove = true;      // Start movement sfx when entering PLAYING
                }
                break;
            
            case STATE_PLAYING:
                update_pacman();
                update_ghosts();
                update_fruit();

                if (!soundMuted && !IsSoundPlaying(sfx_pacman_move)) {
                    SetSoundVolume(sfx_pacman_move, bgMusicVolume * 0.3f);
                    PlaySound(sfx_pacman_move);
                }
                if (powerPelletTimer > 0.0f) {
                    powerPelletTimer -= GetFrameTime();
                    if (powerPelletTimer < 0.0f) {
                        powerPelletTimer = 0.0f;
                    }
                }
        
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                    gameState = STATE_PAUSED;
                    playPacmanMove = false;
                    StopSound(sfx_pacman_move);
                }
                break;

            case STATE_PAUSED:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    gameState = STATE_PLAYING;
                    playPacmanMove = true;      // Resume movement sfx
                }
                // Stop movement sound when entering pause
                if (!pausedThisFrame) {
                    StopSound(sfx_pacman_move);
                    playPacmanMove = false;
                }

                if (handle_pause_input(&pausedThisFrame)) {
                    if (pauseSelectedOption == 3) { // Quit
                        fadingOut = true;
                        nextState = STATE_MENU;
                        prevState = STATE_PAUSED;
                        playPacmanMove = true;
                        StopSound(sfx_pacman_move);
                    }
                }
                pausedThisFrame = false;
                break;

            case STATE_GHOST_EATEN:
                ghostEatenTimer -= GetFrameTime();
                if (ghostEatenTimer <= 0.0f) {
                    eatenGhostIndex = -1;   // Reset for next ghost
                    gameState = STATE_PLAYING;
                    playPacmanMove = true;
                    // Resume frightened sound if any ghost is still frightened
                    if (isFrightenedSoundPaused) {
                        bool anyFrightened = false;
                        for (int i = 0; i < MAX_GHOSTS; i++) {
                            if (ghosts[i].state == GHOST_FRIGHTENED && ghosts[i].stateTimer > 0.0f) {
                                anyFrightened = true;
                                break;
                            }
                        }
                        if (anyFrightened && !soundMuted) {
                            SetSoundVolume(sfx_ghost_frightened, sfxVolume);
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
                // Play death sound once when entering state
                if (!deathSfxPlayed && !soundMuted) {
                    StopSound(sfx_pacman_move);
                    StopSound(sfx_ghost_frightened);
                    SetSoundVolume(sfx_pacman_death, sfxVolume);
                    PlaySound(sfx_pacman_death);
                    deathSfxPlayed = true;
                }

                deathAnimTimer -= GetFrameTime();
                if (deathAnimTimer <= 0.0f) {
                    // Decrement lives at the end of the animation
                    pacman.lives--;
                    deathAnimTimer = 6.0f;  // Reset timer for next death
                    deathAnimFrame = 0;
                    deathSfxPlayed = false; // Reset flag for next death
                    if (pacman.lives > 0) {
                        reset_game_state(false, STATE_READY);
                        gameState = STATE_READY;
                        playPacmanMove = false;
                    } else {
                        fadingOut = true;
                        nextState = STATE_GAME_OVER;
                        prevState = STATE_DEATH_ANIM;
                        gameOverFadeAlpha = 0.0f;
                        gameOverFadingIn = true;
                        playPacmanMove = false;
                        StopSound(sfx_pacman_move);
                    }
                } else {
                    // Update death animation frame based on time
                    // Total animation duration is 6 secs
                    float frameDuration = 6.0f / PACMAN_DEATH_FRAMES;
                    int newFrame = (int)((6.0f - deathAnimTimer) / frameDuration);
                    deathAnimFrame = (newFrame < PACMAN_DEATH_FRAMES) ? newFrame : (PACMAN_DEATH_FRAMES - 1);
                }
                break;

            case STATE_LEVEL_COMPLETE:
                // Play level complete sound once when entering state
                if (!levelCompleteSfxStopped && !soundMuted) {
                    StopSound(sfx_pacman_move);
                    StopSound(sfx_ghost_frightened);
                    SetSoundVolume(sfx_level_complete, sfxVolume);
                    PlaySound(sfx_level_complete);
                    levelCompleteSfxStopped = true;
                }

                // Wait for a brief moment before transitioning
                deathAnimTimer -= GetFrameTime();   // Reuse for timing
                if (deathAnimTimer <= 0.0f) {
                    prevState = STATE_LEVEL_COMPLETE;
                    fadingOut = true;
                    nextState = STATE_READY;
                    deathAnimTimer = 6.0f;      // Reset timer
                    reset_game_state(false, STATE_READY);
                    //level++;                    // Increment level for next iteration
                    levelCompleteSfxStopped = false;    // Reset flag for next level
                    playPacmanMove = false;
                    StopSound(sfx_pacman_move);
                }
                break;

            case STATE_GAME_OVER:
                handle_game_over_input(&selectedOption);
                break;
        
            default:
                break;
        }

        // Start Rendering
        // ----------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

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
            case STATE_PERSONAL_LOGO:
                render_personal_logo(&logoAnim, screenWidth, screenHeight, font, sfx_menu_nav);
                break;

            case STATE_RAYLIB_LOGO:
                render_raylib_logo(&logoAnim);
                break;

            case STATE_LOGO:
                render_game_logo(&logoAnim, screenWidth, screenHeight, font, pacman.sprite, ghosts, sfx_menu_nav, sfx_ready);
                break;

            case STATE_MENU:
                render_menu(screenWidth, screenHeight, font, selectedOption, pacman.sprite, ghosts);
                break;

            case STATE_HIGHSCORES:
                render_highscores(screenWidth, screenHeight, font);
                break;

            case STATE_ABOUT:
                render_about(screenWidth, screenHeight, font);
                break;

            case STATE_SETTINGS:
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
                render_fruit(mazeOffsetX, mazeOffsetY);
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
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                render_fruit(mazeOffsetX, mazeOffsetY);
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
                // Render pause menu
                render_pause_menu(screenWidth, screenHeight, font);
                break;

            case STATE_DEATH_ANIM:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman_death(mazeOffsetX, mazeOffsetY);
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

            case STATE_LEVEL_COMPLETE:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                render_fruit(mazeOffsetX, mazeOffsetY);

                // Call the new GUI function for level complete screen
                render_level_complete(screenWidth, screenHeight, font, deathAnimTimer, level,
                         pelletsEaten, powerPelletsEaten, totalGhostsEaten,
                         totalFruitsCollected, fruit.points);

                // Standard HUD elements
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

            case STATE_GAME_OVER:
                render_game_over(screenWidth, screenHeight, font, gameOverFadeAlpha);
                break;

            default:
                break;
        }

        // Draw fade-to-black transition
        if (transitionAlpha > 0.0f) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));
        }

        EndDrawing();
        // ----------------------------------------------------------------------------------------
    }

    // De-Initialization
    // ----------------------------------------------------------------------------------------
    UnloadImage(icon);
    UnloadFont(font);
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
    UnloadGhostTextures(ghosts);
    UnloadTexture(pacman.sprite);
    UnloadTexture(fruit.sprite);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}