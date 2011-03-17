#include "cepmods.h"
#include <fftw3.h>

// Requires fftw3
// based on code by Hermenegild Arevalo in the lab of Natalia Tryanova
void df_mod(float* tdata, float* vdata, int ndata, MapStruct* dfs)
{
    if(ndata <= 0){
	all_abort("No data provided to df_mod.");
	exit(1);
    }

#ifdef DEBUG
    cerr << "Received count of ndata: " << ndata << endl;
#endif

    fftw_complex *in, *out;
    fftw_plan p;
    double dat[ndata];
    float timeframe = tdata[ndata-1] - tdata[0];
    float sampfreq = ndata/timeframe;

    in = (double (*)[2])fftw_malloc(sizeof(fftw_complex) * ndata);
    out = (double (*)[2])fftw_malloc(sizeof(fftw_complex) * ndata);
    if(in == NULL || out == NULL)
	all_abort("Failed to allocate arrays needed for FFT.");

    p = fftw_plan_dft_1d(ndata, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for(int tstep = 0; tstep < ndata; tstep++){
	in[tstep][0] = (double)vdata[tstep];
	in[tstep][1] = 0;
    }
    
    fftw_execute(p); // perform fft
    
    double maxamp = 0;
    int maxindex = 0;
    
    for(int j = 1; j < (ndata/2); j++){
	dat[j] = (out[j][0]*out[j][0]+out[j][1]*out[j][1])/ndata; //power spectrum

	if(dat[j] > maxamp)
	{
	    maxindex = j;
	    maxamp = dat[j];
	}
    }


#ifdef DEBUG
    cerr << "Max index = " << maxindex << ", Max amp = " << maxamp << ", Samp freq = " << sampfreq << endl;
#endif
    
    dfs->mapval = maxindex*sampfreq/ndata;

    free(in);
    free(out);
}
