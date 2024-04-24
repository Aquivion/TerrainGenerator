cmake_minimum_required(VERSION 3.15)
project(test CXX)

find_package(fmt REQUIRED)
find_package(SDL2 REQUIRED)
find_package(freetype REQUIRED)
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} fmt::fmt SDL2::SDL2 SDL2::SDL2main Freetype::Freetype GLEW::GLEW)