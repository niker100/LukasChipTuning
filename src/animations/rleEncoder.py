from PIL import Image
import os

def rle_encode(bitmap):
    """Encode bitmap data using Run-Length Encoding (RLE) for binary images,
    ensuring each run length does not exceed 255."""
    compressed = []
    count = 1
    for i in range(1, len(bitmap)):
        if bitmap[i] == bitmap[i-1]:
            count += 1
            # Split the encoding into multiple segments if count exceeds 255
            if count == 256:
                compressed.append(255)
                compressed.append(bitmap[i])
                count = 1
        else:
            if count > 0:
                compressed.append(count)
                compressed.append(bitmap[i-1])
            count = 1
    if count > 0:
        compressed.append(count)
        compressed.append(bitmap[-1])
    return compressed

def convert_image_to_rle(image_path):
    """Convert an image to RLE compressed format."""
    with Image.open(image_path) as img:
        # Convert image to black and white
        bw = img.convert('1')
        # Flatten the image data and convert to binary format
        bitmap = [1 if pixel else 0 for pixel in bw.getdata()]
        # Apply RLE compression
        compressed = rle_encode(bitmap)
        return compressed
    
def main():
    image_sequence_folder = 'images'
    output_file = 'compressed_images.hpp'
    
    frame_arrays = []
    
    with open(output_file, 'w') as cpp_file:
        cpp_file.write('#include <vector>\n#include <array>\n#include <main.hpp>\n\n')
        
        for index, image_name in enumerate(sorted(os.listdir(image_sequence_folder))):
            if image_name.endswith(('.png', '.jpg', '.jpeg')):
                image_path = os.path.join(image_sequence_folder, image_name)
                compressed = convert_image_to_rle(image_path)
                frame_name = f"frame_{index}"
                frame_arrays.append((frame_name, len(compressed)))
                cpp_file.write(f'const unsigned char {frame_name}[] PROGMEM = ' + '{')
                cpp_file.write(', '.join(f'0x{length:02x}' for length in compressed))
                cpp_file.write('};\n')
        
        # Write the array of all frames
        cpp_file.write('CompressedFrame all_frames[] PROGMEM = {')
        cpp_file.write(', '.join(f'{{ {frame[0]}, {frame[1]} }}' for frame in frame_arrays))
        cpp_file.write('};\n')
        
    print(f"Compressed images have been written to {output_file}")

if __name__ == "__main__":
    main()