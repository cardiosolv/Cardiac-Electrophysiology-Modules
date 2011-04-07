cmake_minimum_required(VERSION 2.8)

set (CMAKE_BUILD_TYPE "NULL"
  CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
  )

if (CMAKE_BUILD_TYPE STREQUAL "NULL")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
    FORCE
    )
endif (CMAKE_BUILD_TYPE STREQUAL "NULL")

project (parallel_mapper)

set (modules
act_mod.cxx
repol_mod.cxx
amap_mod.cxx
rmap_mod.cxx
apdmap_mod.cxx
df_mod.cxx
dfmap_mod.cxx
freqmap_mod.cxx
periodmap_mod.cxx
dimap_mod.cxx
di_mod.cxx
)

set (utils
read_igbfile.cxx
read_trace.cxx
array_minmax.cxx
read_tfile.cxx
get_tfile_nodect.cxx
tline_extract_time.cxx
print_trace.cxx
write_datfile.cxx
get_igbfile_nodect.cxx
mesgutils.cxx
write_tfiles.cxx
get_mpi_stats.cxx
calc_parallel_nodes.cxx
initialize_map.cxx
)

set (classes
VmFile.cxx
) 

set (lib_sources
${modules}
${utils}
${classes}
)

add_library (cepmods_serial STATIC ${lib_sources})
find_library (FFTW3 
  fftw3 
  )

add_executable (serial_mapper parallel_mapper.cxx)
target_link_libraries(serial_mapper cepmods_serial ${FFTW3})

add_executable (ctrace trace.cxx)
target_link_libraries(ctrace cepmods_serial)

find_program(MPICXX mpicxx)
find_program(SED sed)
if(MPICXX AND SED)
  set( SED_ARGS
    "s/g++ //"
    )

  execute_process(COMMAND ${MPICXX} --showme 
    COMMAND ${SED} ${SED_ARGS}
    OUTPUT_VARIABLE PARALLEL_FLAGS 
    #OUTPUT_FILE found_parallel_flags.txt
    #ERROR_QUIET 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  set(ALL_FLAGS
    "${PARALLEL_FLAGS} -DPARALLEL"
    )

  add_library (cepmods_parallel STATIC ${lib_sources})

  set_target_properties(cepmods_parallel 
    PROPERTIES 
    COMPILE_FLAGS ${ALL_FLAGS}
    )

  add_executable(parallel_mapper parallel_mapper.cxx)
  target_link_libraries(parallel_mapper cepmods_parallel ${FFTW3} mpi)
  set_target_properties(parallel_mapper 
    PROPERTIES 
    COMPILE_FLAGS ${ALL_FLAGS}
    LINK_FLAGS ${PARALLEL_FLAGS}
    )

endif(MPICXX AND SED)