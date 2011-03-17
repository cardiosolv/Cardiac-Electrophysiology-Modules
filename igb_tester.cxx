#include "cepmods.h"
#include "IgbFile.h"

int main(int argc, char** argv)
{
    IgbFile igb;
    float* vdata;
    float* tdata;
    int nodect;
    int timesteps;

    igb.open(&argv[1][0]);
    nodect = igb.get_nodect();

    if(amRoot()){
	cerr << "Data from " << nodect << " nodes in file." << endl;
	cerr << "Header inidicates " << igb.get_igb_timesteps() << " time steps." << endl;
	cerr << "Seek test reveals " << igb.get_true_timesteps() << " time steps." << endl;
    }

    vdata = new float[nodect];
    tdata = new float;
    
    timesteps = igb.get_true_timesteps();

    for(int step = 0; step < igb.get_true_timesteps(); step++){
	igb.get_next_step(tdata, vdata);
	if(amRoot()){
	    cout << *tdata << " ";
	    for(int i = 0; i < 5; i++){
		cout << vdata[i] << " ";
	    }
	    cout << endl;
	}
    }

//     igb.get_steps(0, 10, tdata, vdata);
//     for(int i = 0; i < 10; i++){
// 	cout << tdata[i] << " " << *(vdata + nodect*i + 25) << endl;
//     }
    igb.close();
    return 0;
}
