#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming 2 processes for this task
    if (world_size != 2) {
        fprintf(stderr, "World size must be two for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int count = 0;
    int limit = 10;
    int partner = (world_rank + 1) % 2;

    while (count < limit) {
        if (world_rank == count % 2) {
            count++;
            MPI_Send(&count, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            printf("%d sent %d to %d\n", world_rank, count, partner);
        }else {
            MPI_Recv(&count, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%d received %d from %d\n", world_rank, count, partner);
        }
    }

    MPI_Finalize();

    return 0;
}