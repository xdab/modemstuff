cmake_minimum_required(VERSION 3.14...3.29)

project(
  modemstuff
  VERSION 0.1
  DESCRIPTION "modemstuff"
  LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_STANDARD 99)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

find_package(PORTAUDIO)
if(PORTAUDIO_FOUND)
    message(STATUS "portaudio include dirs: ${PORTAUDIO_INCLUDE_DIRS}")
    message(STATUS "portaudio libraries: ${PORTAUDIO_LIBRARIES}")
else()
    message(FATAL_ERROR "portaudio not found") 
endif()

# Library code
add_subdirectory(src)

# Application code
add_subdirectory(apps)

