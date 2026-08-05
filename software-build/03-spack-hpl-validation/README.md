# Spack-built HPL validation

This module preserves a terminal screenshot from an HPL build installed through a Spack environment. The screenshot records linked Intel oneAPI MKL, Open MPI, and OpenMP runtime libraries and shows the test summary:

- 864 tests completed and passed residual checks;
- 0 failed residual checks;
- 0 tests skipped for illegal input values.

The screenshot also shows small test cases with `N = 35`, `NB = 4`, and a 4 × 1 process grid. This is validation evidence, not a competitive throughput benchmark.

## Evidence

- [`evidence/hpl_validation.png`](evidence/hpl_validation.png)
