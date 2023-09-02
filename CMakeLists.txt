cmake_minimum_required(VERSION 3.7)
project(SeaVision)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(tests)
enable_testing()