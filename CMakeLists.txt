cmake_minimum_required(VERSION 3.17)
project(mySemProject CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build tests" OFF)

if (BUILD_TESTS)
    add_subdirectory(extlibs/gtest)
else()
	add_subdirectory(extlibs/pybind11)
endif()
 