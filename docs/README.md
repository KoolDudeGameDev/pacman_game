# Pac-Man v1.0

A classic Pac-Man game built with Raylib, featuring a 28x31 tile maze, ghost AI, pellet collection, and retro CRT-style visuals with pulsating maze glow and vignette effects.

## Features
- Authentic Pac-Man gameplay with a 28x31 maze.
- Four ghosts (Blinky, Pinky, Inky, Clyde) with unique AI:
  - Blinky: Direct chase.
  - Pinky: Ambush targeting 4 tiles ahead.
  - Inky: Complex targeting using Blinky's position.
  - Clyde: Switches between chase and scatter based on distance.
- Chase/Scatter modes with timed transitions.
- Pellets and power pellets enabling temporary ghost-eating.
- Retro visuals: Pulsating maze glow (light blue, 2Hz) and CRT vignette/noise.
- Menu with Start/Exit, pause functionality, and score tracking.
- Logo animations for intro sequence.

## Installation
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd pacman_game
   ```
   If Git is not installed, download it from [Git for Windows](https://gitforwindows.org/).

2. Install dependencies (see [Dependencies](#dependencies)).
3. Ensure a GCC compiler environment (e.g., MinGW-w64).

## Usage
1. Build the project (see [Building the Project](#building-the-project)).
2. Run the game:
   ```bash
   ./pacman.exe
   ```
3. Use UP/DOWN keys to navigate the menu and ENTER to select.
4. Play using the controls below.

## Controls
- **UP/DOWN/LEFT/RIGHT**: Move Pac-Man.
- **P**: Pause/Unpause.
- **R**: Return to menu from Game Over.
- **ENTER**: Select menu options (Start/Exit).
- **F11**: Toggle fullscreen.

## Project Structure
```
pacman_game/
├── assets/                  # Sounds and sprites
│   ├── sounds/             # Audio files
│   └── sprites/            # Sprite sheets (e.g., pacman_general_sprites.png)
├── include/                # Header files
│   ├── all_libs.h
│   ├── game_logic.h
│   ├── logo_animation.h
│   ├── rendering.h
│   ├── resource_dir.h
│   └── utils.h
├── src/                    # Source code
│   ├── game_logic/
│   │   ├── game.c         # Game state and maze setup
│   │   ├── ghost_ai.c     # Ghost AI and movement
│   │   └── pacman_movement.c # Pac-Man movement and pellets
│   ├── logo_animation.c   # Logo animations
│   ├── main.c             # Main loop and rendering
│   ├── rendering.c        # Maze, Pac-Man, and ghost rendering
│   └── utils.c            # Utility functions
├── docs/                  # Documentation
│   └── README.md
├── .gitignore             # Git ignore file
└── Makefile               # Build script
```

## Dependencies
- **Raylib**: Game development library.
  - Download from [Raylib releases](https://github.com/raysan5/raylib/releases) (e.g., `raylib-X.X.X_win64_mingw-w64.zip`).
  - Extract to `C:\raylib` and add `C:\raylib\lib` to PATH or copy `libraylib.a` to project.
  - Ensure `C:\raylib\include` is accessible for headers.
- **MinGW-w64**: GCC compiler for Windows.
  - Download from [MinGW-w64 SourceForge](https://sourceforge.net/projects/mingw-w64/).
  - Add `C:\msys64\mingw64\bin` to PATH.

## Building the Project
1. Navigate to project directory:
   ```bash
   cd path/to/pacman_game
   ```
2. Build using Makefile:
   ```bash
   make
   ```
3. Run the game:
   ```bash
   ./pacman.exe
   ```

## Contributing
Fork the repository, create a branch, and submit a pull request with changes. Follow existing code style and add comments for clarity.

## License
Copyright (c) 2025 [Kyle Ibo]

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.