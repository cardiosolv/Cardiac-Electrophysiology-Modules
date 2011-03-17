#include <iostream>
#include <fstream>
using namespace std;

void allocate_and_fill(float** arrayptr, int count);

int main(){
    float* my_array;
    char filename[] = "test.txt";
    ofstream textfile(filename, ios::out);
    if(!textfile){
	cerr << "File could not be opened." << endl;
	exit(1);
    }

    allocate_and_fill(&my_array, 20);
    for(int i = 0; i < 20; i++){
	textfile << *(my_array + i) << endl;
    }
}

void allocate_and_fill(float** arrayptr, int count){
    int blocksize, bufsize;
    blocksize = count/10;
    bufsize = blocksize;

    *arrayptr = (float*)calloc(blocksize, sizeof(float));

    for(int i = 0; i < count; i++){
	if(i == bufsize){
	    bufsize += blocksize;
	    *arrayptr = (float*)realloc(*arrayptr, bufsize*sizeof(float));
	    if(*arrayptr == NULL){
		cerr << "Out of memory! Bailing!" << endl;
		exit(1);
	    }else{
		cerr << "Memory resized to " << bufsize << " floats." << endl;
	    }
	}

	*(*arrayptr + i) = (float)i;
    }
}
