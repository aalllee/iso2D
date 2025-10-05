# Game Project

A C++ game built with SFML, ImGui, and ImGui-SFML.

## Prerequisites

### macOS

Install dependencies using Homebrew:
```bash
brew install cmake sfml@2 glfw
```

### Windows

1. Install [CMake](https://cmake.org/download/) (3.15 or higher)
2. Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ development tools
3. Install SFML 2.6:
   - Download from [SFML downloads](https://www.sfml-dev.org/download/sfml/2.6.2/)
   - Extract to a location (e.g., `C:\SFML-2.6.2`)
   - Add to PATH or use `-DSFML_DIR` flag when running CMake
4. Install GLFW:
   - Download from [GLFW downloads](https://www.glfw.org/download.html)
   - Extract and add to PATH or use `-Dglfw3_DIR` flag

### Linux

Install dependencies using your package manager:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install cmake build-essential libsfml-dev libglfw3-dev
```

**Fedora:**
```bash
sudo dnf install cmake gcc-c++ SFML-devel glfw-devel
```

**Arch Linux:**
```bash
sudo pacman -S cmake sfml glfw
```

## Building the Project

### Clone the Repository

```bash
git clone <your-repository-url>
cd game
```

### Build Instructions

#### macOS / Linux

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Or open the generated `.sln` file in Visual Studio and build from the IDE.

#### Windows (MinGW)

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### Running the Game

After building, the executable will be in the build directory:

**macOS/Linux:**
```bash
./build/game
```

**Windows:**
```bash
build\Release\game.exe
```

The `assets` folder will be automatically copied to the build directory.

## Project Structure

```
game/
├── assets/           # Game assets (textures, fonts, etc.)
├── external/         # Third-party libraries
│   ├── imgui/       # Dear ImGui
│   ├── imgui-sfml-2.6.1/  # ImGui-SFML binding
│   └── nlohmann/    # JSON library
├── include/         # Header files
├── src/             # Source files
│   ├── collision/   # Collision system
│   ├── editor/      # Editor components
│   └── events/      # Event system
├── CMakeLists.txt   # Build configuration
└── README.md
```

## Dependencies

The following libraries are included in the `external/` directory:
- **ImGui** - Immediate mode GUI library
- **ImGui-SFML** - SFML binding for ImGui
- **nlohmann/json** - JSON parsing library

External dependencies (must be installed separately):
- **SFML 2.6** - Graphics and windowing
- **GLFW** - Window and input handling
- **OpenGL** - Graphics rendering

## Troubleshooting

### SFML not found on Windows
Add SFML to CMake:
```bash
cmake -DSFML_DIR="C:/path/to/SFML/lib/cmake/SFML" ..
```

### GLFW not found
Add GLFW to CMake:
```bash
cmake -Dglfw3_DIR="C:/path/to/glfw/lib/cmake/glfw3" ..
```

### Assets not loading
Ensure you run the executable from the build directory, or that the `assets` folder is in the same directory as the executable.

## License

[Add your license here]
