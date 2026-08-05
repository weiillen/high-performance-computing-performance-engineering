# OpenMP profiling and HPL parameter study

This module combines three performance-engineering activities:

1. compare `(i,j,k)` and `(i,k,j)` matrix-multiplication loop orders;
2. inspect cache behavior and OpenMP scaling with AMD uProf;
3. compare HPL block sizes while holding the matrix size and process grid fixed.

## Recorded findings

- `mat_mul_a`: about **81.173 s**
- `mat_mul_b`: about **14.510 s**
- recorded improvement: approximately **5.6×** from loop reordering
- the report records an L1 data-cache miss rate of **67.9637%** for the original kernel
- OpenMP improved strongly from one to two threads but not from two to four on a VM exposing two CPUs

HPL values recorded in the report:

| NB | Best visible GFLOPS |
|---:|---:|
| 64 | 2.6942 |
| 128 | 2.1315 |
| 256 | 2.3626 |

## Contents

- `original/`: source, HPL configuration, run script, and final report
- `evidence/`: selected CSV summaries and HPL text output referenced by the report

Large raw profiler databases were excluded; the compact CSV reports were retained.
