#include "cepmods.h"

void periodmap_mod(int nodect, MapStruct* acts1, MapStruct* acts2, MapStruct* periods)
{
    for(int node = 0; node < nodect; node++){
	if(acts1[node].mapval != -1 && acts2[node].mapval != -1)
	    periods[node].mapval = acts2[node].mapval - acts1[node].mapval;
	else
	    periods[node].mapval = -1;
    }
}
