#include "cepmods.h"

void print_trace(float* tdata, float* vdata, int lines)
{
    if(amRoot()){
	for(int i = 0; i < lines; i++){
	    cout << *(tdata + i) << " " << *(vdata + i) << endl;
	}
    }
}

void print_trace(float* tdata, float* vdata, int nodes, int times){
  if(amRoot()){
    for(int i = 0; i < times; i++){
      cout << *(tdata + i) << " ";
      for(int node = 0; node < nodes; node++){
	cout << *(vdata + times*node + i);
	if(node < (nodes-1)){
	  cout << " ";
	}
      }
      cout << endl;
    }
  }
}
