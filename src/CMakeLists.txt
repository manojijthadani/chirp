add_library(nice STATIC nice_threads.cpp nice_logger.cpp message_loop.cpp message.cpp nice_service.cpp)
target_include_directories(nice PRIVATE ${CMAKE_SOURCE_DIR}/inc)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE nice)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/inc)
