add_executable(app app.c)
target_compile_features(app PRIVATE c_std_99)
target_link_libraries(app PRIVATE soundcard)
