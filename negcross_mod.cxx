// Brock M. Tice
// Negative threshold crossing detection algorithm
// 

#include "cepmods.h"

void negcross_mod(float* tdata, float* vdata, int ndata, MapStruct* crossdata, float threshold)
{
    int i = 0;

    if(crossdata->lastvm == -1){
	crossdata->lastvm = vdata[0];
	crossdata->lasttime = tdata[0];
	i++;
    }


    while(i < ndata && crossdata->mapval == -1){
	if(vdata[i] < threshold && crossdata->lastvm > threshold){
	    crossdata->mapval = tdata[i];
	}else{
	    crossdata->lasttime = tdata[i];
	    crossdata->lastvm = vdata[i];
	    i++;
	}
    }

}
