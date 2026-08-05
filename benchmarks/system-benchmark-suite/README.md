# HPC system benchmark suite

This report evaluates different parts of a small cluster using HPL, HPCG, STREAM, OSU Micro-Benchmarks, and IOR. The purpose is not to produce a single universal score, but to distinguish compute, memory, communication, and storage behavior.

## Recorded results

| Area | Benchmark | Recorded result |
|---|---|---|
| Dense compute | HPL | all 18 residual tests passed; visible results around 1.55–1.67 GFLOP/s |
| Sparse compute | HPCG | VALID; 3.56383 GFLOP/s |
| Memory | STREAM | Copy 34,579.7 MB/s; Triad 22,130.8 MB/s |
| Same-node communication | OSU | 1-byte latency 0.16 µs; peak bandwidth about 12,409.57 MB/s |
| Cross-node communication | OSU | 1-byte latency 23.95 µs; peak bandwidth about 3,853.56 MB/s |
| Parallel I/O | IOR | write 1,916.50 MiB/s; read 17,164 MiB/s |

The report explicitly warns that the unusually high IOR read result may be influenced by Linux page cache.

## Original artifact

- [`original/report.pdf`](original/report.pdf)
