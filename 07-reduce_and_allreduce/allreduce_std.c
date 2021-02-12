#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

float *create_rand_nums(int num_elements) {
    float *rand_nums = (float*) malloc(sizeof(float) * num_elements);
    for (int i = 0; i < num_elements; ++i) {
        rand_nums[i] = rand() / (float) RAND_MAX;
    }
    return rand_nums;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: avg num_elements_per_proc\n");
        exit(-1);
    }

    MPI_Init(NULL, NULL);
    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create random array of elements on all process
    srand(time(NULL) * world_rank);
    int num_elements_per_proc = atoi(argv[1]);
    float *rand_nums = create_rand_nums(num_elements_per_proc);

    // Sum the numbers locally
    float local_sum = 0;
    for (int i = 0; i < num_elements_per_proc; ++i) {
        local_sum += rand_nums[i];
    }

    // Reduce all of the local sums into the global sum in order to calculate the mean
    float global_sum;
    MPI_Allreduce(&local_sum , &global_sum , 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    float mean = global_sum / world_size / num_elements_per_proc;

    // Compute the local sum of the squared differences from the mean
    float local_sq_diff = 0;
    for (int i = 0; i < num_elements_per_proc; ++i) {
        local_sq_diff += (rand_nums[i] - mean) * (rand_nums[i] - mean);
    }

    // Reduce the global sum of the squared differences to the root process
    float global_sq_diff;
    MPI_Reduce(&local_sq_diff, &global_sq_diff, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        float stddev = sqrt(global_sq_diff / world_size / num_elements_per_proc);
        printf("Mean - %f, Standard deviation = %f\n", mean, stddev);
    }

    free(rand_nums);
    MPI_Finalize();

    return 0;
}