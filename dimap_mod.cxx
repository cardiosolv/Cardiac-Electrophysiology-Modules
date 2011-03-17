#include "cepmods.h"

void dimap_mod(int nfiles, 
	       float* tdata, 
	       float* vdata, 
	       int nodect, 
	       MapStruct* repols,
	       MapStruct* activations,
	       MapStruct* dis)
{

#ifdef DEBUG
    cerr << "Mapping " << nodect << " nodes from " << nfiles << " files."<< endl;
#endif

    for(int node = 0; node < nodect; node++){
	di_mod(tdata, &vdata[node*nfiles], nfiles, &repols[node], &activations[node], &dis[node], 1, -0.070);
    }
}
