include_directories(${PLIBSYS_INCLUDE_DIRS} ${PORTAUDIO_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(netaud ${SOURCES}) 

target_compile_features(netaud PRIVATE c_std_99)
target_include_directories(netaud PRIVATE ${PLIBSYS_INCLUDE_DIRS} ${PORTAUDIO_INCLUDE_DIRS}) 
target_link_libraries(netaud PRIVATE m ${PLIBSYS_LIBRARIES} ${PORTAUDIO_LIBRARIES})
