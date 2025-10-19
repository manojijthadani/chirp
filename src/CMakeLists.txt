add_library(chirp SHARED chirp_threads.cpp 
                        chirp_logger.cpp 
                        message_loop.cpp 
                        message.cpp 
                        chirp.cpp 
                        chirp_impl.cpp
                        chirp_factory_impl.cpp
                        chirp_timer.cpp
                        timer_mgr.cpp)

# Set version information for the library
set_target_properties(chirp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "chirp"
)
                         
target_include_directories(chirp PUBLIC ${CMAKE_SOURCE_DIR}/inc PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Enable coverage for library source files
if(ENABLE_COVERAGE)
    target_compile_options(chirp PRIVATE --coverage)
    target_link_libraries(chirp PRIVATE gcov)
endif()


