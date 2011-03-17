#include "cepmods.h"

void rmap_mod(int nfiles, float* tdata, float* vdata, int nodect, MapStruct* activations, MapStruct* repols, float recovery_vm)
{
//    *repols = (float*)malloc(nodect*sizeof(float));
//    if(*repols == NULL)
//	all_abort("Failed to allocate repols array in rmap_mod.");

    for(int node = 0; node < nodect; node++){
	repol_mod(tdata, (vdata+node*nfiles), nfiles, &activations[node], &repols[node], recovery_vm);


    }
}
