# Sobel edge detection with HIP and OpenMP offload

This module preserves several stages of a 5 × 5 Sobel image-filter implementation:

- host C++ baseline;
- HIP GPU kernel with device memory management;
- OpenMP target-offload variants;
- a later HIP homework version.

The code reads and writes PNG images with libpng, evaluates horizontal and vertical convolution masks for RGB channels, and clamps the gradient magnitude to 8-bit output.

## Original files

- `original/lab/`: baseline and HIP lab versions
- `original/openmp-offload/`: target-offload development version
- `original/openmp-offload-submission/`: submitted OpenMP-offload version and Makefile
- `original/final-homework/`: later HIP homework source

No GPU run is claimed here because the portfolio environment did not provide the original HIP/ROCm compiler and GPU target.
