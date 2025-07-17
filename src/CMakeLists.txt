add_library(nice STATIC nice_threads.cpp nice_logger.cpp)

add_executable(main main.cpp
        message_loop.h
        message.h
        message_loop.cpp
        message.cpp
        nice_service.h
        nice_service.cpp
        nice_logger.h)
target_link_libraries(main PRIVATE nice)
