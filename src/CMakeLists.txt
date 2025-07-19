add_library(nice SHARED nice_threads.cpp 
                         nice_logger.cpp 
                         message_loop.cpp 
                         message.cpp 
                         nice_service.cpp 
                         nice_service_impl.cpp)
                         
target_include_directories(nice PUBLIC ${CMAKE_SOURCE_DIR}/inc PRIVATE ${CMAKE_SOURCE_DIR}/src)


