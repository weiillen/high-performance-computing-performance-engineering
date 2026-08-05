#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>

static int next_power_of_two(int x) {
    int p = 1;
    while (p < x) p <<= 1;
    return p;
}

static void compare_swap(float &a, float &b, bool ascending) {
    if ((ascending && a > b) || (!ascending && a < b)) {
        float tmp = a;
        a = b;
        b = tmp;
    }
}

static void local_bitonic_sort(float *a, int n, bool ascending) {
    for (int k = 2; k <= n; k <<= 1) {
        for (int j = k >> 1; j > 0; j >>= 1) {
            for (int i = 0; i < n; i++) {
                int ixj = i ^ j;
                if (ixj > i) {
                    bool dir = ((i & k) == 0);
                    if (!ascending) dir = !dir;
                    compare_swap(a[i], a[ixj], dir);
                }
            }
        }
    }
}

static void compare_split(float *local, float *recv, float *tmp, int n, bool keep_small) {
    if (keep_small) {
        int i = 0, j = 0, k = 0;
        while (k < n) {
            if (i < n && (j >= n || local[i] <= recv[j])) tmp[k++] = local[i++];
            else tmp[k++] = recv[j++];
        }
    } else {
        int i = n - 1, j = n - 1, k = n - 1;
        while (k >= 0) {
            if (i >= 0 && (j < 0 || local[i] >= recv[j])) tmp[k--] = local[i--];
            else tmp[k--] = recv[j--];
        }
    }

    for (int i = 0; i < n; i++) local[i] = tmp[i];
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 4) {
        if (rank == 0) {
            std::fprintf(stderr, "Usage: ./bitonic <N> <input> <output>\n");
        }
        MPI_Finalize();
        return 1;
    }

    int original_n = std::atoi(argv[1]);
    char *input_filename = argv[2];
    char *output_filename = argv[3];

    if ((size & (size - 1)) != 0) {
        if (rank == 0) std::fprintf(stderr, "Process count must be power of two.\n");
        MPI_Finalize();
        return 1;
    }

    int padded_n = next_power_of_two(original_n);
    if (padded_n < size) padded_n = size;

    int local_n = padded_n / size;
    float *local = new float[local_n];
    float *recvbuf = new float[local_n];
    float *tmp = new float[local_n];

    float INF = std::numeric_limits<float>::infinity();

    for (int i = 0; i < local_n; i++) local[i] = INF;

    MPI_File input_file;
    MPI_File_open(MPI_COMM_WORLD, input_filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &input_file);

    int global_start = rank * local_n;
    int read_count = 0;
    if (global_start < original_n) {
        read_count = local_n;
        if (global_start + read_count > original_n) {
            read_count = original_n - global_start;
        }
    }

    MPI_File_read_at(
        input_file,
        (MPI_Offset)global_start * sizeof(float),
        local,
        read_count,
        MPI_FLOAT,
        MPI_STATUS_IGNORE
    );

    MPI_File_close(&input_file);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    local_bitonic_sort(local, local_n, true);

    for (int k = 2; k <= size; k <<= 1) {
        for (int j = k >> 1; j > 0; j >>= 1) {
            int partner = rank ^ j;

            MPI_Sendrecv(
                local, local_n, MPI_FLOAT, partner, 0,
                recvbuf, local_n, MPI_FLOAT, partner, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );

            bool ascending_group = ((rank & k) == 0);
            bool keep_small = ((rank & j) == 0);

            if (!ascending_group) keep_small = !keep_small;

            compare_split(local, recvbuf, tmp, local_n, keep_small);
        }
    }

    double end_time = MPI_Wtime();

    MPI_File output_file;
    MPI_File_open(
        MPI_COMM_WORLD,
        output_filename,
        MPI_MODE_CREATE | MPI_MODE_WRONLY,
        MPI_INFO_NULL,
        &output_file
    );

    MPI_File_set_size(output_file, (MPI_Offset)original_n * sizeof(float));

    int write_count = 0;
    if (global_start < original_n) {
        write_count = local_n;
        if (global_start + write_count > original_n) {
            write_count = original_n - global_start;
        }
    }

    MPI_File_write_at(
        output_file,
        (MPI_Offset)global_start * sizeof(float),
        local,
        write_count,
        MPI_FLOAT,
        MPI_STATUS_IGNORE
    );

    MPI_File_close(&output_file);

    if (rank == 0) {
        std::printf("Elapsed time : %lf\n", end_time - start_time);
    }

    delete[] local;
    delete[] recvbuf;
    delete[] tmp;

    MPI_Finalize();
    return 0;
}
