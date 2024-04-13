file(GLOB_RECURSE HEADER_LIST "${modemstuff_SOURCE_DIR}/include/netstuff/*.h")
file(GLOB_RECURSE SOURCE_LIST "*.c")

add_library(netstuff ${SOURCE_LIST} ${HEADER_LIST})
target_compile_features(netstuff PUBLIC c_std_99)
target_include_directories(netstuff PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
