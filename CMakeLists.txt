cmake_minimum_required(VERSION 3.10)
project(chirp VERSION 2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Handle multi-config generators (e.g., MSVC)
foreach(OUTPUTCONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# Option to enable coverage (disabled by default)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Add src directory (always built)
add_subdirectory(src)

# Option to build examples (enabled by default)
option(BUILD_EXAMPLES "Build example programs" ON)

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
