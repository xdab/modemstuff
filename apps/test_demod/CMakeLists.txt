file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(test_demod ${SOURCES})

target_compile_features(test_demod PRIVATE c_std_99)
target_link_libraries(test_demod PRIVATE configstuff modemstuff hamstuff)
