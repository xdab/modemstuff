set(PORTAUDIO_ROOT_DIR
  "${PORTAUDIO_ROOT_DIR}"
  CACHE
  PATH
  "Directory to search for portaudio")

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_PORTAUDIO portaudio-2.0)
endif()

find_path(PORTAUDIO_INCLUDE_DIRS
  NAMES
  portaudio.h
  PATHS
  /usr/local/include
  /usr/include
  /opt/local/include
  HINTS
  ${PC_PORTAUDIO_INCLUDEDIR}
  ${PORTAUDIO_ROOT_DIR}
  )

find_library(PORTAUDIO_LIBRARIES
  NAMES
  portaudio
  PATHS
  /usr/local/lib
  /usr/lib
  /usr/lib64
  /opt/local/lib
  HINTS
  ${PC_PORTAUDIO_LIBDIR}
  ${PORTAUDIO_ROOT_DIR}
  )

mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

if(EXISTS ${PORTAUDIO_INCLUDE_DIRS}/portaudio.h)
  include(CheckCXXSourceCompiles)
  set(CMAKE_REQUIRED_INCLUDES_SAVED ${CMAKE_REQUIRED_INCLUDES})
  set(CMAKE_REQUIRED_INCLUDES ${PORTAUDIO_INCLUDE_DIRS})
  CHECK_CXX_SOURCE_COMPILES(
    "#include <portaudio.h>\nPaDeviceIndex pa_find_device_by_name(const char *name); int main () {return 0;}"
    PORTAUDIO2_FOUND)
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVED})
  unset(CMAKE_REQUIRED_INCLUDES_SAVED)
  if(PORTAUDIO2_FOUND)
    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(PORTAUDIO DEFAULT_MSG PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)
  else(PORTAUDIO2_FOUND)
    message(STATUS "portaudio.h not compatible (requires API 2.0)")
    set(PORTAUDIO_FOUND FALSE)
  endif(PORTAUDIO2_FOUND)
endif()
