/* 
 * Brock M. Tice
 * Activation Module
 * 
 *  di_mod.cxx - finds an activation in a time/voltage array, starting at a specified previous repolarization time
 *
 */

#include "cepmods.h"

/* data is a 2D array arranged in columns */
/* column 1 is the time, column 2 is vm */
void di_mod(float* tdata, 
	    float* vdata, 
	    int ndata, 
	    MapStruct* repoldata, 
	    MapStruct* actdata, 
	    MapStruct* didata,
	    float mindvdt, 
	    float minvm)
{
    
    float dvdt;
    int i;
//    float maxdvdt = -10000;
//    float maxtime = -1;
//    float upstroke = 1;

//    bool found = 0;

//    *actdata = 0; // actdata should be supplied as -1 on the first go-round

#ifdef DEBUG
//    cerr << "Activation data: "<< endl;
#endif
    i = 0;
    if(actdata->lastvm == -1){
	actdata->lastvm = vdata[0];
	actdata->lasttime = tdata[0];
	actdata->maxdvdt = -1000;
	actdata->maxtime = -1;
	actdata->upstroke = 1;
	i++;
    }
    

    while(i < ndata && actdata->mapval == -1){

	// skip until we get to the time of the last repolarization
	if(tdata[i] < repoldata->mapval || repoldata->mapval == -1){
	    i++;
	    continue;
	}

	dvdt = (vdata[i] - actdata->lastvm)/(tdata[i] - actdata->lasttime);
	if(actdata->upstroke && dvdt > mindvdt && dvdt > actdata->maxdvdt && vdata[i] > minvm){
	    actdata->maxdvdt = dvdt;
	    actdata->maxtime = tdata[i];
//	    cerr << "Max dV/dt = " << actdata->maxdvdt << " @ " << actdata->maxtime << endl;
	}
	
	if(dvdt < actdata->maxdvdt && actdata->upstroke && actdata->maxtime != -1)
	    if(actdata->upstroke < 3){
		actdata->upstroke++;
//		cerr << "Actdata->Upstroke count " << actdata->upstroke << endl;
	    }else{
		actdata->upstroke = 0;
		actdata->mapval = tdata[i];
		didata->mapval = actdata->mapval - repoldata->mapval;
//		cerr << "Found activation at time " << actdata->mapval << endl;
	    }
	actdata->lastvm = vdata[i];
	actdata->lasttime = tdata[i];
	i++;
    }
}

