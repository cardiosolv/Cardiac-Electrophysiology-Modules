// Brock M. Tice
// Repolarization Module
// 
//  repol_mod.cxx - find repolarization time using a calculated
//  threshold and a given percent repol
//

#include "cepmods.h"

void repol_mod(float* tdata, float* vdata, int ndata, MapStruct* actdata, MapStruct* repoldata, float recovery_vm)
{
    //int mindex, maxdex;
    //float minval, maxval, repolthresh;
    float repolthresh;

//    array_minmax(vdata, ndata, &minval, &mindex, &maxval, &maxdex);
    
//    repolthresh = (1-recoverypct) * (maxval-minval) + minval;
    repolthresh = recovery_vm;

//    *repoldata = 0; // should be supplied as -1 on first go-round
    int i = 0;
//    bool found = 0;


    if(repoldata->lastvm == -1){
	repoldata->lastvm = vdata[0];
	repoldata->lasttime = tdata[0];
	i++;
    }

    // don't do anything unless we have an activation time but no repol time
    if(actdata->mapval != -1 && repoldata->mapval == -1){
	// start from activation time
	while(tdata[i] < actdata->mapval && i < ndata){
	    repoldata->lasttime = tdata[i];
	    repoldata->lastvm = vdata[i];
	    i++;
	}

	while(i < ndata && repoldata->mapval == -1){
//	    if(vdata[i] < repolthresh && repoldata->lastvm > repolthresh){
	    if(vdata[i] < repolthresh){
		repoldata->mapval = tdata[i];
#ifdef DEBUG
		cerr << "Repol detected at time " << tdata[i] << " and voltage " << vdata[i] << " with threshold " << repolthresh << "." << endl;
#endif
	    }else{
		repoldata->lasttime = tdata[i];
		repoldata->lastvm = vdata[i];
		i++;
	    }
	}
#ifdef DEBUG
	if(i == ndata && repoldata->mapval == -1){
	    cerr << "Failed to find repolarization for this node. Last values were time = " << repoldata->lasttime << " and Vm = " << repoldata->lastvm << "." << endl;
	}
#endif       
    }
#ifdef DEBUG
else{
	if(actdata->mapval == -1){
	    cerr << "No activation data found for this node." << endl;
	}
	if(repoldata->mapval != -1){
	    cerr << "Repolarization already set for this node." << endl;
	}
}
#endif
// THIS SECTION REWRITTEN TO BE MORE EFFICIENT
//     lastval = *(vdata);
//     while(i < ndata && !found){
// 	if(*(tdata + i) < *actdata){
// 	    lastval = *(vdata + i);
// 	    i++;
// 	    continue;
// 	}

// 	if(*(vdata + i) < repolthresh && lastval > repolthresh){
// 	    found = 1;
// 	    *repoldata = *(tdata + i);
// 	}else{
// 	    lastval = *(vdata + i);
// 	    i ++;
// 	}
//     }

}
