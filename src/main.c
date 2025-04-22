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
    InitAudioDevice();
    SetTargetFPS(60);

    // Load a font
    Font font = GetFontDefault();

    // Calculate maze dimensions and offsets
    const int mazePixelWidth = MAZE_WIDTH * TILE_SIZE;              // 560px
    const int mazePixelHeight = MAZE_HEIGHT * TILE_SIZE;            // 620px
    const int mazeOffsetX = (screenWidth - mazePixelWidth) / 2;     // 360px (180px on each side)
    const int mazeOffsetY = (screenHeight - mazePixelHeight) / 2;   // 50px (top and bottom)

    // Menu variables
    int selectedOption = 0;
    bool shouldExit = false;
    
    // Logo animation state
    LogoAnimation logoAnim;
    init_kool_dude_logo(&logoAnim, screenWidth, screenHeight);      // First state

    // Load ghost textures
    LoadGhostTextures(ghosts);

    // Fade to black transition variables
    float transitionAlpha = 0.0f;
    bool fadingOut = false;
    GameState nextState = STATE_KOOLDUDE_LOGO;

    // Game Loop
    // ----------------------------------------------------------------------------------------
    while (!(shouldExit || WindowShouldClose())) {
        // Fade to black transition
        if (fadingOut) {
            transitionAlpha += 0.05f;
            if (transitionAlpha >= 1.0f) {
                transitionAlpha = 1.0f;
                fadingOut = false;
                gameState = nextState;
                transitionAlpha = 1.0f;
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
                // Navigate menu options
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedOption = (selectedOption + 1) % 2;
                }
                if (IsKeyPressed(KEY_UP)) {
                    selectedOption = (selectedOption - 1 + 2) % 2;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedOption == 0) {
                        init_maze();
                        int startX, startY;
                        find_pacman_start(&startX, &startY);
                        init_pacman(startX, startY);
                        init_ghosts();
                        readyTimer = 3.0f;      // Show "READY!" for 3 seconds
                        gameState = STATE_READY;
                        isResetting = false;
                    } else {
                        CloseWindow();
                        return 0;
                    }
                }
                break;
            
            case STATE_READY:
                readyTimer -= GetFrameTime();
                if (readyTimer <= 0.0f) {
                    gameState = STATE_PLAYING;
                    readyTimer = 0.0f;
                }
                break;

            case STATE_PLAYING:
                DrawFPS(10,10);
                update_pacman();
                update_ghosts();
                if (IsKeyPressed(KEY_P)) {
                    gameState = STATE_PAUSED;
                }
                break;

            case STATE_PAUSED:
                if (IsKeyPressed(KEY_P)) {
                    gameState = STATE_PLAYING;
                }
                break;

            case STATE_GAME_OVER:
                if (IsKeyPressed(KEY_R)) {
                    gameState = STATE_MENU;
                    selectedOption = 0;
                    // Reset score and lives for a new game
                    pacman.score = 0;
                    pacman.lives = 3;
                    init_maze();    // Reset maze with pellets
                }
                break;
        
            default:
                break;
        }

        // Start Rendering
        // ----------------------------------------------------------------------------------------
        BeginDrawing();

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
                DrawTextEx(font, "Pac-Man", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 60}, 30, 1, YELLOW);
                DrawTextEx(font, "Start", (Vector2){screenWidth / 2 - 20, screenHeight / 2 - 20}, 20, 1, selectedOption == 0 ? YELLOW : WHITE);
                DrawTextEx(font, "Exit", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 10}, 20, 1, selectedOption == 1 ? YELLOW : WHITE);
                DrawTextEx(font, "Use UP/DOWN to select, ENTER to confirm", (Vector2){screenWidth / 2 - 120, screenHeight / 2 + 50}, 15, 1, GRAY);
                break;

            case STATE_READY:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "READY!", (Vector2){screenWidth / 2 - 30, mazeOffsetY + (14 * TILE_SIZE + 3)}, 18, 1, YELLOW);     // Position at row 14 (13 in 0-based index)
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Lives: %d", pacman.lives), (Vector2){mazeOffsetX + mazePixelWidth - 100, screenHeight - 30}, 20, 1, WHITE);

            case STATE_PLAYING:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_ghosts(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Lives: %d", pacman.lives), (Vector2){mazeOffsetX + mazePixelWidth - 100, screenHeight - 30}, 20, 1, WHITE);
                break;
            
            case STATE_PAUSED:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Paused", (Vector2){screenWidth / 2 - 30, screenHeight / 2}, 20, 1, WHITE);
                DrawTextEx(font, "Press P to Resume", (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 30}, 20, 1, WHITE);
                break;

            case STATE_GAME_OVER:
                ClearBackground(BLACK);
                DrawTextEx(font, "Game Over", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, 20, 1, RED);
                DrawTextEx(font, TextFormat("Final Score: %d", pacman.score), (Vector2){screenWidth / 2 - 70, screenHeight / 2}, 20, 1, WHITE);
                DrawTextEx(font, "Press R to Return to Menu", (Vector2){screenWidth / 2 - 90, screenHeight / 2}, 20, 1, WHITE);
                break;

            default:
                break;
        }

        // Apply the fade-to-black transition
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));

        EndDrawing();
    }

    UnloadGhostTextures(ghosts);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}