add_library(chirp SHARED chirp_threads.cpp 
                         chirp_logger.cpp 
                         message_loop.cpp 
                         message.cpp 
                         chirp.cpp 
                         chirp_impl.cpp)
                         
target_include_directories(chirp PUBLIC ${CMAKE_SOURCE_DIR}/inc PRIVATE ${CMAKE_SOURCE_DIR}/src)


