#include "game_logic.h"
#include "logo_animation.h"
#include "rendering.h"

#include <stdio.h>

int main(void) {
    // Seed random number generator
    SetRandomSeed((unsigned int)time(NULL));

    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Pacman v1.0");
    //ToggleFullscreen();
    InitAudioDevice();
    SetTargetFPS(60);

    // Load a font
    Font font = GetFontDefault();

    // Load sound effects
    sfx_menu = LoadSound("assets/sounds/pacman_menu.wav");
    sfx_menu_nav = LoadSound("assets/sounds/pacman_menu_nav.wav");
    sfx_ready = LoadSound("assets/sounds/pacman_beginning.wav");
    sfx_pacman_move = LoadSound("assets/sounds/pacman_movement.wav");
    sfx_pacman_chomp = LoadSound("assets/sounds/pacman_chomp.wav");
    sfx_pacman_death = LoadSound("assets/sounds/pacman_death.wav");
    sfx_eat_fruit= LoadSound("assets/sounds/pacman_eatfruit.wav");
    sfx_eat_ghost= LoadSound("assets/sounds/pacman_eatghost.wav");
    sfx_ghost_frightened = LoadSound("assets/sounds/pacman_ghost_frightened.wav");
    sfx_intermission= LoadSound("assets/sounds/pacman_intermission.wav");
    sfx_extra_life= LoadSound("assets/sounds/pacman_extralife.wav");
    

    // Calculate maze dimensions and offsets
    const int mazePixelWidth = MAZE_WIDTH * TILE_SIZE;              // 560px
    const int mazePixelHeight = MAZE_HEIGHT * TILE_SIZE;            // 620px
    const int mazeOffsetX = (screenWidth - mazePixelWidth) / 2;     // 360px (180px on each side)
    const int mazeOffsetY = (screenHeight - mazePixelHeight) / 2;   // 50px (top and bottom)

    // Menu variables
    int selectedOption = 0;
    bool shouldExit = false;

    static bool isMenuLoopPlaying = false;
    
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

    // Game Loop
    // ----------------------------------------------------------------------------------------
    while (!(shouldExit || WindowShouldClose())) {

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
                    reset_game_state();
                    if (prevState == STATE_LEVEL_COMPLETE || prevState == STATE_MENU) {
                        pelletsEaten = 0;
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
                if (!isMenuLoopPlaying && !IsSoundPlaying(sfx_menu)) {
                    PlaySound(sfx_menu);
                    isMenuLoopPlaying = true;
                }

                // Navigate menu options
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedOption = (selectedOption + 1) % 4;
                    PlaySound(sfx_menu_nav);
                }
                if (IsKeyPressed(KEY_UP)) {
                    selectedOption = (selectedOption - 1 + 4) % 4;
                    PlaySound(sfx_menu_nav);
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedOption == 0) {      // START
                        init_maze();
                        level = 1;                  // Reset level to 1 when starting a new game
                        pacman.score = 0;           // Reset score for new game
                        pacman.lives = 3;           // Reset lives for new game
                        totalFruitsCollected = 0;   // Reset fruit count
                        reset_game_state();         // Initialize game state and play sfx_ready                      
                    } else if (selectedOption == 1) { // HIGHSCORES
                        gameState = STATE_HIGHSCORES;
                    } else if (selectedOption == 2) {
                        gameState = STATE_ABOUT;    // ABOUT
                    } else {                        // EXIT
                        CloseWindow();
                        return 0;
                    }
                    PlaySound(sfx_menu_nav);
                }
                break;

            case STATE_HIGHSCORES:
                // Ensure menu loop stops when entering HIGHSCORES
                if (isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }

                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    gameState = STATE_MENU;
                    selectedOption = 1;
                    PlaySound(sfx_menu_nav);
                }
                break;

            case STATE_ABOUT:
                // Ensure menu loop stops when entering ABOUT
                if (isMenuLoopPlaying) {
                    StopSound(sfx_menu);
                    isMenuLoopPlaying = false;
                }

                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    gameState = STATE_MENU;
                    selectedOption = 2;
                    PlaySound(sfx_menu_nav);
                }
                break;
            
            case STATE_READY:
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
                //printf("pelletsEaten = %d, fruit.active = %d\n", pelletsEaten, fruit.active); // Debug print
                if (powerPelletTimer > 0.0f) {
                    powerPelletTimer -= GetFrameTime();
                    if (powerPelletTimer <= 0.0f) {
                        powerPelletTimer = 0.0f;
                        StopSound(sfx_ghost_frightened);
                        isFrightenedSoundPaused = false;
                    }
                }
                if (IsKeyPressed(KEY_P)) {
                    gameState = STATE_PAUSED;
                }
                break;

            case STATE_PAUSED:
                if (IsKeyPressed(KEY_P)) {
                    gameState = STATE_PLAYING;
                }
                break;

            case STATE_GHOST_EATEN:
                ghostEatenTimer -= GetFrameTime();

                // Pause frightened sound during ghost eaten animation
                if (!isFrightenedSoundPaused) {
                    PauseSound(sfx_ghost_frightened);
                    isFrightenedSoundPaused = true;
                }

                if (ghostEatenTimer <= 0.0f) {
                    eatenGhostIndex = -1;   // Reset for next ghost
                    gameState = STATE_PLAYING;
                    // Resume frightened sound if power pellet effect is still active
                    if (powerPelletTimer > 0.0f && isFrightenedSoundPaused) {
                        ResumeSound(sfx_ghost_frightened);
                        isFrightenedSoundPaused = false;
                    }
                }
                break;

            case STATE_DEATH_ANIM:
                deathAnimTimer -= GetFrameTime();
                if (deathAnimTimer <= 0.0f) {
                    reset_game_state();
                    deathAnimTimer = 0.0f;
                } else {
                    // Update death animation frame based on time
                    // Total animation duration is 2 secs
                    float frameDuration = 2.0f / PACMAN_DEATH_FRAMES;
                    int newFrame = (int)((2.0f - deathAnimTimer) / frameDuration);
                    deathAnimFrame = (newFrame < PACMAN_DEATH_FRAMES) ? newFrame : (PACMAN_DEATH_FRAMES - 1);
                }
                break;

            case STATE_LEVEL_COMPLETE:
                // Wait for a brief moment before transitioning
                deathAnimTimer -= GetFrameTime();   // Reuse for timing
                if (deathAnimTimer <= 0.0f) {
                    fadingOut = true;
                    nextState = STATE_READY;
                    deathAnimTimer = 6.0f;      // Reset timer
                }
                break;

            case STATE_GAME_OVER:
                // Update high scores before returning to menu
                check_and_update_high_scores(pacman.score);
                save_high_scores();
                if (IsKeyPressed(KEY_R)) {
                    gameState = STATE_MENU;
                    selectedOption = 0;
                    // Reset score, lives, and level for a new game
                    pacman.score = 0;
                    pacman.lives = 3;
                    level = 1;
                    pelletsEaten = 0;
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
                DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 100}, 30, 1, YELLOW);
                DrawTextEx(font, "Start", (Vector2){screenWidth / 2 - 20, screenHeight / 2 - 40}, 20, 1, selectedOption == 0 ? YELLOW : WHITE);
                DrawTextEx(font, "Highscores", (Vector2){screenWidth / 2 - 40, screenHeight / 2 - 10}, 20, 1, selectedOption == 1 ? YELLOW : WHITE);
                DrawTextEx(font, "About", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 20}, 20, 1, selectedOption == 2 ? YELLOW : WHITE);
                DrawTextEx(font, "Exit", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 50}, 20, 1, selectedOption == 3 ? YELLOW : WHITE);
                DrawTextEx(font, "Use UP/DOWN to select, ENTER to confirm", (Vector2){screenWidth / 2 - 120, screenHeight / 2 + 80}, 15, 1, GRAY);
                break;

            case STATE_HIGHSCORES:
                ClearBackground(BLACK);
                DrawTextEx(font, "High Scores", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 100}, 30, 1, YELLOW);
                for (int i = 0; i < MAX_HIGH_SCORES; i ++) {
                    char scoreText[32];
                    sprintf(scoreText, "%d. %s - %d", i + 1, highscores[i].name, highscores[i].score);
                    DrawTextEx(font, scoreText, (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 60 + i * 30}, 20, 1, WHITE);
                }
                DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 100}, 15, 1, GRAY);
                break;

            case STATE_ABOUT:
                ClearBackground(BLACK);
                DrawTextEx(font, "About Pac-Man Remake ", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 100}, 30, 1, YELLOW);
                DrawTextEx(font, "Developed by Kyle Ibo", (Vector2){screenWidth / 2 - 80, screenHeight / 2 - 40}, 20, 1, WHITE);
                DrawTextEx(font, "Powered by Raylib", (Vector2){screenWidth / 2 - 60, screenHeight / 2 - 10}, 20, 1, WHITE);
                DrawTextEx(font, "Version 1.0", (Vector2){screenWidth / 2 - 40, screenHeight / 2 + 20}, 20, 1, WHITE);
                DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){screenWidth / 2 - 100, screenHeight / 2 + 80}, 15, 1, GRAY);
                break;

            case STATE_READY:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "READY!", (Vector2){screenWidth / 2 - 30, mazeOffsetY + (14 * TILE_SIZE + 3)}, 18, 1, YELLOW);     // Position at row 14 (13 in 0-based index)
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Level: %d", level), (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 20, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 150, screenHeight - 40}, 20, 1, WHITE);
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
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Level: %d", level), (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Top Score: %d", highscores[0].score), (Vector2){screenWidth / 2 - 50, 10}, 20, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 150, screenHeight - 40}, 20, 1, WHITE);
                
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
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Paused", (Vector2){screenWidth / 2 - 30, screenHeight / 2}, 20, 1, WHITE);
                DrawTextEx(font, "Press P to Resume", (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 30}, 20, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 150, screenHeight - 40}, 20, 1, WHITE);
                
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

            case STATE_DEATH_ANIM:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman_death(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 150, screenHeight - 40}, 20, 1, WHITE);
                
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
                DrawTextEx(font, "Level Complete!", (Vector2){screenWidth / 2 - 70, screenHeight / 2}, 20, 1, YELLOW);
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Level: %d", level - 1), (Vector2){mazeOffsetX + mazePixelWidth - 100, 10}, 20, 1, WHITE);
                DrawTextEx(font, "Lives: ", (Vector2){mazeOffsetX + mazePixelWidth - 150, screenHeight - 40}, 20, 1, WHITE);
                
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
                ClearBackground(BLACK);
                DrawTextEx(font, "Game Over", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, 20, 1, RED);
                DrawTextEx(font, TextFormat("Final Score: %d", pacman.score), (Vector2){screenWidth / 2 - 70, screenHeight / 2}, 20, 1, WHITE);
                DrawTextEx(font, "Press R to Return to Menu", (Vector2){screenWidth / 2 - 90, screenHeight / 2 + 50}, 20, 1, WHITE);
                break;

            default:
                break;
        }

        // Apply the fade-to-black transition
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));

        EndDrawing();
    }

    // Unload resources
    UnloadGhostTextures(ghosts);
    UnloadSound(sfx_menu);
    UnloadSound(sfx_menu_nav);
    UnloadSound(sfx_ready);
    UnloadSound(sfx_pacman_move);
    UnloadSound(sfx_pacman_chomp);
    UnloadSound(sfx_pacman_death);
    UnloadSound(sfx_eat_fruit);
    UnloadSound(sfx_eat_ghost);
    UnloadSound(sfx_ghost_frightened);
    UnloadSound(sfx_intermission);
    UnloadSound(sfx_extra_life);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}