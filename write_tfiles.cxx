#include "cepmods.h"

void write_tfiles(char* basename, float* tdata, float* vdata, int startnum, int timesteps, int nodes, bool threecols)
{
    char tfile[BASEBUFF];
    MapStruct* vbuffer;
    initialize_map(&vbuffer, nodes);
    if(vbuffer == NULL)
	all_abort("Failed to allocate vbuffer in write_tfiles.");

    for(int time = 0; time < timesteps; time++){
	for(int node = 0; node < nodes; node++){
	    vbuffer[node].mapval = vdata[node*timesteps + time];
	}
	sprintf(tfile, "%s.t%d", basename, startnum+time);
	write_datfile(tfile, vbuffer, nodes, tdata[time], -1, 1, threecols);
    }// timesteps

    free(vbuffer);
}
