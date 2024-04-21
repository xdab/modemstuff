file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(aprsig ${SOURCES})

target_compile_features(aprsig PRIVATE c_std_99)
target_link_libraries(aprsig PRIVATE configstuff netstuff hamstuff)
