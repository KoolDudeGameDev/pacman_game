#include "raylib.h"
#include "resource_dir.h"
#include "game_logic.h"
#include "logo_animation.h"

int main(void) {
    // Initialization
    const int screenWidth = MAZE_WIDTH * TILE_SIZE;
    const int screenHeight = MAZE_HEIGHT * TILE_SIZE;
    InitWindow(screenWidth, screenHeight, "Pacman v1.0");
    SetTargetFPS(60);

    // Menu variables
    int selectedOption = 0; // 0 = Easy, 1 = Medium, 2 = Hard

    // Logo animation state
    LogoAnimation logoAnim;
    init_kool_dude_logo(&logoAnim, screenWidth, screenHeight); // First state

    // Game Loop
    // ----------------------------------------------------------------------------------------

    while (!WindowShouldClose()) {
        switch (gameState) {
            case STATE_KOOLDUDE_LOGO:
                if (update_kool_dude_logo(&logoAnim)) {
                    gameState = STATE_DEV_LOGO;
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
                    selectedOption - (selectedOption - 1 + 3) % 3;
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
    }

    // Start Rendering
    // ----------------------------------------------------------------------------------------

    BeginDrawing();

    switch (gameState) {
        case STATE_KOOLDUDE_LOGO:
            render_kool_dude_logo(&logoAnim);
            break;

        case STATE_DEV_LOGO:
            render_dev_logo(&logoAnim, screenWidth, screenHeight);
            break;

        case STATE_RAYLIB_LOGO:
            render_raylib_logo(&logoAnim);
            break;

        case STATE_LOGO:
            render_game_logo(&logoAnim, screenWidth, screenHeight);

        case STATE_MENU:
            ClearBackground(BLACK);
            DrawText("Select Dfficulty", screenWidth / 2 - 70, screenHeight / 2 - 60, 20, WHITE);
            DrawText("Easy", screenWidth / 2 - 20, screenHeight / 2 - 20, 20, selectedOption == 0 ? YELLOW : WHITE);
            DrawText("Medium", screenWidth / 2 - 20, screenHeight / 2, 20, selectedOption == 1 ? YELLOW : WHITE);
            DrawText("Hard", screenWidth / 2 - 20, screenHeight / 2 + 20, 20, selectedOption == 2 ? YELLOW : WHITE);
            DrawText("Use UP/DOWN to select, ENTER to start", screenWidth / 2 - 120, screenHeight / 2 + 60, 15, GRAY);
            break;

        case STATE_PLAYING:
            ClearBackground(BLACK);
            render_maze();
            render_pacman();
            DrawText(TextFormat("Score: %d", pacman.score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", pacman.lives), screenWidth - 100, 10, 20, WHITE);
            break;
        
        default:
            break;
    }

    CloseWindow();
    return 0;
}