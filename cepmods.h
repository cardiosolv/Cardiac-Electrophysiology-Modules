/*
 * Brock M. Tice
 * Modules Header
 *
 *  modules.h - include useful objects and module declarations
 *
 */

// IMPORTANT INFO ABOUT DATA FORMATS
// **tdata - points to a 1D array of floats containing times, size is equal to the end-start+1
// **vdata - points to a 2D array of floats containing voltages in mV, size is equal to the size of tdata * the number of nodes
//         * Rows = nodes
//         * Columns = time slices
//         Thus, to get the data for node 9 at time step 6 (which is actually 10, it's base 0) you would use *( *vdata + ( 9 * (time slices) ) + 6 )

#ifndef _MODULES_H_
#define _MODULES_H_
// Includes that we always want
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>

// for tolower()
#include <ctype.h>
using namespace std;

// map structure
typedef struct
{
    float lastvm;
    float lasttime;
    float mapval;
    float maxdvdt;
    float maxtime;
    int upstroke;
} MapStruct;

// Useful constants
#define MAXLINESIZE 2000
#define BASEBUFF 1000
#define IGBHEADERLENGTH 1024
#define IGBTAGLENGTH 20

// message / mpi functions
void all_abort(const char* message);
bool amRoot();
void warn(const char* message);
void passert(const bool test); // parallel assert-type function

// Utility functions
// format is function( args )
//           description
//           Inputs
//           Outputs

void initialize_map(MapStruct** map, int size);
// Allocates and initializes to all -1 a map array
// Inputs:
//   size - desired length of "map"
// Outputs:
//   map - pointer to a pointer for map, will be allocated to in the function

void read_trace(char* filename, float* tdata, float* vdata, int* outsize, float start, float end);
// Reads a trace, i.e. a file with two columns: time in the left, data in the right. anything you pass to it should already be allocated
// Inputs: 
//   filename - the whole filename
//   start - start time (per the left column of data)
//   end - end time (per the left column of data)
// Outputs: 
//   tdata - time data, single float. You should allocate it before passing it.
//   vdata - voltage data, array of floats. You should allocate it before passing it.
//   outsize - number of data points)

void read_tfile(char* filename, float* time, int toffset, float* vdata, int datact, int basenode = 0);
// Reads a tfile into memory
// Inputs: 
//   filename - takes the WHOLE NAME of the tfile including .t(number)
//   datact  - expected number of nodes, you can get this with get_tfile_nodect() which is described below
//   basenode - (optional) if you want a basenmode other than 0, specify it
// Outputs: 
//   tdata - time pointer tdata (should already be allocated)
//   vdata - voltage data pointer vdata (should already be allocated)

void read_tfiles(char* basename, int start, int end, float** tdata, float** vdata, int nodect, int basenode = 0);
// Reads a bunch of tfiles into memory
// Inputs: 
//   basename - (base filename without .t, like memfem/cg take with -n)
//   start - start tfile number
//   end - end tfile number
//   nodect - how many nodes to expect. Can get this with get_tfile_nodect(), described below
//   basenode - (optional) node to start at if desired, defaults to 0
// Outputs:
//   tdata - function will allocate this array
//   vdata - function will allocate this array

void get_tfile_nodect(char* basename, int filenum, int* nodect);
// Gets the node count in a tfile of a given basename and file number
// Inputs:
//   basename - base filename without .t
//   filenum - the number appended after .t
// Outputs:
//   nodect - number of nodes in the tfile

void array_minmax(float* data, int ndata, float* minval, int* mindex, float* maxval, int* maxdex);
// Gets the min and max values and indices from a 1-D array
// Inputs:
//   data - some 1D array
//   ndata - number of elements in the array
// Outputs:
//   minval - minimum value
//   mindex - index at which minimum value was found
//   maxval - maximum value
//   maxdex - index at which max value was found

float tline_extract_time(char* tline);
// Gets the time from the header line of a tfile
// Inputs:
//   tline - string containing a tfile header
// Outputs:
//   (returned float value) - time in tfile header

void print_trace(float* tdata, float* vdata, int lines);
// Prints a trace from the given vdata and tdata array
//   *NOTE* - you can print an arbitrary node by adding some offset to the vdata pointer. For instance, to print node 9 over all times:
//            print(tdata, (vdata + 9*lines);
// Inputs:
//   tdata - 1-d time data array
//   vdata - 1-d time data array
//   lines - number of time steps to print
// Outputs:
//   PRINTS TO STDOUT

void print_trace(float* tdata, float* vdata, int nodes, int lines);
// Prints a trace from the given vdata array of arrays and tdata array
// Wraps print_trace(tdata, vdata, lines)
//   *NOTE* - you can print an arbitrary node by adding some offset to the vdata pointer. For instance, to print node 9 over all times:
//            print(tdata, (vdata + 9*lines);
// Inputs:
//   tdata - 1-d time data array
//   vdata - 2-d node / time data array (vdata[node][timestep])
//   lines - number of time steps to print
// Outputs:
//   PRINTS TO STDOUT


void write_datfile(char* filename, MapStruct* actdata, int datact, float headertime, float iso_step, bool header, bool threecols);
// Writes an arbitrary datfile. Will write a t-file like output if header == 1, otherwise, no header
// Inputs:
//   filename - file to write to
//   actdata - activation times or other data (I use this in write_tfiles, for example)
//   datact - number of elements in actdata
//   headertime - time to insert into t-file header if header == 1, otherwise I guess it's unused.
//   iso_step - output resolution. will round values up to the next multiple of this. Set to < 0 to disable isochrones
//   header - do you want a t-file like header?

void read_datfile(char* filename, MapStruct* data, int basenode, int nodect);
// Reads an arbitrary datfile. Will only fill in the "mapval" part of the struct.
// Inputs:
//   filename - file to read, with extension
//   basenode - node to start from
//   nodect - number of nodes to read starting at basenode
// Outputs:
//   data - array of "MapStruct"s with mapval data filled in

// DEPRECATED, see VmFile.h
//void read_igbfile(char* filename, int start, int* end, float** tdata, float** vdata, int nodect, int basenode = 0);
// Reads an IGB file
//   *NOTE* - if this function hits the end of a tfile unexpectedly, it will change the input value of "end" and go from there.
//            In this case, a warning will be printed to STDERR
// Inputs:
//   filename - file to read, with .igb and optionally .gz
//   start - start time step to read, 0 is the beginning of the data
//   end - last time step to read. Will complain loudly and exit if you specify an end beyond the end of the file
//   nodect - how many nodes to expect. You can find out with get_igbfile_nodect() below
//   basenode - (optional) which node to start from, if not zero
// Outputs:
//   tdata - this will be allocated memory and assigned time data
//   vdata - this will be allocated memory and assigned voltage data
//   (end) - will be modified if gzseek fails to read where there should be data. Your program should account for the fact that this might change when read_igbfile is called

void get_igbfile_nodect(char* filename, int* nodect);
// Gets the number of nodes in an igb file (according to the header)
// Inputs:
//   filename - igbfile to check
// Outputs:
//   nodect - number of nodes

// THIS FUNCTION NOT YET IMPLEMENTED
//void merge_igbfiles(char* filename1, char* filename2, int start1, int end1, int start2, int end2);

// THIS FUNCTION NOT YET IMPLEMENTED
//void write_igbfile(char* filename, int start, int end);

void calc_parallel_nodes(int nodect, int* my_startnode, int* my_block);
// Given a node count, will automatically compute the start node and number of nodes each parallel process should handle
// Inputs:
//   nodect - can get in a variety of ways, see the get_igbfile_nodect() and get_tfile_nodect() functions
// Outputs:
//   my_startnode - which node to start processing at
//   my_block - how many nodes to process

void get_mpi_stats(int* procs, int* rank);
// Get mpi rank and number of procs. When used in serial always sets procs = 1, rank = 0 (even without MPI)
// Inputs:
//   none
// Outputs:
//   procs - number of procs in MPI_COMM_WORLD
//   rank - rank of the current process

void write_tfiles(char* basename, float* tdata, float* vdata, int startnum, int timesteps, int nodes, bool threecols);
// Writes t files
// Inputs:
//   basename - base t-file name (without .t)
//   tdata - 1-D array of time data
//   vdata - 2-D array of voltage data
//   startnum - offset of t-file numbering
//   timesteps - number of timesteps in vdata, must be correct even if you don't want all of the timesteps in the array (only pass an array with timesteps you want)
//   nodes - how many nodes are in the vdata array
// Outputs:
//   writes t-files with name basename in the working directory. If you feed it a basename with a directory name, it will write into that directory (if it exists)

// Module declarations
void act_mod(float* tdata, float* vdata, int ndata, MapStruct* actdata, float mindvdt, float minvm);
// Get time of the max dv/dt during the upstroke for ONLY ONE NODE
// Makes sure voltage continues to rise for 3 consecutive timesteps as a filter of sorts to eliminate blips
// Inputs:
//   tdata - 1-D time data array
//   vdata - 1-D voltage data array
//   ndata - how many time steps are in the arrays
//   mindvdt - good default is 10. Units are volts/s
//   minvm - good default is -0.070. Units are volts
// Outputs:
//   actdata - activation time (1 float)

void repol_mod(float* tdata, float* vdata, int ndata, MapStruct* actdata, MapStruct* repoldata, float recoverypct);
// Get time of repolarization following a given actiation time. Uses a simple threshold
// Inputs:
//   tdata - 1-D time data array
//   vdata - 1-D voltage data array
//   ndata - number of time steps in the arrays
//   recoverypct - What percentage recovery to use as a repol. threshold.  Range is 0-1 (1 = 100%). Calculates min and max over the data to figure it out.
//   actdata - an array of ndata activation times from act_mod
// Outputs:
//   repoldata - repolarization time (1 float)

void negcross_mod(float* tdata, float* vdata, int ndata, MapStruct* crossdata, float threshold);
// Get time of negative threshold crossing
// Inputs:
//   tdata - 1-D time data array
//   vdata - 1-D voltage data array
//   ndata - number of timesteps in the arrays
//   threshold - value at which negative crossing is detected and recorded
// Outputs:
//   crossdata - array of ndata MapStructs with mapval set to the time of negative threshold crossing

void poscross_mod(float* tdata, float* vdata, int ndata, MapStruct* crossdata, float threshold);
// Get time of positive threshold crossing
// Inputs:
//   tdata - 1-D time data array
//   vdata - 1-D voltage data array
//   ndata - number of timesteps in the arrays
//   threshold - value at which positive crossing is detected and recorded
// Outputs:
//   crossdata - array of ndata MapStructs with mapval set to the time of positive threshold crossing

void di_mod(float* tdata, float* vdata, int ndata, MapStruct* repoldata, MapStruct* actdata, MapStruct* didata, float mindvdt, float minvm);
// Calculate activation and diastolic interval follwing a given repolarization
// Inputs:
//   tdata - 1-D time data array
//   vdata - 1-D voltage data array
//   ndata - number of timesteps in the arrays
//   repoldata - previous repolarization data
//   mindvdt - activation rapidity threshold
//   minvm - activation voltage threshold
// Outputs:
//   actdata - activation data
//   didata - diastolic interval data (just act time - prior repol time)

void dimap_mod(int timesteps, float* tdata, float* vdata, int nodect, MapStruct* repols, MapStruct* activations, MapStruct* dis);
// Calculate a map of diastolic intervals (see di_mod)
// Inputs:
//   timesteps - how many timesteps are in the tdata/vdata arrays
//   tdata - 1-D time array
//   vdata - 2-D voltage array
//   nodect - number of nodes in the arrays
//   repols - map of prior repolarization times
// Outputs:
//   activations - 1-D activation time array
//   dis - 1-D diastolic interval array

void periodmap_mod(int nodect, MapStruct* acts1, MapStruct* acts2, MapStruct* periods);
// Finds the period between two activations
// Inputs:
//   nodect - number of nodes in the arrays
//   acts1 - first (in time) set of activations
//   acts2 - second (in time) set of activations
// Ouptuts:
//   periods - periods between acts1 and acts2

void freqmap_mod(int nodect, MapStruct* periods, MapStruct* freqs);
// Just inverts periods to get frequencies
// Inputs:
//   nodect - number of nodes in the arrays
//   periods - previously-calculated periods between activations
// Outputs:
//   freqs - frequency of activations

void amap_mod(int timesteps, float* tdata, float* vdata, int nodect, MapStruct* activations, float mindvdt, float minactvm);
// Runs activation time calculations on an entire 2-D array of nodes/times
// Inputs:
//   timesteps - how many time steps are in the arrays
//   nodect - how many nodes are in the arrays
//   tdata - 1-D time data array
//   vdata - 2-D voltage data array
// Outputs:
//   activations - 1-D activation time array

void rmap_mod(int timesteps, float* tdata, float* vdata, int nodect, MapStruct* activations, MapStruct* repols, float recoverypct);
// Runs repolarization time calculations
// Inputs:
//   timesteps - number of timesteps in tdata/vdata arrays
//   nodect - number of nodes in tdata/vdata arrays
//   activations - 1-D activation time array (from amap_mod probably)
//   recoverypct - whwat percent of recovery to use (range 0 - 1), see repol_mod above
//   tdata - 1-D array of time data
//   vdata - 2-D array of voltage data
// Outputs:
//   repols - 1-D repolarization time array

void apdmap_mod(int nodect, MapStruct* activations, MapStruct* repols, MapStruct* apds);
// Generate an APD map from activation and repolarization times. Basically it just subtracts
// Inputs:
//   nodect - number of nodes in the other inputs
//   activations - 1-D activation time array
//   repols - 1-D repol time array
// Outputs:
//   apds - 1-D repolarization time array

void df_mod(float* tdata, float* vdata, int ndata, MapStruct* dfs);
// Calculate the dominant frequency for one node
// Inputs:
//   tdata - 1-D array of times
//   vdata - 1-D array of voltage data
//   ndata - how many time steps are in the arrays
// Outpts:
//   Dominant frequency. Just one float. (in Hz)

void dfmap_mod(int timesteps, float* tdata, float* vdata, int nodect, MapStruct* dfs);
// Generate a map of dominant frequencies over a range of nodes
// Inputs:
//   timesteps - number of timesteps in the arrays
//   tdata - 1-D array of time data
//   vdata - 2-D array of time data
//   nodect - how many nodes are in the supplied arrays
// Outputs:
//   dfs - 1-D array of dominant frequency

#endif

