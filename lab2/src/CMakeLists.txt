cmake_minimum_required(VERSION 3.25)
project(src LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(network)
add_subdirectory(printer)
add_subdirectory(matrix/src)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    network
    matrix
    report_printer
)
