#include "cepmods.h"
#include <getopt.h>
#include <cstdlib>

using namespace std;

void usage();

int main(int argc, char** argv)
{
    char* basename = NULL;
    char* actmap = NULL;
    char* repolmap = NULL;
    char* apdmap = NULL;
    char* freqmap = NULL;
    char* igbfile = NULL;
    int start = 0;
    int end = 0;
    int optc;
    float isochrone = 0.001;
    float pct = 0.9;

    static struct option long_options[] = 
    {
	{"help", no_argument, 0, 'h'},
	{"name", required_argument, 0, 'n'},
	{"tstart", required_argument, 0, 's'},
	{"tend", required_argument, 0, 'e'},
	{"actmap", required_argument, 0, 'a'},
	{"repolmap", required_argument, 0, 'r'},
	{"apdmap", required_argument, 0, 'd'},
	{"freqmap", required_argument, 0, 'f'},
	{"isochrone", required_argument, 0, 'i'},
	{"pct", required_argument, 0, 'p'},
	{"igbfile", required_argument, 0, 'g'},
	
    };
    int option_index = 0;
    while((optc = getopt_long(argc, argv, "b:g:s:e:a:r:d:f:i:h:p:", long_options, &option_index)) != -1){
	switch(optc){
	case 'n': basename = optarg; break;
	case 's': start = atoi(optarg); break;
	case 'e': end = atoi(optarg); break;
	case 'a': actmap = optarg; break;
	case 'r': repolmap = optarg; break;
	case 'd': apdmap = optarg; break;
	case 'f': freqmap = optarg; break;
	case 'i': isochrone = atof(optarg); break;
	case 'p': pct = atof(optarg); break;
	case 'g': igbfile = optarg; break;
	case 'h': usage(); break;
	default:
	    usage();
	}
    }

    // do some sanity checks
    if(basename == NULL && igbfile == NULL){
	cerr << "Must specify either a base name for tfiles, or an IGB file. Bailing." << endl;
	usage();
    }
	
    if(igbfile != NULL && (start != 0 || end != 0)){
	cerr << "WARNING: You specified an IGB file with a start and/or end time. These times will be used as offsets in terms of timesteps for making maps, not as raw times." << endl;
    }

    if(basename != NULL && igbfile !=NULL){
	cerr << "Don't know what to do with both tfiles and an IGB file. Bailing." << endl;
	exit(1);
    }

    if(basename != NULL && end == 0){
	cerr << "We assume start = 0 if not specified, but you have to supply an end time when using tfiles. Bailing." << endl;
	usage();
    }

    if(start > end){
	cerr << "Times backwards. Bailing. " << endl;
	exit(1);
    }

    if(repolmap != NULL && actmap == NULL)
	cerr << "WARNING: You specified a repol map without an activation map. We will have to calculate one anyway, but it won't be output. Continuing... " << endl;
    if(isochrone > 1)
	cerr << "WARNING: isochone time units are seconds, NOT milliseconds. Do you want " << isochrone << "-second long isochrones? Continuing... " << end;
    
    if(apdmap != NULL && (actmap == NULL || repolmap == NULL))
	cerr << "WARNING: You specifed an APD map without activation and repolarization maps. We will have to calculate them anyway, but they won't necessarily be output (depending on which you specified). Continuing... " << endl;
    
    if(actmap == NULL && repolmap == NULL && apdmap == NULL && freqmap == NULL){
	cerr << "Stubbornly refusing to do a bunch of work without an output file specified. Bailing." << endl;
	usage();
    }

    if(pct < 0){
	cerr << "A repol percent less than zero is nonsense. Bailing. " << endl;
	exit(1);
    }

    // Meat
    float* vdata;
    float* tdata;
    int nodect;
    ActData* activations;
    ActData* repols;
    ActData* apds;
    ActData* dfs;
    int timesteps = (end-start+1);

    // get our data no matter what we're doing with it.
    if(basename != NULL)
	read_tfiles(basename, start, end, &tdata, &vdata, &nodect);
    
    if(igbfile != NULL)
	read_igbfile(igbfile, &tdata, start, end, &vdata, &nodect);

    if(actmap != NULL || repolmap != NULL || apdmap != NULL){
	cerr << "Generating activation map... ";
	amap_mod(timesteps, &tdata, &vdata, nodect, &activations);
	write_datfile(actmap, &activations, nodect, tdata[0], isochrone);
	cerr << "done." << endl;
    }

    if(repolmap != NULL || apdmap != NULL){
	cerr << "Generating repolarization map... ";
	rmap_mod(timesteps, &tdata, &vdata, nodect, &activations, &repols, pct);
	write_datfile(repolmap, &repols, nodect, tdata[0], isochrone);
	cerr << "done." << endl;
    }

    if(apdmap != NULL){
	cerr << "Generating APD map... ";
	apdmap_mod(nodect, &activations, &repols, &apds);
	write_datfile(apdmap, &apds, nodect, tdata[0], isochrone);
	cerr << "done." << endl;
    }

    if(freqmap != NULL){
	cerr << "Generating dominant frequency map... ";
	dfmap_mod(timesteps, &tdata, &vdata, nodect, &dfs);
	write_datfile(freqmap, &dfs, nodect, tdata[0], isochrone);
	cerr << "done." << endl;
    }
    cerr << "All done, exiting. " << endl;

    return 0;
}

void usage()
{
    cerr << "Usage: serial_mapper <required args> [options]" << endl << endl;;
    cerr << "  You may use either long or (short) option flags. " << endl << endl;
    cerr << "  Required: " << endl;
    cerr << "   Either --name or --igbfile as:" << endl;
    cerr << "     --name      (-n) (filename) : Base name for tfiles. Same as memfem and cg." << endl;
    cerr << "     --igbfile   (-g) (filename) : IGB file to read data from. Can read zipped but it's VERY VERY slow." << endl;
    cerr << "     --tend      (-e) (integer)  : End time. Need at least an end time. Filename for t files, offset for IGB." << endl;
    cerr << "  Optional: " << endl;
    cerr << "     --tstart    (-s) (integer)  : Start time. Assumed to be zero if not specified. Filenamem for t files, offset for IGB." << endl;
    cerr << "     --actmap    (-a) (filename) : outputs an activation map of the given name " << endl;
    cerr << "     --repolmap  (-r) (filename) : outputs a repol map of the given name " << endl;
    cerr << "     --apdmap    (-d) (filename) : outputs an APD map of the given name " << endl;
    cerr << "     --freqmap   (-f) (filename) : outputs a dominant frequency map of the given name " << endl;
    cerr << "     --isochrone (-i) (decimal)  : isochrone interval for output (in seconds) " << endl;
    cerr << "     --pct       (-p) (decimal)  : Percent repol for APD. As a fraction of 1, not a fraction of 100. Default is 0.9 (90%). " << endl;
    cerr << "     --help      (-h)            : this message " << endl;
    exit(1);    
}
