# sffcli
SFF Command Line Tool - Extract sprites and palettes from SFF (Sprite Flinch Format) files

## Overview

This project provides a C++ library (`libsff`) and command-line tool (`sffcli`) for working with SFF sprite files commonly used in fighting games like MUGEN.

## Features

- **Reusable C++ Library**: Core SFF parsing and extraction logic in `libsff`
- **CLI Tool**: Thin wrapper for command-line operations
- **Multiple Build Systems**: Both CMake and traditional Makefile
- **Library Formats**: Build static and shared library variants
- **Sprite Extraction**: Extract individual or all sprites to PNG format
- **Palette Extraction**: Export palette data in JASC-PAL format

## Building

### Prerequisites

- C++11 compatible compiler (GCC, Clang, MSVC)
- libpng development files
- zlib development files
- CMake 3.10+ (for CMake build) or GNU Make (for Makefile build)

### CMake Build (Recommended)

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Install (optional)
sudo cmake --install .
```

This builds:
- `libsff.a` - Static library
- `libsff.so` - Shared library
- `sffcli` - CLI executable

### Makefile Build

```bash
# Build everything
make all

# Or build specific targets
make static     # Static library only
make shared     # Shared library only
make cli        # CLI executable only

# Install to /usr/local
sudo make install

# Clean build artifacts
make clean
```

## Usage

### Command-Line Tool

```bash
# Display file information
sffcli -i fighter.sff

# Extract all sprites to a directory
sffcli -e output_dir fighter.sff

# Extract a specific sprite
sffcli -s 0 -o sprite.png fighter.sff

# Extract a palette
sffcli -p 0 -o palette.pal fighter.sff
```

### Library API

```cpp
#include <libsff/sff.hpp>

// Load SFF file
sff::SFFFile sff;
if (!sff.load("fighter.sff")) {
    std::cerr << "Error: " << sff.getError() << std::endl;
    return 1;
}

// Get information
std::cout << "Sprites: " << sff.getSpriteCount() << std::endl;
std::cout << "Palettes: " << sff.getPaletteCount() << std::endl;

// Extract sprite
sff.extractSprite(0, "sprite_0.png");

// Extract all sprites
sff.extractAllSprites("output_directory");
```

## Project Structure

```
sffcli/
├── src/
│   ├── libsff/
│   │   ├── sff.hpp          # Library header
│   │   └── sff.cpp          # Library implementation
│   └── main.cpp             # CLI wrapper
├── CMakeLists.txt           # CMake build configuration
├── Makefile                 # Traditional Makefile
├── libsff.pc.in             # pkg-config template
└── README.md                # This file
```

## License

MIT License - See repository for details
