#include "sff.hpp"
#include <fstream>
#include <cstring>
#include <png.h>
#include <zlib.h>
#include <sys/stat.h>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#endif

namespace sff {

// SFF v1 header structure
struct SFFv1Header {
    char signature[12];  // "ElecbyteSpr\0"
    uint8_t version[4];  // 1, 0, 1, 0
    uint32_t group_count;
    uint32_t image_count;
    uint32_t subfile_header_offset;
    uint32_t subfile_header_size;
    uint8_t shared_palette;
    uint8_t reserved[3];
};

// SFF v1 subfile header
struct SFFv1SubfileHeader {
    uint32_t next_offset;
    uint32_t length;
    int16_t axis_x;
    int16_t axis_y;
    uint16_t group;
    uint16_t image;
    uint16_t index;
    uint8_t palette_index;
    uint8_t reserved;
};

// Implementation class
class SFFFile::Impl {
public:
    Version version;
    std::vector<Sprite> sprites;
    std::vector<Palette> palettes;
    std::string error;
    
    bool loadV1(std::ifstream& file);
    bool extractSpritePNG(const Sprite& sprite, const Palette& palette, const std::string& output_path) const;
    bool createDirectory(const std::string& path) const;
};

// Helper to create directory
bool SFFFile::Impl::createDirectory(const std::string& path) const {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    
    #ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
    #else
    return mkdir(path.c_str(), 0755) == 0;
    #endif
}

// Load SFF v1 format
bool SFFFile::Impl::loadV1(std::ifstream& file) {
    SFFv1Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (file.gcount() != sizeof(header)) {
        error = "Failed to read SFF header";
        return false;
    }
    
    // Verify signature
    if (std::strncmp(header.signature, "ElecbyteSpr", 11) != 0) {
        error = "Invalid SFF signature";
        return false;
    }
    
    version = Version::V1;
    
    // Read shared palette (if present)
    Palette shared_palette;
    if (header.shared_palette) {
        for (int i = 0; i < 256; i++) {
            uint8_t rgb[3];
            file.read(reinterpret_cast<char*>(rgb), 3);
            shared_palette.colors[i] = {rgb[0], rgb[1], rgb[2], 255};
        }
    }
    palettes.push_back(shared_palette);
    
    // Seek to subfile header
    file.seekg(header.subfile_header_offset);
    
    // Read sprites
    for (uint32_t i = 0; i < header.image_count; i++) {
        SFFv1SubfileHeader subheader;
        
        file.read(reinterpret_cast<char*>(&subheader), sizeof(subheader));
        
        if (file.gcount() != sizeof(subheader)) {
            error = "Failed to read subfile header";
            return false;
        }
        
        Sprite sprite;
        sprite.info.group = subheader.group;
        sprite.info.index = subheader.image;
        sprite.info.x = subheader.axis_x;
        sprite.info.y = subheader.axis_y;
        sprite.info.palette_index = subheader.palette_index;
        
        // Read sprite data
        if (subheader.length > 0) {
            std::vector<uint8_t> compressed_data(subheader.length);
            file.read(reinterpret_cast<char*>(compressed_data.data()), subheader.length);
            
            // First 4 bytes are width and height
            if (compressed_data.size() >= 4) {
                sprite.info.width = compressed_data[0] | (compressed_data[1] << 8);
                sprite.info.height = compressed_data[2] | (compressed_data[3] << 8);
                
                // Decompress RLE data (simplified - actual SFF uses custom RLE)
                // For now, we'll use a basic implementation
                size_t pixel_count = sprite.info.width * sprite.info.height;
                sprite.pixel_data.resize(pixel_count, 0);
                
                // Copy pixel data (skipping first 4 bytes)
                size_t data_offset = 4;
                size_t pixel_offset = 0;
                
                while (data_offset < compressed_data.size() && pixel_offset < pixel_count) {
                    uint8_t byte = compressed_data[data_offset++];
                    
                    if (byte & 0x80) {
                        // RLE: repeat next byte (byte & 0x7F) times
                        uint8_t count = byte & 0x7F;
                        if (data_offset < compressed_data.size()) {
                            uint8_t value = compressed_data[data_offset++];
                            for (uint8_t j = 0; j < count && pixel_offset < pixel_count; j++) {
                                sprite.pixel_data[pixel_offset++] = value;
                            }
                        }
                    } else {
                        // Copy byte as-is
                        sprite.pixel_data[pixel_offset++] = byte;
                    }
                }
            }
        }
        
        sprites.push_back(sprite);
        
        // Move to next subfile
        if (subheader.next_offset != 0) {
            file.seekg(subheader.next_offset);
        }
    }
    
    return true;
}

// Extract sprite to PNG
bool SFFFile::Impl::extractSpritePNG(const Sprite& sprite, const Palette& palette, const std::string& output_path) const {
    if (sprite.info.width == 0 || sprite.info.height == 0) {
        return false;
    }
    
    FILE* fp = fopen(output_path.c_str(), "wb");
    if (!fp) {
        return false;
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return false;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return false;
    }
    
    png_init_io(png_ptr, fp);
    
    // Set image info
    png_set_IHDR(png_ptr, info_ptr, sprite.info.width, sprite.info.height,
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_write_info(png_ptr, info_ptr);
    
    // Write image data
    std::vector<png_byte> row(sprite.info.width * 4);
    for (uint32_t y = 0; y < sprite.info.height; y++) {
        for (uint32_t x = 0; x < sprite.info.width; x++) {
            uint8_t index = sprite.pixel_data[y * sprite.info.width + x];
            const Color& color = palette.colors[index];
            
            row[x * 4 + 0] = color.r;
            row[x * 4 + 1] = color.g;
            row[x * 4 + 2] = color.b;
            row[x * 4 + 3] = (index == 0) ? 0 : color.a;  // Index 0 is transparent
        }
        png_write_row(png_ptr, row.data());
    }
    
    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    
    return true;
}

// SFFFile implementation
SFFFile::SFFFile() : pImpl(new Impl()) {}

SFFFile::~SFFFile() = default;

bool SFFFile::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        pImpl->error = "Failed to open file: " + filename;
        return false;
    }
    
    // Try to load as v1
    return pImpl->loadV1(file);
}

Version SFFFile::getVersion() const {
    return pImpl->version;
}

size_t SFFFile::getSpriteCount() const {
    return pImpl->sprites.size();
}

size_t SFFFile::getPaletteCount() const {
    return pImpl->palettes.size();
}

const Sprite* SFFFile::getSprite(size_t index) const {
    if (index >= pImpl->sprites.size()) {
        return nullptr;
    }
    return &pImpl->sprites[index];
}

const Palette* SFFFile::getPalette(size_t index) const {
    if (index >= pImpl->palettes.size()) {
        return nullptr;
    }
    return &pImpl->palettes[index];
}

bool SFFFile::extractSprite(size_t index, const std::string& output_path) const {
    if (index >= pImpl->sprites.size()) {
        return false;
    }
    
    const Sprite& sprite = pImpl->sprites[index];
    size_t palette_idx = std::min<size_t>(sprite.info.palette_index, pImpl->palettes.size() - 1);
    const Palette& palette = pImpl->palettes[palette_idx];
    
    return pImpl->extractSpritePNG(sprite, palette, output_path);
}

bool SFFFile::extractAllSprites(const std::string& output_dir) const {
    if (!pImpl->createDirectory(output_dir)) {
        pImpl->error = "Failed to create output directory";
        return false;
    }
    
    for (size_t i = 0; i < pImpl->sprites.size(); i++) {
        const Sprite& sprite = pImpl->sprites[i];
        std::string filename = output_dir + "/sprite_" + 
                              std::to_string(sprite.info.group) + "_" + 
                              std::to_string(sprite.info.index) + ".png";
        
        if (!extractSprite(i, filename)) {
            return false;
        }
    }
    
    return true;
}

bool SFFFile::extractPalette(size_t index, const std::string& output_path) const {
    if (index >= pImpl->palettes.size()) {
        return false;
    }
    
    const Palette& palette = pImpl->palettes[index];
    
    std::ofstream file(output_path);
    if (!file) {
        return false;
    }
    
    file << "JASC-PAL\n0100\n256\n";
    for (const auto& color : palette.colors) {
        file << static_cast<int>(color.r) << " "
             << static_cast<int>(color.g) << " "
             << static_cast<int>(color.b) << "\n";
    }
    
    return file.good();
}

const std::string& SFFFile::getError() const {
    return pImpl->error;
}

std::string getVersionString(Version version) {
    switch (version) {
        case Version::V1: return "1.0";
        case Version::V2: return "2.0";
        default: return "Unknown";
    }
}

} // namespace sff
