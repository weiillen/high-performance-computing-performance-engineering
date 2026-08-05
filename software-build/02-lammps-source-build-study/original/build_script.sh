#!/bin/bash
set -e  # Exit immediately if any command fails

echo "========================================="
echo "Starting Automated LAMMPS Build Script"
echo "========================================="

# 1. Clone the official LAMMPS repository if not already present
if [ ! -d "lammps" ]; then
    echo "Cloning official LAMMPS repository..."
    git clone --depth 1 --branch stable https://github.com/lammps/lammps.git
fi

cd lammps
LAMMPS_DIR=$(pwd)

echo "========================================="
echo "Building BASELINE Version (-O2)"
echo "========================================="
mkdir -p build_baseline
cd build_baseline

cmake ../cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_CXX_FLAGS="-O2" \
    -D BUILD_MPI=yes \
    -D BUILD_OMP=no \
    -D PKG_OPENMP=no

make -j$(nproc)
cp lmp ../../lmp_baseline

echo "========================================="
echo "Building OPTIMIZED Version (-O3 + Native AMD + OpenMP)"
echo "========================================="
cd $LAMMPS_DIR
mkdir -p build_optimized
cd build_optimized

cmake ../cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_CXX_FLAGS="-O3 -march=native -ffast-math" \
    -D BUILD_MPI=yes \
    -D BUILD_OMP=yes \
    -D PKG_OPENMP=yes

make -j$(nproc)
cp lmp ../../lmp_optimized

echo "========================================="
echo "Created binaries: lmp_baseline and lmp_optimized"
echo "========================================="
