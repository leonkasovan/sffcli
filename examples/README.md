# libsff Examples

This directory contains example code showing how to use the libsff library.

## Example Programs

### example_usage.cpp

Demonstrates basic library usage:
- Loading an SFF file
- Getting file information
- Listing sprites
- Extracting individual sprites
- Extracting palettes
- Extracting all sprites to a directory

## Building the Examples

### Using CMake

Add the following to your CMakeLists.txt:

```cmake
add_executable(example_usage examples/example_usage.cpp)
target_link_libraries(example_usage PRIVATE sff)
```

Then build normally:
```bash
cd build
cmake ..
cmake --build .
./example_usage /path/to/file.sff
```

### Using g++ directly

```bash
# With static library
g++ -std=c++11 -Isrc -o example_usage examples/example_usage.cpp lib/libsff.a -lpng -lz

# With shared library
g++ -std=c++11 -Isrc -o example_usage examples/example_usage.cpp -Llib -lsff -lpng -lz

# Run
./example_usage /path/to/file.sff
```

## Sample SFF Files

You can find SFF files in various MUGEN fighter packages available online. Common sources include:
- MUGEN character downloads from community sites
- Game sprite rips
- Open source MUGEN projects

Note: Ensure you have permission to use any SFF files you download.
