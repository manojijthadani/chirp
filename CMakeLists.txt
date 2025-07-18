cmake_minimum_required(VERSION 3.10)
project(nice VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add src and tests directories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)

# Enable testing
enable_testing()
