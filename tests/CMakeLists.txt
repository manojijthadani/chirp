# Main test executable for chirp library
add_executable(chirp_test chirp_test.cpp)

# ChirpFactory test executable
add_executable(chirp_factory_test chirp_factory_test.cpp)

# ChirpLogger test executable
add_executable(chirp_logger_test chirp_logger_test.cpp)


# Set C++ standard
set_target_properties(chirp_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(chirp_factory_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(chirp_logger_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Link with chirp library
target_link_libraries(chirp_test PRIVATE chirp)
target_link_libraries(chirp_factory_test PRIVATE chirp)
target_link_libraries(chirp_logger_test PRIVATE chirp)

# ChirpMessage test linking removed - merged into chirp_test

# Include directories
target_include_directories(chirp_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(chirp_factory_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(chirp_logger_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

# Enable coverage for test executables
if(ENABLE_COVERAGE)
    target_compile_options(chirp_test PRIVATE --coverage)
    target_link_libraries(chirp_test PRIVATE gcov)

    target_compile_options(chirp_factory_test PRIVATE --coverage)
    target_link_libraries(chirp_factory_test PRIVATE gcov)

    target_compile_options(chirp_logger_test PRIVATE --coverage)
    target_link_libraries(chirp_logger_test PRIVATE gcov)

endif()

# Add tests
add_test(NAME ChirpTest COMMAND chirp_test)
add_test(NAME ChirpFactoryTest COMMAND chirp_factory_test)
add_test(NAME ChirpLoggerTest COMMAND chirp_logger_test)

