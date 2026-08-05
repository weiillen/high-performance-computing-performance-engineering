#include <iostream>
#include <cstdlib>
#include <cassert>
#include <zlib.h>
#include <png.h>
#include <hip/hip_runtime.h>

#define Z 2
#define Y 5
#define X 5
#define xBound X / 2
#define yBound Y / 2
#define SCALE 8

int read_png(const char* filename, unsigned char** image, unsigned* height, 
             unsigned* width, unsigned* channels) {

    unsigned char sig[8];
    FILE* infile;
    infile = fopen(filename, "rb");

    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
        return 1;   /* bad signature */

    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */
  
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr, width, height, &bit_depth, &color_type, NULL, NULL, NULL);

    png_uint_32  i, rowbytes;
    png_bytep  row_pointers[*height];
    png_read_update_info(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *channels = (int) png_get_channels(png_ptr, info_ptr);

    if ((*image = (unsigned char *) malloc(rowbytes * *height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 3;
    }

    for (i = 0;  i < *height;  ++i)
        row_pointers[i] = *image + i * rowbytes;
    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);
    return 0;
}

void write_png(const char* filename, png_bytep image, const unsigned height, const unsigned width, 
               const unsigned channels) {
    FILE* fp = fopen(filename, "wb");
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

__constant__ __device__ char mask[Z][Y][X] = { { { -1, -4, -6, -4, -1 },
                                        { -2, -8, -12, -8, -2 },
                                        { 0, 0, 0, 0, 0 },
                                        { 2, 8, 12, 8, 2 },
                                        { 1, 4, 6, 4, 1 } },
                                      { { -1, -2, 0, 2, 1 },
                                        { -4, -8, 0, 8, 4 },
                                        { -6, -12, 0, 12, 6 },
                                        { -4, -8, 0, 8, 4 },
                                        { -1, -2, 0, 2, 1 } } };

inline __device__ int bound_check(int val, int lower, int upper) {
    if (val >= lower && val < upper)
        return 1;
    else
        return 0;
}


__global__ void sobel(unsigned char *s, unsigned char *t,
                      unsigned height, unsigned width, unsigned channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    int tileW = blockDim.x + 2 * xBound + 1;  // +1 padding to reduce bank conflict
    int tileH = blockDim.y + 2 * yBound;

    extern __shared__ unsigned char tile[];

    int tid = threadIdx.y * blockDim.x + threadIdx.x;
    int blockThreads = blockDim.x * blockDim.y;
    int tileSize = tileW * tileH * channels;

    // Load shared tile including halo.
    // IMPORTANT: all threads must do this before any return.
    for (int i = tid; i < tileSize; i += blockThreads) {
        int c = i % channels;
        int pixel = i / channels;
        int lx = pixel % tileW;
        int ly = pixel / tileW;

        int gx = blockIdx.x * blockDim.x + lx - xBound;
        int gy = blockIdx.y * blockDim.y + ly - yBound;

        unsigned char value = 0;
        if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
            value = s[channels * (width * gy + gx) + c];
        }

        tile[i] = value;
    }

    __syncthreads();

    if (x >= width || y >= height) return;

    int out_idx = channels * (width * y + x);

    if (x < xBound || x >= width - xBound || y < yBound || y >= height - yBound) {
        t[out_idx + 0] = 0;
        t[out_idx + 1] = 0;
        t[out_idx + 2] = 0;
        return;
    }

    int valR0 = 0, valG0 = 0, valB0 = 0;
    int valR1 = 0, valG1 = 0, valB1 = 0;

    int sx = threadIdx.x + xBound;
    int sy = threadIdx.y + yBound;

    #pragma unroll
    for (int v = -yBound; v <= yBound; ++v) {
        #pragma unroll
        for (int u = -xBound; u <= xBound; ++u) {
            int tile_idx = channels * (tileW * (sy + v) + (sx + u));

            const unsigned char B = tile[tile_idx + 0];
            const unsigned char G = tile[tile_idx + 1];
            const unsigned char R = tile[tile_idx + 2];

            int m0 = mask[0][u + xBound][v + yBound];
            int m1 = mask[1][u + xBound][v + yBound];

            valR0 += R * m0;
            valG0 += G * m0;
            valB0 += B * m0;

            valR1 += R * m1;
            valG1 += G * m1;
            valB1 += B * m1;
        }
    }

    float totalR = sqrtf((float)(valR0 * valR0 + valR1 * valR1)) / SCALE;
    float totalG = sqrtf((float)(valG0 * valG0 + valG1 * valG1)) / SCALE;
    float totalB = sqrtf((float)(valB0 * valB0 + valB1 * valB1)) / SCALE;

    t[out_idx + 2] = (totalR > 255.0f) ? 255 : (unsigned char)totalR;
    t[out_idx + 1] = (totalG > 255.0f) ? 255 : (unsigned char)totalG;
    t[out_idx + 0] = (totalB > 255.0f) ? 255 : (unsigned char)totalB;
}

int main(int argc, char **argv) {
    assert(argc == 3);
    unsigned height, width, channels;
    unsigned char *src = NULL, *dst;
    unsigned char *dsrc, *ddst;

    /* read the image to src, and get height, width, channels */
    if (read_png(argv[1], &src, &height, &width, &channels)) {
        std::cerr << "Error in read png" << std::endl;
        return -1;
    }

    dst = (unsigned char *)malloc(height * width * channels * sizeof(unsigned char));
    hipHostRegister(src, height * width * channels * sizeof(unsigned char), hipHostRegisterDefault);

    // hipMalloc(...) for device src and device dst
    hipMalloc(&dsrc, height * width * channels * sizeof(unsigned char));
    hipMalloc(&ddst, height * width * channels * sizeof(unsigned char));

    // hipMemcpy(...) copy source image to device (mask matrix if necessary)
    hipMemcpy(dsrc, src, height * width * channels * sizeof(unsigned char), hipMemcpyHostToDevice);

    // decide to use how many blocks and threads
    dim3 threadsPerBlock(32, 8);
    dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                   (height + threadsPerBlock.y - 1) / threadsPerBlock.y);
  
    size_t sharedBytes =
        (threadsPerBlock.x + 2 * xBound + 1) *
        (threadsPerBlock.y + 2 * yBound) *
        channels * sizeof(unsigned char); 

    // launch hip kernel
    hipLaunchKernelGGL(sobel, numBlocks, threadsPerBlock, sharedBytes, 0,
                       dsrc, ddst, height, width, channels);

    // hipMemcpy(...) copy result image to host
    hipMemcpy(dst, ddst, height * width * channels * sizeof(unsigned char), hipMemcpyDeviceToHost);

    write_png(argv[2], dst, height, width, channels);
    free(src);
    free(dst);
    hipFree(dsrc);
    hipFree(ddst);
    return 0;
}
