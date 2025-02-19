#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"

using namespace std;

long GetFileSize(const string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

bool CompressImage(const string& path, const string& outputPath, int quality, int newWidth = 0, int newHeight = 0) {
    int width, height, channels;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image) {
        cerr << "Error: Could not load image " << path << endl;
        return false;
    }
    
    unsigned char* resizedImage = image;
    if (newWidth > 0 && newHeight > 0 && (newWidth != width || newHeight != height)) {
        resizedImage = (unsigned char*)malloc(newWidth * newHeight * channels);
        stbir_resize_uint8_linear(image, width, height, width * channels,
                                resizedImage, newWidth, newHeight, newWidth * channels,
                                (stbir_pixel_layout)channels);
        width = newWidth;
        height = newHeight;
    }

    int success = stbi_write_jpg(outputPath.c_str(), width, height, channels, resizedImage, quality);

    if (resizedImage != image) {
        free(resizedImage);
    }
    stbi_image_free(image);

    if (!success) {
        cerr << "Error: Failed to write compressed image" << endl;
        return false;
    }

    return true;
}

bool CramImage(const string& path, long targetSize) {
    const string outputPath = path.substr(0, path.find_last_of('.')) + "_crammed.jpg";
    
    int width, height, channels;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image) {
        cerr << "Error: Could not load image " << path << endl;
        return false;
    }
    stbi_image_free(image);
    if (GetFileSize(path) <= targetSize) {
        cout << "Image is already smaller than the target size" << endl;
        return true;
    }

    int left = 20, right = 99;
    bool sizeAchieved = false;


    while (left <= right) {
        int quality = (left + right) / 2;
        
        if (!CompressImage(path, outputPath, quality)) {
            return false;
        }

        long currentSize = GetFileSize(outputPath);
        if (currentSize <= targetSize) {
            sizeAchieved = true;
            if (left == right) break;
            left = quality + 1;
        } else {
            right = quality - 1;
        }
    }


    if (!sizeAchieved) {
        int currentWidth = width;
        int currentHeight = height;
        const int MIN_DIMENSION = 32; // Prevent images from becoming too small

        while (!sizeAchieved) {
            double scaleFactor = sqrt((double)targetSize / GetFileSize(outputPath));
            int newWidth = max(MIN_DIMENSION, (int)(currentWidth * scaleFactor));
            int newHeight = max(MIN_DIMENSION, (int)(currentHeight * scaleFactor));

            if (newWidth == currentWidth && newHeight == currentHeight) {
                cout << "Warning: Cannot achieve target size without making image too small" << endl;
                break;
            }
            if (!CompressImage(path, outputPath, 20, newWidth, newHeight)) {
                return false;
            }

            long currentSize = GetFileSize(outputPath);

            if (currentSize <= targetSize) {
                sizeAchieved = true;
            } else {
                currentWidth = newWidth;
                currentHeight = newHeight;
            }
        }
    }
    cout << "Original size: " << GetFileSize(path) / 1024.0 << " KB" << endl;
    cout << "Compressed size: " << GetFileSize(outputPath) / 1024.0 << " KB" << endl;
    cout << "Crammed image saved to: " << outputPath << endl;
    return true;

}

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "usage: cram <input_image_path> <desired_file_size_lower_limit_in_kb>" << endl;
        return -1;
    }

    long targetSize = atol(argv[2]) * 1024; // Convert KB to bytes
    if (targetSize <= 0) {
        cerr << "Error: Invalid target size" << endl;
        return -1;
    }

    if (!CramImage(argv[1], targetSize)) {
        return -1;
    }

    return 0;
}

