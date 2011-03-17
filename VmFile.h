#include "cepmods.h"
#ifndef _VMFILE_H_
#define _VMFILE_H_
#define ByteSwap5(x) ByteSwap((unsigned char *) &x,sizeof(x))

class VmFile{
public:
    VmFile();
    void open(char* filename, int tnum = 0); // open IGB file and read header into vars or read tfile of number tnum for info
    void close();                  // close IGB file
    void get_steps(int start, int end, float* tdata, float* vdata); // get a block of timesteps from start to end. YOU MUST ALLOCATE the data arrays
    void get_next_step(float* time, float* vdata); // gets next timestep to be read
    void get_previous_step(float* time, float* vdata); // gets previous timestep from the one just read
    void set_time(int time); // set the time step number (i.e. first step is 0, second is 1, etc)
    int  get_time(); // find out at which time step the next read will take place
    void get_step(int time, float* vdata); // get data for one time step at step "time"
    int get_nodect(); // how many nodes *total* in the model
    int get_my_startnode(); // what's the first node that this process will read?
    int get_my_block(); // how many nodes will this process read?
    int get_igb_timesteps(); // how many timesteps does the IGB file claim to have in the header?
    int get_true_timesteps(); // how many timesteps are actually in the file?
    void set_tfile_mode(); // set mode = 1 (for tfiles)
    void set_igbfile_mode(); // set mode = 0 (for IGB files, which is the default)
    void set_startnode(int startnode);
    void set_block(int block);
private:
    int blocksize(int start, int end);
    unsigned long int time2seek(int time);
    int seek2time(ios::pos_type seek);
    
    // ifstream igbfile;
    FILE *igbfile;
    bool swapbytes;
    int headersize;
    float voltagescale;
    int mode;
    int tfile_currtime;
    char* basename;
    int nodect;
    int my_startnode;
    int my_block;
    
    int x;
    int y;
    int z;
    int t;
    int dim_x;
    int dim_y;
    int dim_z;
    int dim_t;
    char* unites_x;
    char* unites_y;
    char* unites_z;
    char* unites_t;
    char* units;
    int datsize;
    char* endianness;
    float fac_t;    // could be an integer too
    float facteur;  // could be an integer too
    float tzero;
};
#endif
