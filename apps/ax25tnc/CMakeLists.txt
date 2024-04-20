file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(ax25tnc ${SOURCES})

target_compile_features(ax25tnc PRIVATE c_std_99)
target_link_libraries(ax25tnc PRIVATE configstuff netstuff modemstuff hamstuff)
