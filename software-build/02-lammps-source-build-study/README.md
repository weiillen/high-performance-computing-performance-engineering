# LAMMPS source-build and performance comparison

This study automates two LAMMPS builds for the Lennard-Jones melt example:

- baseline: `-O2`, MPI enabled, OpenMP package disabled;
- comparison build: `-O3 -march=native -ffast-math`, MPI and OpenMP enabled.

## Recorded result

The comparison build was **slower**, and the original report presents that negative result instead of claiming an unsupported speedup.

| Metric | Baseline | OpenMP/compiler-flag build |
|---|---:|---:|
| Total wall time | 0:13:08 | 0:47:38 |
| Final loop time | 232.304 s | 893.627 s |
| Timesteps/s | 64.571 | 16.786 |

The report attributes the result cautiously to possible threading, synchronization, and execution-layout overhead, while noting that isolated ablation runs would be needed to determine the exact cause.

## Original files

- [`original/build_script.sh`](original/build_script.sh)
- [`original/report.pdf`](original/report.pdf)
- [`evidence/visualization.png`](evidence/visualization.png)
