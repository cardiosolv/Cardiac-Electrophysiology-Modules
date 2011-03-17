#include "cepmods.h"

void amap_mod(int nfiles, float* tdata, float* vdata, int nodect, MapStruct* activations, float mindvdt, float minactvm)
{

#ifdef DEBUG
    cerr << "Mapping " << nodect << " nodes from " << nfiles << " files."<< endl;
#endif

    for(int node = 0; node < nodect; node++){
#ifdef DEBUG
//	cerr << "Passing node " << node << " to act_mod(). " << endl;
#endif
	act_mod(tdata, &vdata[node*nfiles], nfiles, &activations[node], mindvdt, minactvm);
#ifdef DEBUG
//	cerr << (*activations+node)->time << endl;
#endif
    }
}
