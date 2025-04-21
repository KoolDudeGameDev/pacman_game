# Pac-Man v1.0

A classic Pac-Man game implementation using the Raylib library. This project includes a fully functional Pac-Man game with maze rendering, ghost AI, pellet collection, and game state management.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building the Project](#building-the-project)
- [Contributing](#contributing)
- [License](#license)

## Features
- Classic Pac-Man gameplay with a 28x31 tile maze.
- Four ghosts (Blinky, Pinky, Inky, Clyde) with unique AI behaviors:
  - Blinky chases Pac-Man directly.
  - Pinky ambushes by targeting 4 tiles ahead of Pac-Man.
  - Inky uses a complex targeting system based on Blinky's position.
  - Clyde switches between chasing Pac-Man and scattering based on distance.
- Chase and Scatter modes for ghosts, with timed transitions.
- Pellets and power pellets that allow Pac-Man to eat ghosts temporarily.
- Logo animations and a menu system with Start/Exit options.
- Pause functionality and game-over state with score tracking.

## Installation
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd pacman_game
      If you don’t have Git installed, download it from Git for Windows and install it.
2. Install the required dependencies (see Dependencies for details).
3. Ensure you have a working environment for compiling with GCC and Make (e.g., using MinGW-w64).

## Usage
1. Build the project using the provided Makefile (see Building the Project).
2. Run the game:
   ```bash
    pacman.exe
3. Navigate the menu using the UP/DOWN keys and press ENTER to select an option.
4. Play the game using the controls listed below.

## Controls
UP/DOWN/LEFT/RIGHT: Move Pac-Man in the respective direction.
P: Pause/Unpause the game.
R: Return to the menu from the Game Over screen.
ENTER: Select menu options (Start/Exit).

## Project Structure
pacman_game/
│
├── assets/              # Sound and other asset files
│   └── sounds/
|
├── include/
|   ├── all_libs.h
│   ├── game_logic.h     # Core game logic definitions
│   ├── logo_animation.h
│   ├── rendering.h
|   ├── resource_dir.h
|   └── utils.h 
|
├── src/                 # Source code
│   ├── game_logic/
│   │   ├── game.c            # Game state and maze initialization
│   │   ├── ghost_ai.c        # Ghost AI and movement logic
│   │   └── pacman_movement.c # Pac-Man movement and pellet collection
│   │
│   ├── logo_animation.c # Logo animation logic
│   ├── main.c           # Main game loop and rendering
│   ├── rendering.c      # Rendering functions for maze, Pac-Man, and ghosts
│   └── utils.c          # Utility functions (e.g., collision detection)
|
├── docs/                # Documentation files
│   └── README.md
|
├── .gitignore           # Git ignore file
└── Makefile             # Build script

## Dependencies
Raylib: A simple and easy-to-use library for game development.
    Download the precompiled Raylib binaries for Windows from the Raylib releases page. Choose the latest release (e.g., raylib-X.X.X_win64_mingw-w64.zip for 64-bit systems).
    Extract the zip file to a directory, e.g., C:\raylib.
    Add the Raylib lib/ directory (e.g., C:\raylib\lib) to your system’s PATH environment variable, or copy libraylib.a to your project directory for linking.
    Ensure the Raylib include/ directory (e.g., C:\raylib\include) is accessible for including headers (you may need to specify this path in your Makefile).

MinGW-w64 (GCC for Windows): Provides the GCC compiler for compiling C code on Windows.
    Download the MinGW-w64 installer from MinGW-w64 SourceForge
    Add the MinGW-w64 bin/ directory (e.g., C:\msys64\mingw64\bin) to your system’s PATH environment variable.


## Building the Project
1. Ensure all dependencies are installed.
2. Navigate to the project directory:
  ```bash
  cd path\to\pacman_game
3. Build the project:
  ```bash
  make
4. Run the game:
  ```bash
  pacman.exe

## Contributing
Contributions are welcome! Please fork the repository, create a new branch, and submit a pull request with your changes. Ensure your code follows the existing style and includes appropriate comments.

## License
Copyright (c) 2025 [nameeeee]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
