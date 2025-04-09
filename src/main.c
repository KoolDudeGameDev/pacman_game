#include "raylib.h"
#include "resource_dir.h"
#include "game_logic.h"
#include "logo_animation.h"
#include "rendering.h"

int main(void) {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;

    Pacman pacman;
    Ghost ghosts[4];
    Maze maze;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman Game v1.0");
    SetTargetFPS(60);

    InitGame(&pacman, ghosts, &maze);

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
            ClearBackground(BLACK);
            DrawGame(&pacman, ghosts, &maze);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}