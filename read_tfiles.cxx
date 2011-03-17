#include "cepmods.h"
#ifdef PARALLEL
#include "mpi.h"
#endif

void read_tfiles(char* basename, int start, int end, float** tdata, float** vdata, int nodect, int basenode)
{
    int numfiles;
    char* filename;
    float* vbuffer;
    
#ifdef PARALLEL
    double time;
    time = -MPI_Wtime();
#endif
    
    filename = new char[BASEBUFF];

    numfiles = end-start+1;

    // allocate a big ol' block of memory from start to end
//    *tdata = (float*)malloc(numfiles*sizeof(float));
    if(*tdata == NULL)
	all_abort("Failed to allocate tdata array in read_tfiles.");

    cerr << "Allocating vdata array of size " << numfiles * nodect << endl;
//    *vdata = (float*)malloc(numfiles*nodect*sizeof(float));
    if(*vdata == NULL)
	all_abort("Failed to allocate vdata array in read_tfiles.");

    // Load each file and put it in its slot.  We're returning tdata
    // and a block of vdata, where each row is from a tfile and each
    // col is a given time, so it lines up with the tdata
    for(int i = 0; i < numfiles; i++){
	read_tfile(filename, tdata, i, vbuffer, nodect, basenode);
	for(int j = 0; j < nodect; j++){
	    *(*vdata + j*numfiles + i) = *(vbuffer + j);
	}
	free(vbuffer);
    }
    cerr << "Numfiles: " << numfiles << endl;

#ifdef PARALLEL
    time += MPI_Wtime();
    cerr << "Spent " << time << " seconds reading tfiles. " << endl;
#endif
}
