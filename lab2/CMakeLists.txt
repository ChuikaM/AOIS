cmake_minimum_required(VERSION 3.25)
project(LAB2_AOIS_ii02707 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LAB1_BUILD_TESTS "Flag to build tests" ON)

add_subdirectory(src)
if(${LAB1_BUILD_TESTS})
    add_subdirectory(test)
endif()