#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

bool CompressImage(const string& path, int quality = 80) {
    int width, height, channels;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image) {
        cerr << "Error: Could not load image " << path << endl;
        return false;
    }

    const string& outputPath = path.find('.') != string::npos ? path.substr(0, path.find('.')) + "_crammed.jpg" : path + "_crammed.jpg";


    // Get the original file size
    FILE* f = fopen(path.c_str(), "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long originalSize = ftell(f);
        fclose(f);
        cout << "Original size: " << originalSize / 1024.0 << " KB" << endl;
    }

    int success = stbi_write_jpg(outputPath.c_str(), width, height, channels, image, quality);
    
    // Get the compressed file size
    f = fopen(outputPath.c_str(), "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long compressedSize = ftell(f);
        fclose(f);
        cout << "Compressed size: " << compressedSize / 1024.0 << " KB" << endl;
    }

    stbi_image_free(image);

    if (!success) {
        cerr << "error: Failed to write compressed image" << endl;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        cerr << "usage: cram <input_image_path> [quality]" << endl;
        cerr << "quality: 1-100 (default: 80, higher is better quality but larger file)" << endl;
        return -1;
    }

    int quality = 80;
    if (argc == 3) {
        quality = atoi(argv[2]);
        if (quality < 1 || quality > 100) {
            cerr << "quality must be between 1 and 100" << endl;
            return -1;
        }
    }

    if (!CompressImage(argv[1], quality)) {
        return -1;
    }

    return 0;
}

