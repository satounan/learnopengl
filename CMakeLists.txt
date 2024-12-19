cmake_minimum_required(VERSION 3.30.0)
project(learnopengl VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(glfw3 REQUIRED)
add_subdirectory(glad)


add_executable(learnopengl main.cpp)

target_link_libraries(learnopengl PRIVATE glfw glad)