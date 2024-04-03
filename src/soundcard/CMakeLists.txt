set(HEADER_LIST "${modemstuff_SOURCE_DIR}/include/modemstuff/soundcard/soundcard.h")

add_library(soundcard soundcard.c ${HEADER_LIST})
target_compile_features(soundcard PUBLIC c_std_99)
target_include_directories(soundcard PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
