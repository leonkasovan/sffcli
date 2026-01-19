# Makefile for sffcli
# Traditional build system (preserved alongside CMake)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -Isrc
LDFLAGS = -lpng -lz

# Directories
SRC_DIR = src
LIB_DIR = src/libsff
BUILD_DIR = build
BIN_DIR = bin
LIB_OUTPUT_DIR = lib

# Source files
LIB_SOURCES = $(LIB_DIR)/sff.cpp
LIB_OBJECTS = $(BUILD_DIR)/sff.o
CLI_SOURCES = $(SRC_DIR)/main.cpp
CLI_OBJECTS = $(BUILD_DIR)/main.o

# Output files
STATIC_LIB = $(LIB_OUTPUT_DIR)/libsff.a
SHARED_LIB = $(LIB_OUTPUT_DIR)/libsff.so
CLI_EXECUTABLE = $(BIN_DIR)/sffcli

# Targets
.PHONY: all clean static shared cli install

all: static shared cli

# Create directories
$(BUILD_DIR) $(BIN_DIR) $(LIB_OUTPUT_DIR):
	mkdir -p $@

# Compile library object
$(BUILD_DIR)/sff.o: $(LIB_SOURCES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

# Static library
static: $(STATIC_LIB)

$(STATIC_LIB): $(LIB_OBJECTS) | $(LIB_OUTPUT_DIR)
	ar rcs $@ $^
	@echo "Built static library: $@"

# Shared library
shared: $(SHARED_LIB)

$(SHARED_LIB): $(LIB_OBJECTS) | $(LIB_OUTPUT_DIR)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)
	@echo "Built shared library: $@"

# Compile CLI object
$(BUILD_DIR)/main.o: $(CLI_SOURCES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# CLI executable
cli: $(CLI_EXECUTABLE)

$(CLI_EXECUTABLE): $(CLI_OBJECTS) $(STATIC_LIB) | $(BIN_DIR)
	$(CXX) -o $@ $(CLI_OBJECTS) $(STATIC_LIB) $(LDFLAGS)
	@echo "Built CLI executable: $@"

# Install (requires root/sudo)
install: all
	install -d $(DESTDIR)/usr/local/lib
	install -d $(DESTDIR)/usr/local/include/libsff
	install -d $(DESTDIR)/usr/local/bin
	install -m 644 $(STATIC_LIB) $(DESTDIR)/usr/local/lib/
	install -m 755 $(SHARED_LIB) $(DESTDIR)/usr/local/lib/
	install -m 644 $(LIB_DIR)/sff.hpp $(DESTDIR)/usr/local/include/libsff/
	install -m 755 $(CLI_EXECUTABLE) $(DESTDIR)/usr/local/bin/
	@echo "Installation complete"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_OUTPUT_DIR)
	@echo "Cleaned build artifacts"

# Help
help:
	@echo "SFF CLI Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build everything (static lib, shared lib, CLI)"
	@echo "  static   - Build static library only"
	@echo "  shared   - Build shared library only"
	@echo "  cli      - Build CLI executable only"
	@echo "  install  - Install to /usr/local (requires root)"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help message"
