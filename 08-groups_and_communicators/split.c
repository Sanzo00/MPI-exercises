#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Comm row_comm;
    int color = world_rank / 4;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    printf("World rank/size: %d/%d\tRow rank/size: %d/%d\n",
           world_rank, world_size, row_rank, row_size);
    
    MPI_Finalize();

    return 0;
}