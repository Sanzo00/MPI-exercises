#include <mpi.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void my_bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator) {
    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (root == world_rank) { // root process send data to other process
        for (int i = 0; i < world_size; ++i) {
            if (i != world_rank) {
                MPI_Send(data, count, datatype, i, 0, communicator);
            }
        }
    }else { // other process receive data from root
        MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: compare_bcast num_elements num_trials\n");
        exit(1);
    }
    int num_elements = atoi(argv[1]);
    int num_trials = atoi(argv[2]);

    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    double total_my_bcast_time = 0.0;
    double total_mpi_bcast_time = 0.0;

    int* data = (int*) malloc(sizeof(int) * num_elements);
    for (int i = 0; i < num_trials; ++i) {
        // Time my_bcast
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time -= MPI_Wtime();
        my_bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time += MPI_Wtime();

        // Time MPI_Bcast
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time -= MPI_Wtime();
        MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time += MPI_Wtime();
    }

    if (world_rank == 0) {
        printf("Data size = %d, Trials = %d\n", num_elements * sizeof(int), num_trials);
        printf("Avg my_bacast time = %lf\n", total_my_bcast_time / num_trials);
        printf("Avg MPI_Bacast time = %lf\n", total_mpi_bcast_time / num_trials);
    }

    free(data);
    MPI_Finalize();

    return 0;
}