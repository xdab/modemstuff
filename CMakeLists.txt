cmake_minimum_required(VERSION 3.14...3.29)

project(
  modemstuff
  VERSION 0.1
  DESCRIPTION "modemstuff"
  LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_STANDARD 99)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Library code
add_subdirectory(src)

# Application code
add_subdirectory(apps)

