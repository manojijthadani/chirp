add_executable(chirp-demo main.cpp)
target_link_libraries(chirp-demo PRIVATE chirp)
target_include_directories(chirp 
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)
