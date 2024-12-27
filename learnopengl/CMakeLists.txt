cmake_minimum_required(VERSION 3.29.0)
project(hellotriangle VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(glfw3 3.4 CONFIG REQUIRED)
find_package(stb REQUIRED)

add_subdirectory(../glad ${CMAKE_BINARY_DIR}/glad)
include_directories(${GLFW_INCLUDE_DIRS})
file(GLOB abstractclass "src/*.cpp")
 
add_executable(hellotriangle src/main.cpp ${abstractclass})

target_include_directories(hellotriangle PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(hellotriangle 
                            PRIVATE 
                            glfw 
                            glad
                    ) 