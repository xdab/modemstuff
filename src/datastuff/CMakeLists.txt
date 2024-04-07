set(HEADER_LIST 
  "${modemstuff_SOURCE_DIR}/include/modemstuff/datastuff/linkedbuffer.h"
)

add_library(datastuff linkedbuffer.c ${HEADER_LIST})
target_compile_features(datastuff PUBLIC c_std_99)
target_include_directories(datastuff PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
