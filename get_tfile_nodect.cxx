#include "cepmods.h"
#ifdef PARALLEL
#include "mpi.h"
#endif

void get_tfile_nodect(char* basename, int filenum, int* nodect)
{
#ifdef PARALLEL
    if(amRoot()){
#endif	
	int counter = 0;
	char* line;
	//float val1, val2, val3;
	char filename[BASEBUFF];
	
	sprintf(filename, "%s.t%d", basename, filenum);
	line = new char[MAXLINESIZE];
	
	//ifstream tfile(filename, ios::in);
	FILE* tfile;
	tfile = fopen(filename, "r");

	//if(!tfile){
	if(tfile == NULL){
	    cerr << "File " << filename << " could not be opened." << endl;
	    exit(1);
	}
    
	// pull off the first line, should be t = whatever
	//tfile.getline(line, MAXLINESIZE);
	fgets(line, MAXLINESIZE, tfile);

	// read three values per line
	//while(tfile >> val1 >> val2 >> val3){
	while(!feof(tfile)){
	    fgets(line, MAXLINESIZE, tfile);
	    counter++;
	}
	counter--;

	//tfile.close();
	fclose(tfile);
	*nodect = counter;
#ifdef PARALLEL
    }
    MPI_Bcast(nodect, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif
}
