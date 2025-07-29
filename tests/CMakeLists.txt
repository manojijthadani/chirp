add_executable(test_chirp test_nice_threads.cpp)
target_link_libraries(test_chirp PRIVATE chirp)

target_include_directories(test_chirp PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME TestChirp COMMAND test_chirp)
