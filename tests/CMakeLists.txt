# Main test executable for chirp library
add_executable(chirp_test chirp_test.cpp)

# ChirpFactory test executable
add_executable(chirp_factory_test chirp_factory_test.cpp)

# ChirpLogger test executable
add_executable(chirp_logger_test chirp_logger_test.cpp)

# ChirpTimer test executable
add_executable(chirp_timer_test chirp_timer_test.cpp)

# ChirpWatchDog test executable
add_executable(chirp_watchdog_test chirp_watchdog_test.cpp)

# Chirp benchmark executable
add_executable(chirp_benchmark chirp_benchmark.cpp)

# Message throughput benchmark executable
add_executable(message_throughput_benchmark message_throughput_benchmark.cpp)

# Timer and Watchdog benchmark executable
add_executable(timer_watchdog_benchmark timer_watchdog_benchmark.cpp)


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

set_target_properties(chirp_timer_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(chirp_watchdog_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(chirp_benchmark PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(message_throughput_benchmark PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(timer_watchdog_benchmark PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)


# Link with chirp library
target_link_libraries(chirp_test PRIVATE chirp)
target_link_libraries(chirp_factory_test PRIVATE chirp)
target_link_libraries(chirp_logger_test PRIVATE chirp)
target_link_libraries(chirp_timer_test PRIVATE chirp)
target_link_libraries(chirp_watchdog_test PRIVATE chirp)
target_link_libraries(chirp_benchmark PRIVATE chirp)
target_link_libraries(message_throughput_benchmark PRIVATE chirp)
target_link_libraries(timer_watchdog_benchmark PRIVATE chirp)

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

target_include_directories(chirp_timer_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(chirp_watchdog_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(chirp_benchmark PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(message_throughput_benchmark PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(timer_watchdog_benchmark PRIVATE 
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

    target_compile_options(chirp_timer_test PRIVATE --coverage)
    target_link_libraries(chirp_timer_test PRIVATE gcov)

    target_compile_options(chirp_watchdog_test PRIVATE --coverage)
    target_link_libraries(chirp_watchdog_test PRIVATE gcov)

    target_compile_options(chirp_benchmark PRIVATE --coverage)
    target_link_libraries(chirp_benchmark PRIVATE gcov)

    target_compile_options(message_throughput_benchmark PRIVATE --coverage)
    target_link_libraries(message_throughput_benchmark PRIVATE gcov)

    target_compile_options(timer_watchdog_benchmark PRIVATE --coverage)
    target_link_libraries(timer_watchdog_benchmark PRIVATE gcov)

endif()

# Add tests
add_test(NAME ChirpTest COMMAND chirp_test)
add_test(NAME ChirpFactoryTest COMMAND chirp_factory_test)
add_test(NAME ChirpLoggerTest COMMAND chirp_logger_test)
add_test(NAME ChirpTimerTest COMMAND chirp_timer_test)
add_test(NAME ChirpWatchDogTest COMMAND chirp_watchdog_test)

