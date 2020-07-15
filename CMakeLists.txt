cmake_minimum_required(VERSION 3.0.0)

project(RLL 
    VERSION 1.0.0 
    LANGUAGES CXX
)

add_executable(RLL_example src/example.cpp)
target_include_directories(RLL_example PRIVATE include/)

if(NOT WIN32)
    target_link_libraries(RLL_example PRIVATE dl)
endif()

set_target_properties(RLL_example PROPERTIES CXX_STANDARD 98)