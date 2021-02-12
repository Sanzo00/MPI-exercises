#include <mpi.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

// Create an array of random numbers. Each numaber has a value from 0 - 1
float* create_rand_nums(int num_elements) {
    float *rand_nums = (float*) malloc(sizeof(float) * num_elements);
    assert(rand_nums != NULL);
    for (int i = 0; i < num_elements; ++i) {
        rand_nums[i] = (rand() / (float)RAND_MAX);
    }
    return rand_nums;
}

// Computes the average of an array of numbers
float compute_avg(float *array, int num_elements) {
    float sum = 0.0;
    for (int i = 0; i < num_elements; ++i) {
        sum += array[i];
    }
    return sum / num_elements;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./avg num_elements_per_process\n");
        exit(-1);
    }

    int num_elements_per_proc = atoi(argv[1]);
    srand(time(NULL));

    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    float *rand_nums = NULL;
    if (world_rank == 0) {
        rand_nums = create_rand_nums(num_elements_per_proc * world_size);
    }

    // Each process create a buffer that recv subset array
    float *sub_rand_nums  = (float*) malloc(sizeof(float) * num_elements_per_proc);
    assert(sub_rand_nums != NULL);

    // Scatter the random numbers from the root process to all processes
    MPI_Scatter(rand_nums, num_elements_per_proc, MPI_FLOAT, sub_rand_nums,
                num_elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float sub_avg = compute_avg(sub_rand_nums, num_elements_per_proc);

    // Gather all partial averages down to the root process
    float *sub_avgs = NULL;

    if (world_rank == 0 ){
        sub_avgs = (float*) malloc(sizeof(float) * world_size);
        assert(sub_avgs != NULL);
    }
    MPI_Gather(&sub_avg, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Compute the total average of all sub_avg
    if (world_rank == 0) {
        float avg = compute_avg(sub_avgs, world_size);
        printf("Avg of all elements is %f\n", avg);
        // Compute the average acrosse the original data for comparison
        float original_data_avg = compute_avg(rand_nums, num_elements_per_proc * world_size);
        printf("Avg compute across original data is %f\n", original_data_avg);
    }

    if (world_rank == 0) {
        free(rand_nums);
        free(sub_avgs);
    }
    free(sub_rand_nums);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}