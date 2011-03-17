#include "cepmods.h"

void freqmap_mod(int nodect, MapStruct* periods, MapStruct* freqs)
{
    for(int node = 0; node < nodect; node++){
	if(periods[node].mapval != -1 && periods[node].mapval != 0)
	    freqs[node].mapval = 1/periods[node].mapval;
	else
	    freqs[node].mapval = -1;
    }
}
