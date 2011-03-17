# SHELL = /bin/sh
OS := $(shell uname -s)

BINARIES = modules_tester debug serial_mapper parallel_mapper
LIBRARIES = libcepmods_p.a libcepmods_s.a
MODULES = act_mod.cxx repol_mod.cxx amap_mod.cxx rmap_mod.cxx apdmap_mod.cxx df_mod.cxx dfmap_mod.cxx freqmap_mod.cxx periodmap_mod.cxx dimap_mod.cxx di_mod.cxx
UTILS = read_igbfile.cxx read_trace.cxx array_minmax.cxx read_tfile.cxx get_tfile_nodect.cxx tline_extract_time.cxx print_trace.cxx write_datfile.cxx get_igbfile_nodect.cxx mesgutils.cxx write_tfiles.cxx get_mpi_stats.cxx calc_parallel_nodes.cxx initialize_map.cxx
CLASSES = VmFile.cxx
SOURCES = $(MODULES) $(UTILS) $(CLASSES)
DEBUGFLAGS = -DDEBUG -g
OBJECTS = $(SOURCES:.cxx=.o)
PARALLELFLAGS = -DPARALLEL -DMPICH_IGNORE_CXX_SEEK


ifeq ($(OS),Darwin)
	SCC = /usr/bin/g++ 
	PCC = mpicxx 
	FLAGS = -funroll-loops -O1 -I/Users/brock/local/fftw/include -I/Users/brock/local/include -I/usr/local/include -Wall -D_FILE_OFFSET_BITS=64 -I/opt/local/include
	AR = ar rcs
	LINKFLAGS = -L/usr/local/lib -L/Users/brock/local/lib -L/Users/brock/local/fftw/lib -L/opt/local/lib -lz
endif 

ifeq ($(OS),AIX)
	SCC = xlc++
	PCC = mpCC
	FLAGS = -qarch=pwr5 -qtune=pwr5 -lm -I${HOME}/local/include -L${HOME}/local/lib -DAIX
	AR = ar -rv 
	LINKFLAGS = -L/usr/lib -L/usr/local/lib -lz
endif

ifeq ($(OS),Linux)
	SCC = g++
	PCC = mpicxx
	FLAGS = -O1 -funroll-loops -Wall
	AR = ar rcs
        LINKFLAGS = -L/usr/lib64 -L/usr/local/lib64 -L/home/brock/local/lib  -lz
endif

parallel_mapper: FLAGS:=$(FLAGS) $(PARALLELFLAGS) 
parallel_mapper: SCC=$(PCC)
parallel_mapper: libcepmods_p parallel_mapper.cxx
	$(PCC) $@.cxx -o $@ $(FLAGS) $(LINKFLAGS) -L./ -lcepmods_p -lfftw3

debug: FLAGS:=$(FLAGS) $(PARALLELFLAGS) $(DEBUGFLAGS)
debug: SCC=$(PCC)
debug: libcepmods_p parallel_mapper.cxx
	$(PCC) parallel_mapper.cxx -o $@ $(FLAGS) $(LINKFLAGS) -L./ -lcepmods_p -lfftw3

serial_mapper: FLAGS:=$(FLAGS)
serial_mapper: libcepmods_s parallel_mapper.cxx
	$(SCC) parallel_mapper.cxx -o $@ $(FLAGS) -L./ $(LINKFLAGS) -lcepmods_s -lfftw3

trace: FLAGS:=$(FLAGS) -g
trace: libcepmods_s trace.cxx
	$(SCC) $@.cxx -o $@ $(FLAGS) $(LINKFLAGS) -L. -lcepmods_s

igb_tester: FLAGS:=$(FLAGS) $(PARALLELFLAGS)
igb_tester: SCC=$(PCC)
igb_tester: libcepmods_p igb_tester.cxx
	$(PCC) -o $@ $(FLAGS) $(LINKFLAGS) -L./ -lcepmods_p igb_tester.cxx

SCAPDOBJS = act_mod.o repol_mod.o read_trace.o scapd.o array_minmax.o
scapd: $(SCAPDOBJS)
	$(SCC) -o $@ $(FLAGS) $(LINKFLAGS) $(SCAPDOBJS)

all:	$(BINARIES)

libcepmods_p: FLAGS:=$(FLAGS) $(PARALLELFLAGS)
libcepmods_p: $(OBJECTS)
	$(AR) $@.a $(OBJECTS) 

libcepmods_s: $(OBJECTS)
	$(AR) $@.a $(OBJECTS)
clean:
	rm -f *~ core $(LIBRARIES) $(BINARIES) $(OBJECTS) *.o

%.o : %.cxx
	$(SCC) -c $(FLAGS) $< -o $@
