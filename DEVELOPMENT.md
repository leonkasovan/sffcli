# Development Guide

## Building from Source

### Prerequisites

Ensure you have the following dependencies installed:

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake libpng-dev zlib1g-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake libpng-devel zlib-devel
```

**macOS:**
```bash
brew install cmake libpng
```

### Build Options

#### Option 1: CMake (Recommended)

CMake provides the most flexible build configuration:

```bash
# Create build directory
mkdir build && cd build

# Configure with default options
cmake ..

# Or configure with specific options
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# Build
cmake --build .

# Install (optional, requires appropriate permissions)
sudo cmake --install .
```

**CMake Targets:**
- `sff` - Static library (libsff.a)
- `sff_shared` - Shared library (libsff.so)
- `sffcli` - Command-line tool

Build specific targets:
```bash
cmake --build . --target sff         # Static library only
cmake --build . --target sff_shared  # Shared library only
cmake --build . --target sffcli      # CLI tool only
```

#### Option 2: Makefile

Traditional Make-based build for simplicity:

```bash
# Build everything
make all

# Or build specific components
make static    # Static library only
make shared    # Shared library only
make cli       # CLI tool only

# Clean build artifacts
make clean

# Install to /usr/local (requires root)
sudo make install
```

**Make Targets:**
- `all` - Build everything
- `static` - Build static library
- `shared` - Build shared library
- `cli` - Build CLI executable
- `install` - Install to system
- `clean` - Remove build artifacts
- `help` - Show help message

### Build Artifacts

After building, you'll find:
- **Makefile build**: `lib/libsff.{a,so}`, `bin/sffcli`
- **CMake build**: `build/libsff.{a,so}`, `build/sffcli`

## Development Workflow

### Code Style

- C++11 standard
- Follow existing naming conventions
- Use meaningful variable names
- Add comments for complex logic

### Testing Changes

After making changes, rebuild and test:

```bash
# Makefile
make clean && make all

# CMake
cd build && cmake --build . && cd ..

# Test CLI
./bin/sffcli --help
./bin/sffcli --version
```

### Adding New Features

When adding features to the library:

1. Update `src/libsff/sff.hpp` with new public API
2. Implement in `src/libsff/sff.cpp`
3. Update `src/main.cpp` if CLI needs to expose it
4. Add examples to `examples/` directory
5. Update documentation in README.md

## Library Integration

### Using in Your Project

#### With CMake

```cmake
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

# Link against installed library
find_library(SFF_LIBRARY sff)
include_directories(/usr/local/include)
target_link_libraries(your_target ${SFF_LIBRARY} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})

# Or add as subdirectory
add_subdirectory(path/to/sffcli)
target_link_libraries(your_target sff)
```

#### With pkg-config

After installation, use pkg-config:

```bash
# Get compiler flags
pkg-config --cflags libsff

# Get linker flags
pkg-config --libs libsff

# Compile your program
g++ -std=c++11 $(pkg-config --cflags libsff) myprogram.cpp $(pkg-config --libs libsff) -o myprogram
```

#### Manual Compilation

```bash
# With static library
g++ -std=c++11 -I/usr/local/include myprogram.cpp -L/usr/local/lib -lsff -lpng -lz -o myprogram

# With shared library
g++ -std=c++11 -I/usr/local/include myprogram.cpp -L/usr/local/lib -lsff -lpng -lz -o myprogram
```

## Troubleshooting

### Common Build Issues

**Missing dependencies:**
```
error: png.h: No such file or directory
```
Solution: Install libpng-dev and zlib1g-dev

**CMake can't find PNG:**
```
Could NOT find PNG
```
Solution: Install libpng-dev or set PNG_PNG_INCLUDE_DIR and PNG_LIBRARY

**Linker errors:**
```
undefined reference to `png_create_write_struct'
```
Solution: Ensure -lpng -lz are included in linker flags

### Runtime Issues

**Shared library not found:**
```
error while loading shared libraries: libsff.so
```
Solution: Add library path to LD_LIBRARY_PATH or run ldconfig after installation

## Contributing

When contributing code:

1. Follow the existing code style
2. Ensure code compiles without warnings
3. Test both CMake and Makefile builds
4. Update documentation for new features
5. Add example code when appropriate

## Platform Support

- **Linux**: Full support (tested on Ubuntu 24.04)
- **macOS**: Full support (requires Homebrew dependencies)
- **Windows**: Partial support (requires MinGW or MSVC with proper library paths)

## License

See repository for license information.
