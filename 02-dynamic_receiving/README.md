```cpp
typedef struct _MPI_Status {
  int count;
  int cancelled;
  int MPI_SOURCE;
  int MPI_TAG;
  int MPI_ERROR;
} MPI_Status, *PMPI_Status;

// Gets the number of "top level" elements
int MPI_Get_count(const MPI_Status* status, MPI_Datatype datatype, int* count)

// Blocking test for a message
int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status* status)
```