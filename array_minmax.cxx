// Brock M. Tice
// Array Min/Max calculator
//
//  array_minmax.cxx - computes the minimum and maximum values in an
//  array and the indexes at which they are found
//

void array_minmax(float* data, int ndata, float* minval, int* mindex, float* maxval, int* maxdex)
{
    *minval = 999999.0;
    *maxval = -(*minval);

    for(int i = 0; i < ndata; i++){
	if(*(data + i) < *minval){
	    *minval = *(data + i);
	    *mindex = i;
	}

	if(*(data + i) > *maxval){
	    *maxval = *(data + i);
	    *maxdex = i;
	}
    }
}
