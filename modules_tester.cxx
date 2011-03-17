/*
 * Brock M. Tice
 * Modules Tester
 * 
 *  modules_tester.h - utility to go test the modules
 * 
 */

#include "cepmods.h"
#include <GetOpt.h>
#include <cstdlib>

using namespace std;

#define DEFAULTSIZE 1000000

void berr(string message);

int main(int argc, char** argv){
    char* inputfile = NULL;
    char* basename = NULL;
    char* igbfile = NULL;
    int start = 0;
    int end = 0;
    ActData* my_actdata = new ActData;
    ActData* my_repoldata = new ActData;
    ActData* my_dfdata = new ActData;
    float* my_tdata = new float[DEFAULTSIZE];
    float* my_vdata = new float[DEFAULTSIZE];
    int my_data_ct;
    float mindvdt = 1;
    float minvm = -0.010;
    string usage = "Usage: modules_tester -f <trace file> -b <basename> -s <start tfile> -e <end tfile> -i <igbfile>";
    int optc;

    while ((optc = getopt(argc, argv, "b:f:s:e:i:")) != -1)
	switch(optc){
	case 'f': inputfile = optarg; break;
	case 'b': basename = optarg; break;
	case 's': start = atoi(optarg); break;
	case 'e': end = atoi(optarg); break;
	case 'i': igbfile = optarg; break;
	default:
	    berr(usage);
	}

    if(inputfile == NULL || basename == NULL)
	berr(usage);
    
//     read_trace(inputfile, my_tdata, my_vdata, &my_data_ct, DEFAULTSIZE);
//     act_mod(my_tdata, my_vdata, my_data_ct, my_actdata, mindvdt, minvm);
//     cout << "Sample is of length " << my_data_ct << ". "<< endl;
//     df_mod(my_tdata, my_vdata, my_data_ct, my_dfdata);
//     cout << "Dominant frequency is " << my_dfdata->value << "Hz, found at " << my_dfdata->time  << endl;
//     if(my_actdata->found)
//  	repol_mod(my_tdata, my_vdata, my_data_ct, my_actdata, my_repoldata, 0.9);

//      if(my_actdata->found){
//  	cout << "Found activation at time " << my_actdata->time << " and potential " << my_actdata->value << endl;
//  	if(my_repoldata->found)
//  	    cout << "Found repolarization at time " << my_repoldata->time << " and potential " << my_repoldata->value << endl;
//      }else{
//  	cout << "No activation found." << endl;
//      }

    float* vdata;
    float* tdata;
    int nodect;
    ActData* activations;
    ActData* repols;
    ActData* apds;
    ActData* dfs;
    char amapfile[] = "amap_test.d0";
    char rmapfile[] = "rmap_test.d0";
    char apdmapfile[] = "apdmap_test.d0";
    char dfmapfile[] = "dfmap_test.d0";
    
//    read_tfiles(basename, start, end, &tdata, &vdata, &nodect);
    
// #ifdef DEBUG
//     cerr << "Main: found " << nodect << " nodes." << endl;
// #endif
    
    read_igbfile(igbfile, &tdata, &start, &end, &vdata, &nodect);
    print_trace(tdata, (vdata + 5*(end-start+1)), (end-start+1));
//     amap_mod((end-start+1), &tdata, &vdata, nodect, &activations);
//     rmap_mod((end-start+1), &tdata, &vdata, nodect, &activations, &repols, 0.8);
//     apdmap_mod(nodect, &activations, &repols, &apds);
//     dfmap_mod((end-start+1), &tdata, &vdata, nodect, &dfs);
//     write_datfile(amapfile, &activations, nodect, tdata[0], 0.01);
//     write_datfile(rmapfile, &repols, nodect, tdata[0], 0.01);
//     write_datfile(apdmapfile, &apds, nodect, *tdata, 0.01);
//     write_datfile(dfmapfile, &dfs, nodect, *tdata, 0.001);
}

void berr(string message){
    cerr << message.c_str() << endl;
    exit(1);
}
