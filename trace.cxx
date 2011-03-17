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
    char* igbfile = NULL;
    int start = 0;
    int end = 0;
    int timeblock = 1;
    int selectnode = -1;
    char* nodefilename = NULL;
    int optc;
#ifndef AIX
    static struct option long_options[] = 
    {
	{"help", no_argument, 0, 'h'},
	{"name", required_argument, 0, 'n'},
	{"tstart", required_argument, 0, 's'},
	{"tend", required_argument, 0, 'e'},
	{"igbfile", required_argument, 0, 'g'},
	{"timeblock", required_argument, 0, 'b'},
        {"selectnode", required_argument, 0, 'l'},
	{"nodefile", required_argument, 0, 'f'},
    };
    int option_index = 0;
#endif

#ifdef PARALLEL
    all_abort("This is serial code. Recompile or rerun with only one process.");
#endif

#ifndef AIX
    while((optc = getopt_long(argc, argv, "n:g:s:e:h:b:l:f:", long_options, &option_index)) != -1){
#endif
#ifdef AIX
    while(((optc) = getopt(argc, argv, "n:g:s:e:h:b:l:")) != -1){
#endif
	switch(optc){
	case 'n': basename = optarg; break;
	case 's': start = atoi(optarg); break;
	case 'e': end = atoi(optarg); break;
	case 'g': igbfile = optarg; break;
	case 'b': timeblock = atoi(optarg); break;
	case 'l': selectnode = atoi(optarg); break;
	case 'f': nodefilename = optarg; break;
	case 'h': usage(); break;
	default:
	    usage();
	}
    }
    
    // do some sanity checks
    if(basename == NULL && igbfile == NULL){
	usage();
	all_abort("Must specify either a base name for tfiles, or an IGB file.");
    }
    
    if(selectnode < 0 && nodefilename == NULL){
	usage();
	all_abort("Must specify a node or nodefile name, and node numbers must be greater than 0.");
    }

    if(igbfile != NULL && (start != 0 || end != 0)){
	warn("You specified an IGB file with a start and/or end time. These times will be used as offsets in terms of timesteps for making maps, not as raw times.");
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

    if(timeblock < 1)
	all_abort("Block size less than one is nonsense.");
    
    // Meat
    float* tdata;
    int timesteps;
    int timestep;
    VmFile vmfile;
    int main_steps;
    int remainder;
    int total_steps;
    ifstream nodefile;
    int nodect = 0;
    int nodenum = 0;
    char linebuf[MAXLINESIZE];
    int* nodes;
    float* ndata;

    if(basename != NULL){
	vmfile.set_tfile_mode();
	vmfile.open(basename, start);
    }
    if(igbfile != NULL){
	vmfile.set_igbfile_mode();
	vmfile.open(igbfile);
    }
        
    // Here starts the real processing
    timesteps = (end-start+1);
    timestep = 0;

    if(nodefilename != NULL){
      nodefile.open(nodefilename, ifstream::in);
      if (!nodefile.is_open())
	all_abort("Failed to open specified nodefile for read.");
      while(nodefile.getline(linebuf, MAXLINESIZE)){
	nodect++;
      }
      nodefile.close();
      // cerr << "Counted " << nodect << " nodes." << endl;
      nodes = new int[nodect];
      ndata = new float[nodect*timesteps];
      nodefile.clear();
      nodefile.open(nodefilename, ifstream::in);
      int node;
      while(!nodefile.eof() && nodenum < nodect){
	nodefile >> node;
	nodes[nodenum] = node;
	nodenum++;
      }
      nodefile.close();
      // cerr << "Counted " << nodenum << " nodes." << endl;
      //all_abort("Quitting here. Test complete.");
    }else{
      nodect = 1;
      nodes = new int[nodect];
      nodes[0] = selectnode;
      ndata = new float[timesteps];
    }
    tdata = new float [timesteps];
    for(nodenum = 0; nodenum < nodect; nodenum++){
      
      vmfile.set_startnode(nodes[nodenum]);
      vmfile.set_block(1);

      main_steps = timesteps / timeblock;
      remainder = timesteps % timeblock;

      total_steps = main_steps;
      if(remainder > 0)
	total_steps++;
      
      int true_step = start;
      for(int timestep = 0; timestep < total_steps; timestep++){
	vmfile.get_steps(true_step, true_step+timeblock-1, &tdata[timestep], (ndata + timesteps*nodenum + timestep));
	true_step += timeblock;
	if(remainder > 0 && timestep == total_steps - 2)
	  timeblock = remainder; 
      }
      
    }
    print_trace(tdata, ndata, nodect, timesteps*timeblock);
    
    
    if(igbfile != NULL)
	vmfile.close();
    
#ifdef DEBUG
    cerr << "All done, exiting. " << endl;
#endif

    return 0;
}

void usage()
{
    if(amRoot()){
	cerr << "Usage: trace <required args> [options]" << endl << endl;;
	cerr << "  You may use either long or (short) option flags. " << endl << endl;
	cerr << "  Required: " << endl;
	cerr << "   Either --name or --igbfile as:" << endl;
	cerr << "     --name       (-n) (filename) : Base name for tfiles. Same as memfem and cg." << endl;
	cerr << "     --igbfile    (-g) (filename) : IGB file to read data from. Can read zipped but it's VERY VERY slow." << endl;
        cerr << "   Either --selectnode or --nodefile as:" << endl;
        cerr << "     --selectnode (-l) (integer)  : Node to take a trace from." << endl;
	cerr << "     --nodefile   (-f) (filename) : Filename to take a list of nodes from." << endl;
	cerr << "   Only:" << endl;
	cerr << "     --tend       (-e) (integer)  : End time. Need at least an end time. Filename for t files, offset for IGB." << endl;
	cerr << "  Optional: " << endl;
	cerr << "     --timeblock  (-b) (integer)  : Number of timesteps to load per processing run." << endl;
	cerr << "     --help       (-h)            : this message " << endl;
    }
}
