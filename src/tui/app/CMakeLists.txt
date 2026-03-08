cmake_minimum_required(VERSION 3.25)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(${PROJECT_NAME} STATIC ${APP_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
    menu
    command
    table
)