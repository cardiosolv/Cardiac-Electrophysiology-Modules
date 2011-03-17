#include "cepmods.h"
#ifdef PARALLEL
#include "mpi.h"
#endif
void get_mpi_stats(int* procs, int* rank)
{
#ifdef PARALLEL
    MPI_Comm_size(MPI_COMM_WORLD, procs);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
#endif
#ifndef PARALLEL
    *procs = 1;
    *rank = 0;
#endif
}
