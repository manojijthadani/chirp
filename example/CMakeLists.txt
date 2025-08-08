add_executable(chirp-test main.cpp)
add_executable(chirp-demo chirp_example.cpp)
add_executable(chirp-factory-demo factory_example.cpp)

target_link_libraries(chirp-test PRIVATE chirp)
target_link_libraries(chirp-demo PRIVATE chirp)
target_link_libraries(chirp-factory-demo PRIVATE chirp)

target_include_directories(chirp-test 
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)
target_include_directories(chirp-demo 
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)
target_include_directories(chirp-factory-demo 
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)
