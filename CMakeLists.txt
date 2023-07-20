cmake_minimum_required(VERSION 3.7)
project(SeaVision)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
message("TESTING: ${WITH_TESTS}")
