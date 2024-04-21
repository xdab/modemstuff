file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(aprscli ${SOURCES})

target_compile_features(aprscli PRIVATE c_std_99)
target_link_libraries(aprscli PRIVATE netstuff hamstuff)
