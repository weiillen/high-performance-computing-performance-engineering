# Cache locality in matrix traversal

This study investigates how traversal order changes the performance of summing a row-major matrix. The original report compares column-wise and row-wise access using timing and `perf` cache statistics.

## Recorded result

| Metric | Original traversal | Modified traversal |
|---|---:|---:|
| Runtime | 830 ms | 368 ms |
| L1 data-cache load misses | 152,522,772 | 28,080,906 |
| L1 miss rate | 8.47% | 1.54% |

The report calculates an approximately **2.26× speedup** after changing the access order.

## Original artifact

- [`original/report.pdf`](original/report.pdf)

The source appears as listings and terminal captures inside the original report; no standalone source file was present in the archive.
