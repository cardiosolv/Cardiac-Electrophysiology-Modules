#include "cepmods.h"

float tline_extract_time(char* tline)
{
    char* token;
    token = new char[BASEBUFF];
    token = strtok(tline, " =");
    token = strtok(NULL, " =");

#ifdef DEBUG
    cerr << "Time: " << token << endl;
#endif
    return atof(token);
}
