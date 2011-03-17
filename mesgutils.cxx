#include "cepmods.h"

#ifdef PARALLEL
#include "mpi.h"
#endif

void all_abort(const char* message)
{
#ifdef PARALLEL
    if(amRoot()){
#endif
	cerr << endl;
	cerr << message << " Bailing. " << endl;;
#ifdef PARALLEL
    }
    MPI_Abort(MPI_COMM_WORLD,1);
#endif

#ifndef MPI
    exit(1);
#endif
}

bool amRoot()
{
    bool retval = 1;

#ifdef PARALLEL
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if(my_rank != 0)
	retval = 0;
#endif
    return retval;
}

void warn(const char* warnmsg)
{
#ifdef PARALLEL
    if(amRoot()){
#endif
	cerr << endl;
	cerr << "WARNING: " << warnmsg << endl;
#ifdef PARALLEL
    }
#endif
}

void passert(const bool test)
{
    if(test == false)
	all_abort("Parallel assert failed.");
}
