file(GLOB_RECURSE HEADER_LIST "${modemstuff_SOURCE_DIR}/include/configstuff/*.h")
file(GLOB_RECURSE SOURCE_LIST "*.c")

add_library(configstuff ${SOURCE_LIST} ${HEADER_LIST})
target_compile_features(configstuff PUBLIC c_std_99)
target_include_directories(configstuff PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
