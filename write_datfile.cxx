#include "cepmods.h"
#ifdef PARALLEL
#include "mpi.h"
#endif

void write_datfile(char* filename, MapStruct* actdata, int datact, float headertime, float iso_step, bool header, bool threecols)
{

    int append = 0;
#ifdef PARALLEL
    MPI_Status status;
    int okay = 1;
    int procs;
    int my_rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    // wait for the okay to write
    if(!amRoot()){
	MPI_Recv(&okay, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
#ifdef DEBUG
	cerr << "Process " << my_rank << ": Received okay to write. Writing." << endl;
#endif
	append = 1;
#ifdef DEBUG
    }else{
	cerr << "Process " << my_rank << ": Writing first." << endl;
#endif
    }

#endif

    ofstream datfile;
    if(append)
	datfile.open(filename, ios::app);
    else
	datfile.open(filename, ios::out);

    if(!datfile)
	all_abort("Could not open dat file for writing.");


    if(header && !append)
	datfile << "t = " << headertime << endl;

    float current_iso;
    float tmp;

    for(int line = 0; line < datact; line++){
	tmp = actdata[line].mapval;
	if(tmp == -1)
	    tmp = 0;
	if(iso_step > 0){
	    current_iso = 0;
	    
	    while(tmp > current_iso && tmp != 0){
		current_iso += iso_step;
	    }
	    tmp = current_iso;
	}
	if(threecols)
	    datfile << "0\t" << tmp << "\t";
	datfile << tmp << "\n";
    }
    datfile.flush();
    datfile.close();
    
#ifdef PARALLEL
    //let root know we're done writing
    if(!amRoot())
	MPI_Send(&okay, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    // let the other processes write in order
    if(amRoot()){
	for(int proc = 1; proc < procs; proc++){
	    MPI_Send(&okay, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
	    MPI_Recv(&okay, 1, MPI_INT, proc, 0, MPI_COMM_WORLD, &status);
	}
    }
    MPI_Barrier(MPI_COMM_WORLD);
#endif

}
    
