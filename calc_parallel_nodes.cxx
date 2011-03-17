#include "cepmods.h"
#ifdef PARALLEL
#include "mpi.h"
#endif

void calc_parallel_nodes(int nodect, int* my_startnode, int* my_block)
{
#ifdef PARALLEL
    int remainder, base_block, my_rank, procs;
    get_mpi_stats(&procs, &my_rank);
//    cerr << procs << " " << my_rank << " " << nodect << endl;
    base_block = nodect/procs;
//    cerr << base_block << endl;
    remainder = nodect % procs;
//    cerr << "Base: " << base_block << " Remainder: " << remainder << endl;

    *my_block = base_block;
//    cerr << *my_block << endl;

    if(my_rank < remainder){
	(*my_block)++;
	*my_startnode = my_rank * (*my_block);
    }else{
	*my_startnode = remainder * ((*my_block)+1) + base_block * (my_rank - remainder);
    }
//    cerr << "Process " << my_rank << " to read from " << *my_startnode << " to " << (*my_block)+(*my_startnode) << endl;
#endif PARALLEL

#ifndef PARALLEL
    *my_block = nodect;
    *my_startnode = 0;
#endif
}
