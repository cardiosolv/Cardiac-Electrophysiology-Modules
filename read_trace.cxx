#include "cepmods.h"

void read_trace(char* filename, float* tdata, float* vdata, int* outsize, float start, float end)
{
    float time;
    float vm;
    int linect;
    
    ifstream tracefile(filename, ios::in);
    if(!tracefile){
	cerr << "File " << filename << " could not be opened." << endl;
	exit(1);
    }

    linect = 0;
    while(tracefile >> time >> vm){
	if(time >= start && time <= end){
	    *(tdata + linect) = time;
	    *(vdata + linect) = vm;
	    linect++;
	}
    } // while loop
    *outsize = linect;
    tracefile.close();
} // read_trace

