# Cache-aware matrix transpose

This C++ experiment compares a naive transpose of an 8192 × 8192 integer matrix with tiled variants using tile sizes 16, 32, 48, 64, 96, and 128. The goal is to evaluate whether blocking improves cache reuse during transposition.

## Original files

- [`original/transpose.cpp`](original/transpose.cpp)
- [`original/Makefile`](original/Makefile)

The source is preserved exactly as uploaded. Runtime depends on the host CPU, compiler, and memory system.
