#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming at least 2 process for this task
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int number;
    if (world_rank == 0) {
        // If we are rank 0, set the number to -1 and send it to process 1
        number = -1;
        MPI_Send(&number,           /* data */
                 1,                 /* count */
                 MPI_INT,           /* datatype */
                 1,                 /* destination */
                 0,                 /* tag */
                 MPI_COMM_WORLD     /* communicator */
        );
    }else if (world_rank == 1) {
        MPI_Recv(&number,           /* data */
                 1,                 /* count */
                 MPI_INT,           /* datatype */
                 0,                 /* source */
                 0,                 /* tag */
                 MPI_COMM_WORLD,    /* communicator */
                 MPI_STATUS_IGNORE  /* status */
        );
        printf("Process 1 received number %d from process 0\n", number);
    }

    MPI_Finalize();

    return 0;
}