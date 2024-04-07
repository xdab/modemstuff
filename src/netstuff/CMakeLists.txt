set(HEADER_LIST 
  "${modemstuff_SOURCE_DIR}/include/modemstuff/netstuff/netserver.h"
  "${modemstuff_SOURCE_DIR}/include/modemstuff/netstuff/netclient.h"
)

add_library(netstuff netserver.c ${HEADER_LIST})
target_compile_features(netstuff PUBLIC c_std_99)
target_include_directories(netstuff PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
