#include "VmFile.h"

bool isBigEndian();
void ByteSwap(unsigned char * b, int n);
void str_tolower(char *s);

VmFile::VmFile()
{
    endianness = NULL;
    units = NULL;
    swapbytes = false;
    datsize = sizeof(float);
    headersize = IGBHEADERLENGTH*sizeof(char);
    x = 1;
    y = 1;
    z = 1;
    t = -1;
    nodect = 0;
    tzero = 0;
    voltagescale = 1;
    mode = 0; // IGB file
    tfile_currtime = 0;
    basename = NULL;
}

void VmFile::open(char* filename, int tnum)
{
    if(mode == 0){
	// IGB file mode
	char header[IGBHEADERLENGTH];
	char* token;
	char separators[] = " :\n";
	char* dattype;

	int readitems;

	token = dattype = NULL;
	
	//igbfile.open(filename, ios::binary);
	igbfile = fopen(filename, "r");

	//if(!igbfile.is_open())
	if(igbfile == NULL)
	    all_abort("IGB file could not be opened for read.");
	
	//igbfile.read(header, headersize);
	readitems = fread(header,1,headersize,igbfile);

	//if(igbfile.gcount() != headersize)
	if(readitems != headersize)
	    all_abort("Error reading IGB header.");
	
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
		tzero = atof(strtok(NULL, separators));
	
	    if(!strcmp(token,"unites")){
		units = strtok(NULL, separators);
		str_tolower(units);
		if(!strcmp(units,"mv"))
#ifdef DEBUG
		    cerr << "Scaling for milli-Volts" << endl;
#endif
		    voltagescale = 0.001;
	    }

	    // finish by looking for another token
	    token = strtok(NULL, separators);
	}
 
	// Sanity checks
	if(t == -1 || dattype == NULL || endianness == NULL || datsize == -1 || tzero == -1 || (x == 1 && y == 1 && t == 1)){
	    all_abort("IGB header was missing one of x, y, z, t, type, systeme (endianness), dattype, or zero.");
	}

	if(x*y*z > x && x*y*z > y && x*y*z > z){
	    all_abort("This program only reads 1d IGB files.");
	}

	nodect = x*y*z;
	if(get_igb_timesteps() != get_true_timesteps())
	    //all_abort("Actual timesteps don't match expected.");
	    t = get_true_timesteps();
    }else{
	// tfile mode
	basename = new char[strlen(filename)+1];
	strcpy(basename, filename);
	swapbytes = 0;
	datsize = sizeof(float)/sizeof(char);
	char *tline = new char[MAXLINESIZE];
	char *tfilename = new char[BASEBUFF];
	sprintf(tfilename, "%s.t%d", filename, tnum);
	get_tfile_nodect(basename, tnum, &x);
	ifstream tfile(tfilename, ios::in);
	if(!tfile){
	    all_abort("Tfile could not be opened for read.");
	}
	tfile.getline(tline, MAXLINESIZE);
	tzero = tline_extract_time(tline) - (tnum/1000.);
	tfile.close();
	nodect = x;
//	cerr << "Nodect = " << x << endl;
	y = 1;
	z = 1;
	voltagescale = 1;
	tfile_currtime = tnum;
    }
    calc_parallel_nodes(nodect, &my_startnode, &my_block);
}

void VmFile::close()
{
    passert(mode == 0);
    //igbfile.close();
    fclose(igbfile);
}

void VmFile::get_steps(int start, int end, float* tdata, float* vdata)
{
    int block;
    int readitems;
    float* linebuffer;
    linebuffer = new float[my_block];
    if(linebuffer == NULL)
	all_abort("Failed to allocate linebuffer.");

    float buffer;
    int range;

    range = end-start+1;

    if(mode == 0){
	// read from IGB file
        for (int line = 0 ; line < range; line++) {
	    set_time(start+line);
            readitems = fread(&linebuffer[0], (sizeof(float)/sizeof(char)), my_block, igbfile);

	    if(readitems != my_block){
	        all_abort("Read from igb file failed.");
	    }

            for(int node = 0; node < my_block; node++){
              buffer = linebuffer[node];
              if(swapbytes)
                ByteSwap5(buffer);
              if(node == 0){
                
              }
              vdata[(node*range)+line] = buffer*voltagescale;
	    }

        }
	set_time(end+1);

        for(int time = 0; time < range; time++){
            tdata[time] = tzero + start*0.001 + time*0.001;
        }
    }else{
	// read from tfiles
	char* filename = new char[BASEBUFF];
	for(int i = 0; i < range; i++){
	    sprintf(filename, "%s.t%d", basename, (start + i));
	    read_tfile(filename, tdata, i, &linebuffer[i*my_block], my_block, my_startnode);
	}
	for(int i = 0; i < range; i++)
	    for(int j = 0; j < my_block; j++){
		*(vdata + j*range + i) = linebuffer[i*my_block+j];
	    }
	
	tfile_currtime = end+1;
    }

    delete[] linebuffer;
}

void VmFile::get_next_step(float* time, float* vdata)
{
    int currtime;
//     if(mode == 0)
// 	//currtime = seek2time(igbfile.tellg());
// 	currtime = seek2time(ftell(igbfile));
//     else
// 	currtime = tfile_currtime;
    currtime = get_time();
    get_steps(currtime, currtime, time, vdata);
}

void VmFile::get_previous_step(float* time, float* vdata)
{
    int currtime;
//     if(mode == 0)
// 	//currtime = seek2time(igbfile.tellg());
// 	currtime = seek2time(ftell(igbfile));
//     else
// 	currtime = tfile_currtime;
    currtime = get_time();
    if(currtime > 0)
	currtime--;
    get_steps(currtime, currtime, time, vdata);
}

void VmFile::set_time(int time)
{
    if(mode == 0){
	//igbfile.seekg(time2seek(time));
	fseek(igbfile, time2seek(time), SEEK_SET);
	if(ferror(igbfile))
	    all_abort("Error seeking in IGB file.");
//	cerr << "Given time " << time << ", set seek to " << time2seek(time) << endl;
    }else{
	tfile_currtime = time;
    }
}

int VmFile::get_time()
{
    int currtime;
    if(mode == 0)
	//currtime = seek2time(igbfile.tellg());
	currtime = seek2time(ftell(igbfile));
    else
	currtime = tfile_currtime;
    return currtime;
}

void VmFile::get_step(int time, float* vdata)
{
    float* tdata;
    tdata = new float;

    if(mode == 0)
        set_time(time);
    else
	tfile_currtime = time;
    
    get_next_step(tdata, vdata);
}

int VmFile::get_nodect()
{
    return nodect;
}

int VmFile::get_my_startnode()
{
    return my_startnode;
}

int VmFile::get_my_block()
{
    return my_block;
}

int VmFile::blocksize(int start, int end)
{
    int return_size;

    if(mode == 0)
      return_size = (end-start+1)*my_block;
    else
	return_size = (end-start+1)*my_block;
    
    return return_size;
}

unsigned long int VmFile::time2seek(int time)
{
    passert(mode == 0); // make sure we're using IGB file

    unsigned long int seek;
    seek = (unsigned long int)datsize*
      (
       (unsigned long int)time*(unsigned long int)nodect
       +my_startnode
       ) + (unsigned long int)headersize;
//    cerr << "Calculated seek " << seek << " for time " << time << " with datsize " << datsize << " and node count " << nodect << " with header size " << headersize << endl;
    if(seek < 0)
	all_abort("time2seek produced a negative seek value.");
    return seek;
}

int VmFile::seek2time(ios::pos_type seek)
{
    passert(mode == 0);

    if(seek < 0)
	all_abort("Negative seek passed to seek2time.");
    return (seek-(ios::pos_type)headersize)/(datsize*nodect);
}

int VmFile::get_igb_timesteps()
{
    passert(mode == 0);
    return t;
}

int VmFile::get_true_timesteps()
{
    passert(mode == 0);
    unsigned long int seek_save;
    unsigned long int endtime;
    unsigned long int seek_time;

    //seek_save = igbfile.tellg();
    seek_save = ftell(igbfile);

    //igbfile.seekg(0, ios::end);
    fseek(igbfile, 0, SEEK_END);

    //seek_time = igbfile.tellg();
    seek_time = ftell(igbfile);

    if(seek_time < 0)
	all_abort("Seek failed.");

    endtime = seek2time(seek_time);
    //igbfile.seekg(seek_save);
    fseek(igbfile, seek_save, SEEK_SET);

    return endtime;
}

void VmFile::set_tfile_mode()
{
    mode = 1;
}

void VmFile::set_igbfile_mode()
{
    mode = 0;
}

void VmFile::set_startnode(int startnode)
{
    my_startnode = startnode;
}

void VmFile::set_block(int block)
{
    my_block = block;
}

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

// This is from rosettacode.org
void str_tolower(char *s)
{
    while(*s)
    {
	*s=tolower(*s);
	s++;
    }
}
// end rosettacode.org
