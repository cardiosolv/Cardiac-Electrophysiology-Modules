#include "cepmods.h"

void apdmap_mod(int nodect, MapStruct* activations, MapStruct* repols, MapStruct* apds)
{
//    *apds = (float*)malloc(nodect*sizeof(float));
//    if(*apds == NULL)
//	all_abort("Couldn't allocate memory for apdmap_mod.");

    for(int node = 0; node < nodect; node++){
	if(repols[node].mapval != -1 && activations[node].mapval != -1)
	    apds[node].mapval = repols[node].mapval - activations[node].mapval;
	else
	    apds[node].mapval = -1;
    }
}
