add_executable(nice-services main.cpp)
target_link_libraries(nice-services PRIVATE nice)
target_include_directories(nice-services PRIVATE ${CMAKE_SOURCE_DIR}/inc)
