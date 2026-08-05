#include <iostream>
#include <cstdlib>
#include <cassert>
#include <zlib.h>
#include <png.h>
#include <cmath>

#define MASK_N 2
#define MASK_X 5
#define MASK_Y 5
#define SCALE 8

int mask[MASK_N][MASK_X][MASK_Y] = { 
    {{ -1, -4, -6, -4, -1},
     { -2, -8,-12, -8, -2},
     {  0,  0,  0,  0,  0}, 
     {  2,  8, 12,  8,  2}, 
     {  1,  4,  6,  4,  1}},
    {{ -1, -2,  0,  2,  1}, 
     { -4, -8,  0,  8,  4}, 
     { -6,-12,  0, 12,  6}, 
     { -4, -8,  0,  8,  4}, 
     { -1, -2,  0,  2,  1}} 
};

int read_png(const char* filename, unsigned char** image, unsigned* height, 
             unsigned* width, unsigned* channels) {
    FILE* infile = fopen(filename, "rb");
    
    if (!infile) {
        fprintf(stderr, "Error: Cannot open input file %s\n", filename);
        exit(1);
    }
    
    unsigned char sig[8];
    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8)) return 1;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr, width, height, &bit_depth, &color_type, NULL, NULL, NULL);

    png_read_update_info(png_ptr, info_ptr);
    png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *channels = (int) png_get_channels(png_ptr, info_ptr);

    *image = (unsigned char *) malloc(rowbytes * *height);
    png_bytep row_pointers[*height];
    for (unsigned int i = 0; i < *height; ++i)
        row_pointers[i] = *image + i * rowbytes;
        
    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(infile);
    return 0;
}

void write_png(const char* filename, png_bytep image, const unsigned height, const unsigned width, 
               const unsigned channels) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open output file %s\n", filename);
        exit(1);
    }
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_filter(png_ptr, 0, PNG_NO_FILTERS);
    png_write_info(png_ptr, info_ptr);
    png_set_compression_level(png_ptr, 1);

    png_bytep row_ptr[height];
    for (int i = 0; i < height; ++ i) {
        row_ptr[i] = image + i * width * channels * sizeof(unsigned char);
    }
    png_write_image(png_ptr, row_ptr);
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

void sobel (unsigned char* s, unsigned char* t, unsigned height, unsigned width, unsigned channels) {
    
    int h = height;
    int w = width;
    int c = channels;
    int total_elements = h * w * c;

    #pragma omp target data map(to: s[0:total_elements], mask) map(from: t[0:total_elements])
    {
        #pragma omp target teams distribute parallel for collapse(2)
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                
                double val[6];
                for (int i = 0; i < 6; ++i) {
                    val[i] = 0.0;
                }

                int adjustX = (MASK_X % 2) ? 1 : 0;
                int adjustY = (MASK_Y % 2) ? 1 : 0;
                int xBound = MASK_X / 2;
                int yBound = MASK_Y / 2;

                for (int i = 0; i < MASK_N; ++i) {
                    for (int v = -yBound; v < yBound + adjustY; ++v) {
                        for (int u = -xBound; u < xBound + adjustX; ++u) {
                            if ((x + u) >= 0 && (x + u) < w && (y + v) >= 0 && (y + v) < h) {
                                int idx = c * (w * (y + v) + (x + u));
                                int R = s[idx + 2];
                                int G = s[idx + 1];
                                int B = s[idx + 0];
                                
                                val[i*3+2] += R * mask[i][u + xBound][v + yBound];
                                val[i*3+1] += G * mask[i][u + xBound][v + yBound];
                                val[i*3+0] += B * mask[i][u + xBound][v + yBound];
                            }    
                        }
                    }
                }

                double totalR = 0.0;
                double totalG = 0.0;
                double totalB = 0.0;
                for (int i = 0; i < MASK_N; ++i) {
                    totalR += val[i * 3 + 2] * val[i * 3 + 2];
                    totalG += val[i * 3 + 1] * val[i * 3 + 1];
                    totalB += val[i * 3 + 0] * val[i * 3 + 0];
                }
                
                totalR = sqrt(totalR) / SCALE;
                totalG = sqrt(totalG) / SCALE;
                totalB = sqrt(totalB) / SCALE;
                
                t[c * (w * y + x) + 2] = (totalR > 255.0) ? 255 : (unsigned char)totalR;
                t[c * (w * y + x) + 1] = (totalG > 255.0) ? 255 : (unsigned char)totalG;
                t[c * (w * y + x) + 0] = (totalB > 255.0) ? 255 : (unsigned char)totalB;
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.png> <output.png>\n", argv[0]);
        return 1;
    }

    unsigned height, width, channels;
    unsigned char* host_s = NULL;
    read_png(argv[1], &host_s, &height, &width, &channels);
    
    unsigned char* host_t = (unsigned char*) malloc(height * width * channels * sizeof(unsigned char));
    if (!host_t) {
        fprintf(stderr, "Error: Failed to allocate memory for output image\n");
        return 1;
    }
    
    sobel(host_s, host_t, height, width, channels);
    
    write_png(argv[2], host_t, height, width, channels);

    free(host_s);
    free(host_t);
    return 0;
}

