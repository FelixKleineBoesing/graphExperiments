cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++20")

project(networkcpp)

add_library(networkcpp STATIC
    src/astar.cpp
    src/multidigraph.cpp
)

target_include_directories(networkcpp
    PUBLIC
        ./include
)


# Add an executable
add_executable(networkcpp_main src/main.cpp)

target_link_libraries( networkcpp_main
    PRIVATE
        networkcpp
)
target_compile_features(networkcpp_main PRIVATE cxx_std_20)


