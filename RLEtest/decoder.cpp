#include "compressed_images.hpp"
#include <iostream>
#include <stdint.h>
#include <thread>
#include <chrono>

unsigned char* decompressRLETest(const unsigned char* compressedImage, size_t compressedSize) {
  size_t decompressedIndex = 0; // Index for writing to decompressedBitmap
  unsigned char currentByte = 0; // To accumulate bits for the current byte
  int bitPosition = 7; // Start from the MSB
  uint8_t color = 0; // Start with black
  unsigned char* decompressedBitmap = new unsigned char[256 * 64 / 8]; // Allocate memory for the decompressed image

  for (size_t i = 0; i < compressedSize; i++) {
    unsigned char count = compressedImage[i];

    for (int j = 0; j < count; j++) {
      // Set the bit at the current position to the color
      if (color == 1) {
        currentByte |= (1 << bitPosition);
      } // No need to set for black (0) as bits are initialized to 0

      bitPosition--;

      // If the byte is filled, add it to the vector and reset
      if (bitPosition < 0) {
        decompressedBitmap[decompressedIndex++] = currentByte;
        currentByte = 0; // Reset for the next byte
        bitPosition = 7; // Reset bit position
      }
    }
    color = 1 - color; // Toggle color
  }

  // Add the last byte if it was being filled
  if (bitPosition != 7) {
    decompressedBitmap[decompressedIndex++] = currentByte;
  }
  
  return decompressedBitmap;
}

void emulateScreenOnTerminal(const unsigned char* bitmap, size_t width, size_t height) {
    size_t bytesPerRow = width / 8; // 8 pixels per byte
    std::string output; // Accumulate the output
    for (size_t row = 0; row < height; row++) {
        for (size_t byteIndex = 0; byteIndex < bytesPerRow; byteIndex++) {
            unsigned char byte = bitmap[row * bytesPerRow + byteIndex];
            for (int bit = 7; bit >= 0; bit--) {
                // Extract each bit from the byte (0 for black, 1 for white)
                unsigned char pixel = (byte >> bit) & 1;
                output += (pixel ? 'x' : ' '); // Accumulate the pixel representation
            }
        }
        output += '\n'; // New line at the end of each row
    }
    std::cout << output; // Print the entire output at once
}

int main() {
    size_t compressedSize;
    const unsigned char* compressedImage;
    unsigned char* decompressedBitmap;
    for(CompressedFrame compr : all_frames) {
        
        //system("cls"); 
        std::cout << "\033[H";

        compressedSize = compr.size;
        compressedImage = compr.data;
        decompressedBitmap = decompressRLETest(compressedImage, compressedSize);
        emulateScreenOnTerminal(decompressedBitmap, 256, 64);
        
        
        delete[] decompressedBitmap;

        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}