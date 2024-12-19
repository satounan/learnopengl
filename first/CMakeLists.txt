cmake_minimum_required(VERSION 3.30.0)
project(firstdemo VERSION 0.1.0 LANGUAGES C CXX)
find_package(glfw3 3.4 CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD 17)
# target_link_libraries(first PRIVATE glad)
add_subdirectory(../glad ${CMAKE_BINARY_DIR}/glad)
aux_source_directory(./src firstsrc)
include_directories(./include firstinc)

add_executable(firstdemo main.cpp)

target_link_libraries(firstdemo PRIVATE glfw glad)