#include "cepmods.h"

void read_datfile(char* filename, MapStruct* data, int basenode, int nodect)
{
    ifstream datfile;
    datfile.open(filename);
    if(!datfile.is_open())
	all_abort("Could not open dat file for writing.");

    char* linebuffer = new char [MAXLINESIZE];
    
    // throw away header
    datfile.getline(linebuffer, MAXLINESIZE);
    for(int node = 0; node < basenode; node++)
	datfile.getline(linebuffer, MAXLINESIZE);
    
    int i = 0;
    while(datfile >> data[i].mapval && i < datact)
	i++;
    datfile.close();
}
