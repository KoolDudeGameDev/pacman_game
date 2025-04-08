#include "raylib.h"
#include "resource_dir.h"
#include "game_logic.h"
#include "logo_animation.h"
#include "rendering.h"

int main(void) {
    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Pacman v1.0");
    SetTargetFPS(60);

    // Load a font
    Font font = GetFontDefault(); // Use default font for now

    // Calculate maze dimensions and offsets
    const int mazePixelWidth = MAZE_WIDTH * TILE_SIZE;              // 560px
    const int mazePixelHeight = MAZE_HEIGHT * TILE_SIZE;            // 620px
    const int mazeOffsetX = (screenWidth - mazePixelWidth) / 2;     // 360px (180px on each side)
    const int mazeOffsetY = (screenHeight - mazePixelHeight) / 2;   // 50px (top and bottom)

    // Menu variables
    int selectedOption = 0; // 0 = Easy, 1 = Medium, 2 = Hard

    // Logo animation state
    LogoAnimation logoAnim;
    init_kool_dude_logo(&logoAnim, screenWidth, screenHeight); // First state

    // Fade to black transition variables
    float transitionAlpha = 0.0f;
    bool fadingOut = false;
    GameState nextState = STATE_KOOLDUDE_LOGO;

    // Game Loop
    // ----------------------------------------------------------------------------------------
    while (!WindowShouldClose()) {

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
                    gameState = STATE_DEV_LOGO;
                    init_dev_logo(&logoAnim);
                }
                break;
            
            case STATE_DEV_LOGO:
                if (update_dev_logo(&logoAnim)) {
                    gameState = STATE_RAYLIB_LOGO;
                    init_raylib_logo(&logoAnim, screenWidth, screenHeight);
                }
                break;

            case STATE_RAYLIB_LOGO:
                if (update_raylib_logo(&logoAnim)) {
                    gameState = STATE_LOGO;
                    init_game_logo(&logoAnim);
                }
                break;

            case STATE_LOGO:
                if (update_game_logo(&logoAnim)) {
                    gameState = STATE_MENU;
                }
                break;

            case STATE_MENU:
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedOption = (selectedOption + 1) % 3;
                }
                if (IsKeyPressed(KEY_UP)) {
                    selectedOption = (selectedOption - 1 + 3) % 3;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    selectedDifficulty = (Difficulty)selectedOption;
                    init_maze();
                    init_pacman();
                    gameState = STATE_PLAYING;
                }
                break;

            case STATE_PLAYING:
                update_pacman();
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
                DrawTextEx(font, "Select Difficulty", (Vector2){screenWidth / 2 - 70, screenHeight / 2 - 60}, 20, 1, WHITE);
                DrawTextEx(font, "Easy", (Vector2){screenWidth / 2 - 20, screenHeight / 2 - 20}, 20, 1, selectedOption == 0 ? YELLOW : WHITE);
                DrawTextEx(font, "Medium", (Vector2){screenWidth / 2 - 20, screenHeight / 2}, 20, 1, selectedOption == 1 ? YELLOW : WHITE);
                DrawTextEx(font, "Hard", (Vector2){screenWidth / 2 - 20, screenHeight / 2 + 20}, 20, 1, selectedOption == 2 ? YELLOW : WHITE);
                DrawTextEx(font, "Use UP/DOWN to select, ENTER to start", (Vector2){screenWidth / 2 - 120, screenHeight / 2 + 60}, 15, 1, GRAY);
                break;

            case STATE_PLAYING:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, TextFormat("Score: %d", pacman.score), (Vector2){mazeOffsetX + 10, 10}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("Lives: %d", pacman.lives), (Vector2){mazeOffsetX + mazePixelWidth - 100, screenHeight - 30}, 20, 1, WHITE);
                break;
            
            case STATE_PAUSED:
                ClearBackground(BLACK);
                render_maze(mazeOffsetX, mazeOffsetY);
                render_pacman(mazeOffsetX, mazeOffsetY);
                DrawTextEx(font, "Paused", (Vector2){screenWidth / 2 - 30, screenHeight / 2}, 20, 1, WHITE);
                DrawTextEx(font, "Press P to Resume", (Vector2){screenWidth / 2 - 70, screenHeight / 2 + 30}, 20, 1, WHITE);
                break;

            case STATE_GAME_OVER:
                ClearBackground(BLACK);
                DrawTextEx(font, "Game Over", (Vector2){screenWidth / 2 - 50, screenHeight / 2 - 40}, 20, 1, RED);
                DrawTextEx(font, "Press R to Return to Menu", (Vector2){screenWidth / 2 - 90, screenHeight / 2}, 20, 1, WHITE);
                break;

            default:
                break;
        }
        // Apply fade to black transition
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));

        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}