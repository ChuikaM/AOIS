cmake_minimum_required(VERSION 3.25)
project(LAB1_AOIS_ii02707 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(core)
add_subdirectory(tui)

add_executable(${PROJECT_NAME} main.cpp)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
    app
)