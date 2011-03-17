#include "cepmods.h"
#include "zlib.h"
#include <algorithm>

// THIS WILL NOT WORK ON WINDOWS
// Here's a nickel. Go get a real operating system.

#define ByteSwap5(x) ByteSwap((unsigned char *) &x,sizeof(x))

bool isBigEndian();
void ByteSwap(unsigned char * b, int n);

void read_igbfile(char* filename, int start, int* end, float** tdata, float** vdata, int nodect, int basenode)
{
    int returncheck;
    char header[IGBHEADERLENGTH];
    char* token;
    char* dattype;
    char* endianness;
    char* units;
    char separators[] = " :\n";
    int x, y, z, t;
    token = dattype = endianness = NULL;
    bool swapbytes = 0;
    float zero;
    int datsize;
    t = datsize = -1;
    zero = 0;
    x = y = z = 1;
    int range;
    float voltagescale = 1;
    // Limit buffer to 1GB max
    long int BUFFERLIMIT = (long int)1e9;

    gzFile igbfile = gzopen(filename, "rb");
    if(igbfile == NULL){
	all_abort("IGB file could not be opened.");
    }

    returncheck = gzread(igbfile, header, IGBHEADERLENGTH*sizeof(char));
    if(returncheck == -1 || returncheck == 0){
	all_abort("Error reading IGB file.");
    }

    token = strtok(header, separators);
    while(token != NULL){
	
#ifdef DEBUG
	cerr << "Token read: " << token << endl;
#endif

	if(!strcmp(token,"x"))
	    x = atoi(strtok(NULL, separators));
	if(!strcmp(token,"y"))
	    y = atoi(strtok(NULL, separators));
	if(!strcmp(token,"z"))
	    z = atoi(strtok(NULL, separators));
	if(!strcmp(token,"t"))
	    t = atoi(strtok(NULL, separators));
	if(!strcmp(token,"type")){
	    dattype = strtok(NULL, separators);
	    if(!strcmp(dattype,"float")){
		datsize = sizeof(float);
	    }else{
		cerr << "This program only reads IGBs with floats, not " << dattype << "s. Bailing." << endl;
	    }
	}
	if(!strcmp(token,"systeme")){
	    endianness = strtok(NULL, separators);
#ifdef DEBUG
	    cerr << "Is this machine big-endian? Bool: " << isBigEndian() << endl;;
#endif
	    if(!strcmp(endianness, "big_endian") == isBigEndian())
		swapbytes = 0;
	    else
		swapbytes = 1;
	}
	if(!strcmp(token,"zero"))
	    zero = atof(strtok(NULL, separators));
	
	if(!strcmp(token,"unites")){
	    units = strtok(NULL, separators);
	    if(!strcmp(units,"mV"))
		voltagescale = 0.001;
	}

	// finish by looking for another token
	token = strtok(NULL, separators);
    }
    
    if(t == -1 || dattype == NULL || endianness == NULL || datsize == -1 || zero == -1 || (x == 1 && y == 1 && t == 1)){
	all_abort("IGB header was missing one of x, y, z, t, type, systeme (endianness), datsize, or zero.");
    }

    if(x*y*z > x && x*y*z > y && x*y*z > z){
	all_abort("This program only reads 1d IGB files.");
    }

    range = (*end)-start+1;

    // At this point the header has been read. We know how much vdata we should have.
    *vdata = (float*)malloc(x*y*z*range*datsize);
    if(*vdata == NULL)
	all_abort("Insufficient memory when allocating vdata storage.");
    *tdata = (float*)malloc(range*sizeof(float));
    for(int i = 0; i < range; i++){
	*(*tdata + i) = (zero + start + i)*0.001;
    }

    int lineblock = 100;
    // don't go over 2GB by default
    if((lineblock*x*y*z*datsize) > BUFFERLIMIT)
	lineblock = BUFFERLIMIT / (x*y*z*datsize);

    float* linebuffer = NULL;;
    float* tmp = NULL;
    //linebuffer = (float*)malloc(lineblock*x*y*z*datsize);
    while((tmp = (float*)realloc(linebuffer, lineblock*x*y*z*datsize)) == NULL && lineblock > 1){
	linebuffer = tmp;
	lineblock--;
    }
    if(tmp == NULL)
	all_abort("Failed to allocate buffer. Exiting");
    else
	linebuffer = tmp;

    int remainder;
    int mainblocks;

    mainblocks = start / lineblock;
    remainder = start % lineblock;

    // Read and throw away all the data before start, because gzseek seems to be broken
    for(int i = 0; i < mainblocks; i++){
	returncheck = gzread(igbfile, linebuffer, lineblock*x*y*z*datsize);
	if(returncheck == 0){
	    all_abort("Reached end of file before desired data.");
	}
	if(returncheck == -1)
	    all_abort("Error reading IGB file.");
    }
    // get the last bit before "start"
    if(remainder != 0){
	returncheck = gzread(igbfile, linebuffer, remainder*x*y*z*datsize);
	if(returncheck == 0){
	    all_abort("Reached end of file before desired data.");
	}
	if(returncheck == -1)
	    all_abort("Error reading IGB file.");
    }
    // figure out our loop bounds for the real data
    mainblocks = range / lineblock;
    remainder = range % lineblock;
    float buffer;

    for(int i = 0; i < mainblocks; i++){
	returncheck = gzread(igbfile, linebuffer, lineblock*x*y*z*datsize);

	if(returncheck == -1)
	    all_abort("Error reading IGB file.");
	if(returncheck == 0)
	    all_abort("Unexpected end of file before reaching end of desired data.");

	// if we got here we have a good buffer full of data
	for(int timeline = 0; timeline < lineblock; timeline++){
	    for(int node = 0; node < nodect; node++){
		buffer = linebuffer[(timeline*x*y*z)+node+basenode];
		if(swapbytes)
		    ByteSwap5(buffer);
		*(*vdata+(node*range)+timeline+(i*lineblock)) = buffer*voltagescale;
		}
	}
    }
    if(remainder != 0){
	returncheck = gzread(igbfile, linebuffer, remainder*x*y*z*datsize);
	
	if(returncheck == 0)
	    all_abort("Reached end of file before reaching end of desired data.");
	if(returncheck == -1)
	    all_abort("Error reading IGB file.");
	
	for(int timeline = 0; timeline < remainder; timeline++){
	    for(int node = 0; node < nodect; node++){
		buffer = linebuffer[(timeline*x*y*z)+node+basenode];
		if(swapbytes)
		    ByteSwap5(buffer);
		*(*vdata+(node*range)+timeline+(mainblocks*lineblock)) = buffer*voltagescale;
	    }
	}
    }
    free(linebuffer);
}// read_igbfile

// These are from codeproject.com
bool isBigEndian()
{
    short word = 0x4321;
    if((* (char *)& word) != 0x21 )
	return true;
    else
	return false;
}




void ByteSwap(unsigned char * b, int n)
{
    register int i = 0;
    register int j = n-1;
    while (i<j){
	std::swap(b[i], b[j]);
	i++; j--;
    }
}
// end codeproject.com
