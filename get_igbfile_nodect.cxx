#include "cepmods.h"
#include "zlib.h"
#ifdef PARALLEL
#include "mpi.h"
#endif

void get_igbfile_nodect(char* filename, int* nodect)
{
#ifdef PARALLEL
    if(amRoot()){
#endif
    gzFile igbfile = gzopen(filename, "rb");
    if(igbfile == NULL){
	cerr << "File " << filename << " could not be opened for read. Bailing. " << endl;
	exit(1);
    }

    char header[IGBHEADERLENGTH];
    int returncheck;
    returncheck = gzread(igbfile, header, IGBHEADERLENGTH*sizeof(char));
    if(returncheck <= 0){
	cerr << "Error reading file " << filename << ". Bailing." << endl;
	exit(1);
    }

    char separators[] = " :\n";
    char* token;
    int x, y, z;
    x = y = z = 1;

    token = strtok(header, separators);
    while(x*y*z == 1 && token != NULL){
	if(!strcmp(token, "x"))
	    x = atoi(strtok(NULL, separators));
	if(!strcmp(token, "y"))
	    y = atoi(strtok(NULL, separators));
	if(!strcmp(token, "z"))
	    z = atoi(strtok(NULL, separators));
		    
	token = strtok(NULL, separators);
    }
    
    if(token == NULL && x*y*z == 1){
	cerr << "Dimensional data (node count) not found. Bailing." << endl;
	exit(1);
    }
    *nodect = (x*y*z);
#ifdef PARALLEL
    }
    MPI_Bcast(nodect, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif
}
