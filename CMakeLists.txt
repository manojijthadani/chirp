cmake_minimum_required(VERSION 3.10)
project(chirp VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add src directory (always built)
add_subdirectory(src)

# Option to build examples (disabled by default)
option(BUILD_EXAMPLES "Build example programs" OFF)

# Add example directory only if BUILD_EXAMPLES is enabled
if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# Option to build tests (disabled by default)
option(BUILD_TESTS "Build unit tests" OFF)

# Add tests directory only if BUILD_TESTS is enabled
if(BUILD_TESTS)
    add_subdirectory(tests)
    # Enable testing
    enable_testing()
endif()
