#include "cepmods.h"

void dfmap_mod(int nfiles, float* tdata, float* vdata, int nodect, MapStruct* dfs)
{
//    *dfs = (float*)malloc(nodect*sizeof(float));
//    if(*dfs == NULL)
//	all_abort("Failed to allocate memory for dfmap_mod.");

    for(int node = 0; node < nodect; node++)
	df_mod(tdata, &vdata[node*nfiles], nfiles, &dfs[node]);
}
