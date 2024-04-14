file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(audmod ${SOURCES})

target_compile_features(audmod PRIVATE c_std_99)
target_link_libraries(audmod PRIVATE configstuff netstuff modemstuff hamstuff)
