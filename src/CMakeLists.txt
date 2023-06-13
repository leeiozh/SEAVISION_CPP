cmake_minimum_required(VERSION 3.7)
project(SeaVision)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MY_PROJECT_SOURCES *.h *.cpp *.hpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
if (WIN32)
    set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 -lrt ${CMAKE_CXX_STANDARD_LIBRARIES}")
endif (WIN32)

find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

add_library(SeaVision ${MY_PROJECT_SOURCES})

target_link_libraries(SeaVision PUBLIC Eigen3::Eigen PkgConfig::FFTW)

target_include_directories(SeaVision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
