add_executable(test_nice test_nice_threads.cpp)
target_link_libraries(test_nice PRIVATE nice)

target_include_directories(test_nice PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME TestNice COMMAND test_nice)
