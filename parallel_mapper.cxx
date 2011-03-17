#include "cepmods.h"
#include "VmFile.h"
#ifndef AIX
#include <getopt.h>
#endif
#ifdef AIX
#include <unistd.h>
#endif
#include <cstdlib>

#ifdef PARALLEL
#include "mpi.h"
#endif

using namespace std;

void usage();

int main(int argc, char** argv)
{
    char* basename = NULL;
    char* actmap = NULL;
    char* repolmap = NULL;
    char* apdmap = NULL;
    char* triangmap = NULL;
    char* freqmap = NULL;
    char* igbfile = NULL;
    int start = 0;
    int end = 0;
    int timeblock = 1;
    char* toutname = NULL;
    int optc;
    float isochrone = 0.001;
    float repol_vm = -0.070;
    float repol30_vm = -1;
    float repol90_vm = -1;
    float mindvdt = 10;
    float act_vm = -0.070;
    bool threecols = false;

#ifndef AIX
    static struct option long_options[] = 
    {
	{"help", no_argument, 0, 'h'},
	{"name", required_argument, 0, 'n'},
	{"tstart", required_argument, 0, 's'},
	{"tend", required_argument, 0, 'e'},
	{"actmap", required_argument, 0, 'a'},
	{"repolmap", required_argument, 0, 'r'},
	{"apdmap", required_argument, 0, 'd'},
        {"triangmap", required_argument, 0, 'c'},
	{"freqmap", required_argument, 0, 'f'},
	{"isochrone", required_argument, 0, 'i'},
	{"repol_vm", required_argument, 0, 'p'},
        {"repol30_vm", required_argument, 0, 'j'},
        {"repol90_vm", required_argument, 0, 'k'},
        {"mindvdt", required_argument, 0, 'm'},
        {"act_vm", required_argument, 0, 'v'},
	{"igbfile", required_argument, 0, 'g'},
	{"toutname", required_argument, 0, 't'},
	{"timeblock", required_argument, 0, 'b'},
        {"threecols", no_argument, 0, '3'},
    };
    int option_index = 0;
#endif

#ifdef PARALLEL
    int my_rank;
    int procs;
    MPI_Init(&argc, &argv);
    get_mpi_stats(&procs, &my_rank);
    if(amRoot())
	cerr << "Using " << procs << " processes." << endl;
#endif

#ifndef AIX
    while((optc = getopt_long(argc, argv, "n:g:s:e:a:r:m:v:d:c:f:i:h:p:j:k:t:b:3", long_options, &option_index)) != -1){
#endif
#ifdef AIX
    while(((optc) = getopt(argc, argv, "n:g:s:e:a:r:m:v:d:c:f:i:h:p:j:k:t:b:3")) != -1){
#endif
	switch(optc){
	case 'n': basename = optarg; break;
	case 's': start = atoi(optarg); break;
	case 'e': end = atoi(optarg); break;
	case 'a': actmap = optarg; break;
	case 'r': repolmap = optarg; break;
	case 'd': apdmap = optarg; break;
        case 'c': triangmap = optarg; break;
	case 'f': freqmap = optarg; break;
	case 'i': isochrone = atof(optarg); break;
	case 'p': repol_vm = atof(optarg); break;
        case 'j': repol30_vm = atof(optarg); break;
        case 'k': repol90_vm = atof(optarg); break;
        case 'm': mindvdt = atof(optarg); break;
        case 'v': act_vm = atof(optarg); break;
	case 'g': igbfile = optarg; break;
	case 't': toutname = optarg; break;
	case 'b': timeblock = atoi(optarg); break;
        case '3': threecols = true; break;
	case 'h': usage(); exit(0); break;
	default:
	    usage();
	}
    }
    
    // do some sanity checks
    if(basename == NULL && igbfile == NULL){
	usage();
	all_abort("Must specify either a base name for tfiles, or an IGB file.");
    }
    
    if(igbfile != NULL && (start != 0 || end != 0)){
	warn("You specified an IGB file with a start and/or end time. These times will be used as offsets in terms of timesteps for making maps, not as raw times.");
    }
    
    if( end == 0 ){
	usage();
	all_abort("Must specify a non-zero end time.");
    }

    if(basename != NULL && igbfile !=NULL){
	all_abort("Don't know what to do with both tfiles and an IGB file.");
    }
    
    if(basename != NULL && end == 0){
	usage();
	all_abort("We assume start = 0 if not specified, but you have to supply an end time when using tfiles.");
    }
    
    if(start > end){
	all_abort("Times backwards.");
    }
    
    if(repolmap != NULL && actmap == NULL)
	warn("You specified a repol map without an activation map. We will have to calculate one anyway, but it won't be output. Continuing... ");
    if(isochrone > 1)
	warn("Isochone time units are seconds, NOT milliseconds. Are your units correct?");
    
    if(actmap == NULL && repolmap == NULL && apdmap == NULL && freqmap == NULL && toutname == NULL && triangmap == NULL){
	usage();
	all_abort("Stubbornly refusing to do a bunch of work without an output file specified.");
    }
    
    if(repol_vm < -0.200){
	all_abort("A repol Vm less than -200 mV is nonsense.");
    }

    if(mindvdt > 10 || mindvdt < 1){
        warn("It is recommended that you set a minimum dV/dt between 1 and 10 V/s. You might have to go lower for smoothed data.");
    }

    if(timeblock < 1)
	all_abort("Block size less than one is nonsense.");

    if(timeblock != 1 && freqmap != NULL)
	warn("Time block size will be ignored because FFT is being used, and it requires the whole time series.");

    if(triangmap != NULL && repol30_vm == -1 && repol90_vm == -1)
      all_abort("Triangulation maps require repol30_vm (-j) and repol90_vm (-k) to be specified");
    
    // Meat
    float* vdata;
    float* tdata;
    int nodect = 0;
    MapStruct* activations = NULL;
    MapStruct* repols = NULL;
    MapStruct* apds = NULL;
    MapStruct* triangs = NULL;
    MapStruct* repol30 = NULL;
    MapStruct* repol90 = NULL;
    MapStruct* dfs = NULL;
    int timesteps;
    int timestep;
    VmFile vmfile;
    int my_block, my_startnode;
    int main_steps;
    int remainder;
    int total_steps;

    if(basename != NULL){
	vmfile.set_tfile_mode();
	vmfile.open(basename, start);
    }
    if(igbfile != NULL){
	vmfile.set_igbfile_mode();
	vmfile.open(igbfile);
    }

    nodect = vmfile.get_nodect();
    my_startnode = vmfile.get_my_startnode();
    my_block = vmfile.get_my_block();
        
    // Here starts the real processing
    // if using FFT, set timeblock to the total range
    timesteps = (end-start+1);
    if(freqmap != NULL)
	timeblock = timesteps;

    timestep = 0;
    
    vdata = new float [vmfile.get_my_block() * timeblock];
    tdata = new float [timeblock];

    main_steps = timesteps / timeblock;
    int mainblock;
    mainblock = timeblock;
    remainder = timesteps % timeblock;
    total_steps = main_steps;
    if(remainder > 0)
	total_steps++;
    
    if(actmap != NULL || repolmap != NULL || apdmap != NULL || triangmap != NULL)
	initialize_map(&activations, my_block);

    if(repolmap != NULL || apdmap != NULL)
	initialize_map(&repols, my_block);
	
    if(apdmap != NULL )
        initialize_map(&apds, my_block);

    if(triangmap != NULL ){
      initialize_map(&triangs, my_block);
      initialize_map(&repol30, my_block);
      initialize_map(&repol90, my_block);
    }
    
    if(freqmap != NULL)
	initialize_map(&dfs, my_block);

    for(int timestep = 0; timestep < total_steps; timestep++){
	vmfile.get_steps(start+timestep*mainblock, start+timestep*mainblock+timeblock-1, tdata, vdata);
	
	// now process
	if(activations != NULL)
	    amap_mod(timeblock, tdata, vdata, my_block, activations, mindvdt, act_vm);
	if(repols != NULL)
	    rmap_mod(timeblock, tdata, vdata, my_block, activations, repols, repol_vm);
	if(apds != NULL)
	    apdmap_mod(my_block, activations, repols, apds);
        if(triangs != NULL){
          rmap_mod(timeblock, tdata, vdata, my_block, activations, repol30, repol30_vm);
          rmap_mod(timeblock, tdata, vdata, my_block, activations, repol90, repol90_vm);
          apdmap_mod(my_block, repol30, repol90, triangs);
        }
	if(dfs != NULL)
	    dfmap_mod(timeblock, tdata, vdata, my_block, dfs);
	if(toutname != NULL)
	    write_tfiles(toutname, tdata, vdata, timestep*mainblock, timeblock, my_block, threecols);
	if(remainder > 0 && timestep == total_steps - 2)
	    timeblock = remainder; 
    }

    bool header = 1;
    if(igbfile != NULL)
	header = 0;
    
    if(activations != NULL && actmap != NULL)
	write_datfile(actmap, activations, my_block, tdata[0], isochrone, header, false);
    if(repols != NULL && repolmap != NULL)
	write_datfile(repolmap, repols, my_block, tdata[0], isochrone, header, false);
    if(apds != NULL && apdmap != NULL)
	write_datfile(apdmap, apds, my_block, tdata[0], isochrone, header, false);
    if(triangs != NULL && triangmap != NULL)
      write_datfile(triangmap, triangs, my_block, tdata[0], isochrone, header, false);
    if(dfs != NULL && freqmap != NULL)
	write_datfile(freqmap, dfs, my_block, tdata[0], isochrone, header, false);
    
    if(igbfile != NULL)
	vmfile.close();
    
#ifdef DEBUG
    cerr << "All done, exiting. " << endl;
#endif
#ifdef PARALLEL
    MPI_Finalize();
#endif
    return 0;
}

void usage()
{
    if(amRoot()){
	cerr << "Usage: parallel_mapper <required args> [options]" << endl << endl;;
	cerr << "  You may use either long or (short) option flags. " << endl << endl;
	cerr << "  Required: " << endl;
	cerr << "   Either --name or --igbfile as:" << endl;
	cerr << "     --name       (-n) (filename) : Base name for tfiles. Same as memfem and cg." << endl;
	cerr << "     --igbfile    (-g) (filename) : IGB file to read data from. Can read zipped but it's VERY VERY slow." << endl;
	cerr << "     --tend       (-e) (integer)  : End time. Need at least an end time. Filename for t files, offset for IGB." << endl;
	cerr << "  Optional: " << endl;
	cerr << "     --tstart     (-s) (integer)  : Start time. Assumed to be zero if not specified. Filenamem for t files, offset for IGB." << endl;
	cerr << "     --actmap     (-a) (filename) : Outputs an activation map of the given name " << endl;
        cerr << "     --mindvdt    (-m) (decimal)  : Set minimum dV/dt threshold for detecting activation in V/s. Default is 10." << endl;
        cerr << "     --act_vm     (-v) (decimal)  : Set minimum Vm threshold for detecting activation in V. Default is -0.070." << endl;
	cerr << "     --repolmap   (-r) (filename) : Outputs a repol map of the given name " << endl;
	cerr << "     --apdmap     (-d) (filename) : Outputs an APD map of the given name " << endl;
        cerr << "     --triangmap  (-c) (filename) : Outputs a triangulation map of the given name " << endl;
	cerr << "     --freqmap    (-f) (filename) : Outputs a dominant frequency map of the given name " << endl;
	cerr << "     --isochrone  (-i) (decimal)  : Isochrone interval for output (in seconds) " << endl;
	cerr << "     --repol_vm   (-p) (decimal)  : Repol threshold for APD in V. Default is -0.070 (-70 mV)" << endl;
        cerr << "     --repol30_vm (-j) (decimal)  : Repol threshold for triangulation in V. Default is -1 (-1000 mV)" << endl;
        cerr << "     --repol90_vm (-k) (decimal)  : Repol threshold for traingulation in V. Default is -1 (-1000 mV)" << endl;
	cerr << "     --toutname   (-t) (string)   : Output filename to use for tfiles. \".t<number>\" will be appended to this name." << endl;
	cerr << "     --timeblock  (-b) (integer)  : How many timesteps to load per processing step. This will be ignored if --freqmap (-f) is used." << endl;
	cerr << "     --help       (-h)            : this message " << endl;
    }
}
