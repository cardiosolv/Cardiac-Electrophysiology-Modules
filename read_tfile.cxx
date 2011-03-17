#include "cepmods.h"

void read_tfile(char* filename, float* time, int toffset, float* vdata, int datact, int basenode)
{
    float tmp1, tmp2, vm;
    char *linebuffer = new char [MAXLINESIZE];

    ifstream tfile(filename, ios::in);
    if(!tfile){
	cerr << "File " << filename << " could not be opened for read." << endl;
	exit(1);
    }

    // get time from first line
    tfile.getline(linebuffer, MAXLINESIZE);
    
    time[toffset] = tline_extract_time(linebuffer);
    
    //*vdata = (float*)calloc(datact, sizeof(float));
    
    for(int node = 0; node < basenode; node++){
	tfile.getline(linebuffer, MAXLINESIZE);
    }
    
    int i = 0;
    while(tfile >> tmp1 >> tmp2 >> vm && i < datact){
	vdata[i] = vm;
	i++;
    }
    tfile.close();
}
