file(GLOB_RECURSE HEADER_LIST "${modemstuff_SOURCE_DIR}/include/modemstuff/*.h")
file(GLOB_RECURSE SOURCE_LIST "*.c")

add_library(modemstuff ${SOURCE_LIST} ${HEADER_LIST})
target_compile_features(modemstuff PUBLIC c_std_99)
target_include_directories(modemstuff PUBLIC ../../include)
target_link_libraries(modemstuff PUBLIC m)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
