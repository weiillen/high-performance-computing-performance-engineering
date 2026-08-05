# AVX2 and blocked matrix multiplication

This C++17 project compares four dense matrix-multiplication paths:

- naive triple loop;
- AVX2 vectorization;
- cache blocking;
- blocked AVX2.

It includes a binary input generator and a floating-point result checker. The original source uses 32-byte aligned storage, 64-element tiles, and 256-bit AVX registers.

## Captured results

For a 1241 × 1111 matrix multiplied by an 1111 × 1231 matrix, one uploaded terminal capture records:

| Implementation | Runtime |
|---|---:|
| Naive | 1468 ms |
| Blocked | 171 ms |
| AVX2 + blocked | 190 ms |

Both checked optimized outputs recorded zero differences and maximum error `0.000000`.

For 4096 × 4096 matrices, another capture records:

| Implementation | Runtime |
|---|---:|
| Blocked | 6040 ms |
| AVX2 + blocked | 5632 ms |

These are historical measurements from the uploaded screenshots, not new benchmarks.

## Contents

- `original/`: the unchanged implementation, generator, checker, shell helper, Makefile, and report
- `evidence/`: five unchanged terminal/profile screenshots
