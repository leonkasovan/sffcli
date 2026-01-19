#ifndef LIBSFF_SFF_HPP
#define LIBSFF_SFF_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace sff {

// SFF version
enum class Version {
    V1,  // SFF v1.0
    V2   // SFF v2.0
};

// RGB color structure
struct Color {
    uint8_t r, g, b, a;
};

// Palette structure (256 colors)
struct Palette {
    std::vector<Color> colors;
    
    Palette() : colors(256) {}
};

// Sprite metadata
struct SpriteInfo {
    uint32_t group;
    uint32_t index;
    int16_t x;         // axis X
    int16_t y;         // axis Y
    uint32_t width;
    uint32_t height;
    uint32_t palette_index;
};

// Sprite data
struct Sprite {
    SpriteInfo info;
    std::vector<uint8_t> pixel_data;
};

// SFF File structure
class SFFFile {
public:
    SFFFile();
    ~SFFFile();
    
    // Load SFF file from path
    bool load(const std::string& filename);
    
    // Get version
    Version getVersion() const;
    
    // Get number of sprites
    size_t getSpriteCount() const;
    
    // Get number of palettes
    size_t getPaletteCount() const;
    
    // Get sprite by index
    const Sprite* getSprite(size_t index) const;
    
    // Get palette by index
    const Palette* getPalette(size_t index) const;
    
    // Extract sprite to PNG file
    bool extractSprite(size_t index, const std::string& output_path) const;
    
    // Extract all sprites to directory
    bool extractAllSprites(const std::string& output_dir) const;
    
    // Extract palette to file
    bool extractPalette(size_t index, const std::string& output_path) const;
    
    // Get last error message
    const std::string& getError() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Utility functions
std::string getVersionString(Version version);

} // namespace sff

#endif // LIBSFF_SFF_HPP
