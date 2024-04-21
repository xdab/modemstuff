file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(aprsdp ${SOURCES})

target_compile_features(aprsdp PRIVATE c_std_99)
target_link_libraries(aprsdp PRIVATE configstuff netstuff hamstuff)
