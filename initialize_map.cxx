#include "cepmods.h"

void initialize_map(MapStruct** map, int size)
{
    *map = new MapStruct[size];
    if(*map == NULL)
	all_abort("Failed to allocate map array.");
    
    for(int i = 0; i < size; i++){
	(*map + i)->lastvm = -1;
	(*map + i)->lasttime = -1;
	(*map + i)->mapval = -1;
    }
}
