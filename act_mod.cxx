/* 
 * Brock M. Tice
 * Activation Module
 * 
 *  act_mod.cxx - finds an activation in a time/voltage array
 *
 */

#include "cepmods.h"

/* data is a 2D array arranged in columns */
/* column 1 is the time, column 2 is vm */
void act_mod(float* tdata, float* vdata, int ndata, MapStruct* actdata, float mindvdt, float minvm)
{
    
    float dvdt;
    int i;

#ifdef DEBUG
//    cerr << "Activation data: "<< endl;
#endif
    i = 0;
    if(actdata->lastvm == -1){
	actdata->lastvm = vdata[0];
	actdata->lasttime = tdata[0];
	actdata->maxdvdt = -1000;
	actdata->maxtime = -1;
	actdata->upstroke = 0;
	i++;
    }

    while(i < ndata && actdata->mapval == -1){    
	dvdt = (vdata[i] - actdata->lastvm)/(tdata[i] - actdata->lasttime);
	if(actdata->upstroke == 0 && dvdt > mindvdt && dvdt > actdata->maxdvdt && vdata[i] > minvm){

	    actdata->maxdvdt = dvdt;
	    actdata->maxtime = tdata[i];
	    actdata->upstroke = 1;
	}

	if(actdata->upstroke == 1 && dvdt < mindvdt && actdata->maxdvdt > mindvdt){
	    actdata->mapval = actdata->maxtime;
	    actdata->upstroke = -1;
	}

	actdata->lastvm = vdata[i];
	actdata->lasttime = tdata[i];
	i++;
    }
    
}

