# MPI parallel bitonic sort

This program sorts binary 32-bit floating-point values across MPI ranks. It pads the data to a power of two with positive infinity, reads and writes disjoint file regions with MPI-IO, and performs partner exchanges using `rank ^ j` and `MPI_Sendrecv`.

## Recorded verification and scaling

- all eight judge test cases passed;
- fixed problem size: 400,009 values;
- best recorded runtime: **0.031778 s** at 16 processes;
- best recorded speedup: **3.05×**;
- the 32-process run slowed to 0.048290 s under oversubscription;
- IPM identified `MPI_Sendrecv` as the largest MPI cost.

## Original files

- [`original/bitonic.cc`](original/bitonic.cc)
- [`original/report.pdf`](original/report.pdf)

MPI compilation was not rerun in the portfolio environment because an MPI compiler wrapper was unavailable.
