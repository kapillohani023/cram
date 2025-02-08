#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

bool CompressImage(const string& inputPath, const string& outputPath, int quality = 80) {
    int width, height, channels;
    unsigned char* image = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);
    if (!image) {
        cerr << "Error: Could not load image " << inputPath << endl;
        return false;
    }

    int success = stbi_write_jpg(outputPath.c_str(), width, height, channels, image, quality);
    
    stbi_image_free(image);

    if (!success) {
        cerr << "error: Failed to write compressed image" << endl;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
        cerr << "usage: cram <input_image_path> <output_jpg_path> [quality]" << endl;
        cerr << "quality: 1-100 (default: 80, higher is better quality but larger file)" << endl;
        return -1;
    }

    int quality = 80;
    if (argc == 4) {
        quality = atoi(argv[3]);
        if (quality < 1 || quality > 100) {
            cerr << "quality must be between 1 and 100" << endl;
            return -1;
        }
    }

    if (!CompressImage(argv[1], argv[2], quality)) {
        return -1;
    }

    return 0;
}

