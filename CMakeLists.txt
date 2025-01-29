cmake_minimum_required(VERSION 3.12)

project(
    nd_argparser
    VERSION 1.0
    DESCRIPTION "Simple C++ command line argument parser"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)


add_subdirectory(lib)
add_subdirectory(bin)


enable_testing()
add_subdirectory(tests)
