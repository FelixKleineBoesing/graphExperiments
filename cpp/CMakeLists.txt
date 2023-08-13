cmake_minimum_required(VERSION 3.10)

project(networkcpp LANGUAGES CXX)

add_library(networkcpp STATIC
    src/astar.cpp
    src/multidigraph.cpp
)

target_include_directories(networkcpp
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# try conditional compilation
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

# check results and add flag
if(COMPILER_SUPPORTS_CXX11)#
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)#
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Add an executable
add_executable(networkcpp_main src/main.cpp)

target_link_libraries( networkcpp_main
    PRIVATE
        networkcpp
)

target_compile_features(networkcpp_main PRIVATE cxx_std_17)
