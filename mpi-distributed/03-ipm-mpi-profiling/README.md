# MPI profiling with IPM

This short study interprets an IPM profile of an MPI application. The report identifies `MPI_File_open`, `MPI_File_close`, and `MPI_File_read_at` as the three largest MPI-time consumers and concludes that MPI file I/O dominates the observed run.

## Original artifact

- [`original/report.pdf`](original/report.pdf)

The report also contains the pie-chart visualization generated from the two-process profile.
