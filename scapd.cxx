#include "cepmods.h"
#define DEFAULTSIZE 1000000
int main(int argc, char** argv){
    char* filename = NULL;
    float start = 0;
    float end = 0;
    float pct = 0;
    int my_data_ct = 0;
    float* my_tdata = new float[DEFAULTSIZE];
    float* my_vdata = new float[DEFAULTSIZE];
    ActData* my_actdata = new ActData;
    ActData* my_repoldata = new ActData;

    if(argc < 5){
	cerr << "Usage: scapd filename start end repolpct" << endl;
	exit(1);
    }
	

    filename = argv[1];
    start = atof(argv[2]);
    end = atof(argv[3]);
    pct = atof(argv[4]);

    read_trace(filename, my_tdata, my_vdata, &my_data_ct, start, end);
    act_mod(my_tdata, my_vdata, my_data_ct, my_actdata, 10, -0.070);
    repol_mod(my_tdata, my_vdata, my_data_ct, my_actdata, my_repoldata, pct*0.01);
    cout << my_repoldata->time - my_actdata->time << endl;
}
