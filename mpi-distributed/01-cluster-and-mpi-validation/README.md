# Multi-node MPI cluster validation

This module documents cluster bring-up and a successful Open MPI run across a head node and worker nodes. The terminal evidence shows an initial slot-allocation error followed by a corrected launch command using explicit host slots.

The successful run launches eight ranks across `head`, `work1`, `work2`, and `work3`, with each rank reporting its host and rank number.

## Evidence

- [`evidence/multinode_mpi_run.png`](evidence/multinode_mpi_run.png)
- [`evidence/cluster_setup_context.png`](evidence/cluster_setup_context.png)

These screenshots are preserved exactly as uploaded.
