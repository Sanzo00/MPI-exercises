#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int comm_rank;
    union {
        float f;
        int i;
    } number;
} CommRankNumber;

// Gathers numbers for TMPI_RANK to process 0.
void* gather_numbers_to_root(void *number, MPI_Datatype datatype, MPI_Comm comm) {
    int comm_rank, comm_size;
    MPI_Comm_rank(comm, &comm_rank);
    MPI_Comm_size(comm, &comm_size);

    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    void* gathered_numbers;
    if (comm_rank == 0) {
        gathered_numbers = malloc(comm_size * datatype_size);
    }

    MPI_Gather(number, 1, datatype, gathered_numbers, 1, datatype, 0, comm);
    return gathered_numbers;
}

// A comparison function for sorting float CommRankNumber valuse
int compare_float_comm_rank_number(const void *a, const void *b) {
    CommRankNumber* comm_rank_number_a = (CommRankNumber*) a;
    CommRankNumber* comm_rank_number_b = (CommRankNumber*) b;
    if (comm_rank_number_a->number.f < comm_rank_number_b->number.f) {
        return -1;
    } else if (comm_rank_number_a->number.f > comm_rank_number_b->number.f) {
        return 1;
    } else {
        return 0;
    }
}

// A comparison function for sorting int CommRankNumber valuse
int compare_int_comm_rank_number(const void *a, const void *b) {
    CommRankNumber* comm_rank_number_a = (CommRankNumber*) a;
    CommRankNumber* comm_rank_number_b = (CommRankNumber*) b;
    if (comm_rank_number_a->number.i < comm_rank_number_b->number.i) {
        return -1;
    } else if (comm_rank_number_a->number.i > comm_rank_number_b->number.i) {
        return 1;
    } else {
        return 0;
    }
}

// returns an array of ordered by the process'rank, only executed on the root process
int* get_ranks(void *gathered_numbers, int gathered_number_count, MPI_Datatype datatype) {
    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    CommRankNumber *comm_rank_numbers = (CommRankNumber*) malloc(gathered_number_count * sizeof(CommRankNumber));
    for (int i = 0; i < gathered_number_count; ++i) {
        comm_rank_numbers[i].comm_rank = i;
        memcpy(&(comm_rank_numbers[i].number), gathered_numbers + datatype_size * i, datatype_size);
    }

    // sort the comm rank numbers based on the datatype
    if (datatype == MPI_FLOAT) {
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_float_comm_rank_number);
    }else {
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_int_comm_rank_number);
    }

    int *ranks = (int*) malloc(sizeof(int) * gathered_number_count);
    for (int i = 0; i < gathered_number_count; ++i) {
        ranks[comm_rank_numbers[i].comm_rank] = i;
    }

    free(comm_rank_numbers);
    return ranks;
}

int TMPI_Rank(void *send_data, void *recv_data, MPI_Datatype datatype, MPI_Comm comm) {
    if (datatype != MPI_INT && datatype != MPI_FLOAT) {
        return MPI_ERR_TYPE;
    }

    int comm_size, comm_rank;
    MPI_Comm_rank(comm, &comm_rank);
    MPI_Comm_size(comm, &comm_size);

    void *gathered_numbers = gather_numbers_to_root(send_data, datatype, comm);

    int *ranks = NULL;
    if (comm_rank == 0) {
        ranks = get_ranks(gathered_numbers, comm_size, datatype);
    }

    MPI_Scatter(ranks, 1, MPI_INT, recv_data, 1, MPI_INT, 0, comm);

    if (comm_rank == 0) {
        free(gathered_numbers);
        free(ranks);
    }
}
 
int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Seed the random number generator to get different results each time
    srand(time(NULL) * world_rank);

    float rand_num = rand() / (float)RAND_MAX;
    int rank;
    TMPI_Rank(&rand_num, &rank, MPI_FLOAT, MPI_COMM_WORLD);

    printf("Rank for %f on process %d - %d\n", rand_num, world_rank, rank);

    MPI_Finalize();

    return 0;
}