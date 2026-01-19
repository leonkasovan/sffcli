#include "libsff/sff.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

// Print usage information
void printUsage(const char* program_name) {
    std::cout << "SFF Command Line Tool\n\n";
    std::cout << "Usage: " << program_name << " [options] <sff_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -i, --info              Display SFF file information\n";
    std::cout << "  -e, --extract <dir>     Extract all sprites to directory\n";
    std::cout << "  -s, --sprite <index>    Extract specific sprite by index\n";
    std::cout << "  -o, --output <path>     Output path for single sprite extraction\n";
    std::cout << "  -p, --palette <index>   Extract palette by index\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " -i fighter.sff\n";
    std::cout << "  " << program_name << " -e output fighter.sff\n";
    std::cout << "  " << program_name << " -s 0 -o sprite.png fighter.sff\n";
    std::cout << "  " << program_name << " -p 0 -o palette.pal fighter.sff\n";
}

// Print version information
void printVersion() {
    std::cout << "sffcli version 1.0.0\n";
    std::cout << "SFF sprite file extractor\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    const char* sff_file = nullptr;
    const char* extract_dir = nullptr;
    const char* output_path = nullptr;
    int sprite_index = -1;
    int palette_index = -1;
    bool show_info = false;
    
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        else if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--version") == 0) {
            printVersion();
            return 0;
        }
        else if (std::strcmp(argv[i], "-i") == 0 || std::strcmp(argv[i], "--info") == 0) {
            show_info = true;
        }
        else if (std::strcmp(argv[i], "-e") == 0 || std::strcmp(argv[i], "--extract") == 0) {
            if (i + 1 < argc) {
                extract_dir = argv[++i];
            } else {
                std::cerr << "Error: -e/--extract requires a directory argument\n";
                return 1;
            }
        }
        else if (std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--sprite") == 0) {
            if (i + 1 < argc) {
                sprite_index = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: -s/--sprite requires an index argument\n";
                return 1;
            }
        }
        else if (std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_path = argv[++i];
            } else {
                std::cerr << "Error: -o/--output requires a path argument\n";
                return 1;
            }
        }
        else if (std::strcmp(argv[i], "-p") == 0 || std::strcmp(argv[i], "--palette") == 0) {
            if (i + 1 < argc) {
                palette_index = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: -p/--palette requires an index argument\n";
                return 1;
            }
        }
        else if (argv[i][0] != '-') {
            sff_file = argv[i];
        }
        else {
            std::cerr << "Error: Unknown option: " << argv[i] << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (!sff_file) {
        std::cerr << "Error: No SFF file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Load SFF file
    sff::SFFFile sff;
    if (!sff.load(sff_file)) {
        std::cerr << "Error loading SFF file: " << sff.getError() << "\n";
        return 1;
    }
    
    // Show information
    if (show_info) {
        std::cout << "SFF File: " << sff_file << "\n";
        std::cout << "Version: " << sff::getVersionString(sff.getVersion()) << "\n";
        std::cout << "Sprites: " << sff.getSpriteCount() << "\n";
        std::cout << "Palettes: " << sff.getPaletteCount() << "\n";
        
        std::cout << "\nSprite List:\n";
        for (size_t i = 0; i < sff.getSpriteCount(); i++) {
            const sff::Sprite* sprite = sff.getSprite(i);
            if (sprite) {
                std::cout << "  [" << i << "] Group: " << sprite->info.group
                         << ", Index: " << sprite->info.index
                         << ", Size: " << sprite->info.width << "x" << sprite->info.height
                         << ", Axis: (" << sprite->info.x << ", " << sprite->info.y << ")\n";
            }
        }
    }
    
    // Extract all sprites
    if (extract_dir) {
        std::cout << "Extracting all sprites to: " << extract_dir << "\n";
        if (!sff.extractAllSprites(extract_dir)) {
            std::cerr << "Error extracting sprites: " << sff.getError() << "\n";
            return 1;
        }
        std::cout << "Successfully extracted " << sff.getSpriteCount() << " sprites\n";
    }
    
    // Extract single sprite
    if (sprite_index >= 0) {
        if (!output_path) {
            std::cerr << "Error: -s/--sprite requires -o/--output to specify output path\n";
            return 1;
        }
        
        std::cout << "Extracting sprite " << sprite_index << " to: " << output_path << "\n";
        if (!sff.extractSprite(sprite_index, output_path)) {
            std::cerr << "Error extracting sprite\n";
            return 1;
        }
        std::cout << "Successfully extracted sprite\n";
    }
    
    // Extract palette
    if (palette_index >= 0) {
        if (!output_path) {
            std::cerr << "Error: -p/--palette requires -o/--output to specify output path\n";
            return 1;
        }
        
        std::cout << "Extracting palette " << palette_index << " to: " << output_path << "\n";
        if (!sff.extractPalette(palette_index, output_path)) {
            std::cerr << "Error extracting palette\n";
            return 1;
        }
        std::cout << "Successfully extracted palette\n";
    }
    
    return 0;
}
