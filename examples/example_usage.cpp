#include <libsff/sff.hpp>
#include <iostream>

/**
 * Example: How to use libsff to work with SFF files
 * 
 * This example demonstrates:
 * - Loading an SFF file
 * - Getting file information
 * - Extracting sprites
 * - Extracting palettes
 */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <sff_file>\n";
        return 1;
    }
    
    const char* sff_filename = argv[1];
    
    // Create SFF file object
    sff::SFFFile sff;
    
    // Load the file
    std::cout << "Loading SFF file: " << sff_filename << "\n";
    if (!sff.load(sff_filename)) {
        std::cerr << "Error loading file: " << sff.getError() << "\n";
        return 1;
    }
    
    // Display file information
    std::cout << "\n=== SFF File Information ===\n";
    std::cout << "Version: " << sff::getVersionString(sff.getVersion()) << "\n";
    std::cout << "Sprites: " << sff.getSpriteCount() << "\n";
    std::cout << "Palettes: " << sff.getPaletteCount() << "\n";
    
    // List all sprites
    std::cout << "\n=== Sprite List ===\n";
    for (size_t i = 0; i < sff.getSpriteCount() && i < 10; i++) {
        const sff::Sprite* sprite = sff.getSprite(i);
        if (sprite) {
            std::cout << "[" << i << "] "
                     << "Group " << sprite->info.group << ", "
                     << "Index " << sprite->info.index << ", "
                     << "Size " << sprite->info.width << "x" << sprite->info.height << ", "
                     << "Axis (" << sprite->info.x << ", " << sprite->info.y << ")\n";
        }
    }
    
    if (sff.getSpriteCount() > 10) {
        std::cout << "... and " << (sff.getSpriteCount() - 10) << " more sprites\n";
    }
    
    // Extract first sprite
    if (sff.getSpriteCount() > 0) {
        std::cout << "\n=== Extracting First Sprite ===\n";
        if (sff.extractSprite(0, "example_sprite_0.png")) {
            std::cout << "Successfully extracted sprite 0 to example_sprite_0.png\n";
        } else {
            std::cerr << "Failed to extract sprite 0\n";
        }
    }
    
    // Extract first palette
    if (sff.getPaletteCount() > 0) {
        std::cout << "\n=== Extracting First Palette ===\n";
        if (sff.extractPalette(0, "example_palette_0.pal")) {
            std::cout << "Successfully extracted palette 0 to example_palette_0.pal\n";
        } else {
            std::cerr << "Failed to extract palette 0\n";
        }
    }
    
    // Extract all sprites to a directory
    std::cout << "\n=== Extracting All Sprites ===\n";
    if (sff.extractAllSprites("example_output")) {
        std::cout << "Successfully extracted all sprites to example_output/\n";
    } else {
        std::cerr << "Failed to extract all sprites: " << sff.getError() << "\n";
    }
    
    return 0;
}
