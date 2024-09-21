# Exodian Particle Simulator

<img align="right" width="550" src="https://github.com/000x999/000x999_gifs/blob/main/GHub_3_1.gif">


A 2D particle simulator written in C++ using Raylib. <br>The simulator features realistic particle physics, including collision handling and spatial optimization, An interactive GUI and multiple particle customization options.
<br><br><br>
**Features:**
- Verlet integration for smooth particle motion.
- Particle to particle collision detection using a spring-damping model.
- Spatial hashing to optimize collision detection.
- Mouse interaction for manipulating particles.
- Customizable particle appearance and colors.
- Multiple view modes including wireframe and collision vector viewing.
- Interactive GUI, manual particle customization and spawning.
- Semi realistic fluid simulation using particle collision vector rendering.
<br><br><br>
## How It Works:
- [A Comprehensive and in depth analysis of my particle simulator is available in a PDF format written in LaTeX.](https://github.com/000x999/Exodian_Particle_Sim/blob/main/Particle_Simulator_Analysis_000x999.pdf).


## Requirements
- C++11 or later
- [Raylib](https://www.raylib.com/)

## Installation
**Note:**
- The particle simulator can be built and ran entirely in Visual Studio, the only requirement is having Raylib installed.
- Build instructions for anyone who doesn't use Visual studio can be found below including installing Raylib using vcpkg, MSYS2, Homebrew, apt as well as other build instructions and compiler flags.

### Option 1: Install Raylib using **vcpkg**
1. Install vcpkg by cloning the repository:
    ```bash
    git clone https://github.com/microsoft/vcpkg
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ```

2. Install Raylib using vcpkg:
    ```bash
    ./vcpkg install raylib
    ```

3. Integrate vcpkg with your development environment (CMake, Visual Studio, etc.):
    ```bash
    ./vcpkg integrate install
    ```

4. When building the project using CMake, specify the toolchain file:
    ```bash
    cmake -Bbuild -S. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
    cmake --build build
    ```

### Option 2: Install Raylib via Package Manager

#### On Windows (using MSYS2)
1. Install MSYS2 from [here](https://www.msys2.org/).
2. Open MSYS2 and run the following commands to install Raylib:
    ```bash
    pacman -Syu
    pacman -S mingw-w64-x86_64-raylib
    ```

#### On macOS (using Homebrew)
1. Install Homebrew if not already installed:
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```
2. Install Raylib using Homebrew:
    ```bash
    brew install raylib
    ```

#### On Linux (using apt)
1. Install Raylib via the package manager:
    ```bash
    sudo apt-get install libraylib-dev
    ```

### Option 3: Manual Installation of Raylib
Follow the [official guide](https://github.com/raysan5/raylib) to manually build and install Raylib from source.

### Step 3: Clone the Repository and Build
1. Clone the repository:
    ```bash
    git clone https://github.com/000x999/EXODIAN_PARTICLE_SIM
    cd EXODIAN_PARTICLE_SIM
    ```
2. Compile and run the program:
    ```bash
    g++ -o EXODIAN_PARTICLE_SIM main.cpp Particle.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ./EXODIAN_PARTICLE_SIM
    ```

