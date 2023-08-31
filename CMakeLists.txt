cmake_minimum_required(VERSION 3.10)
project(prmai VERSION 0.5.0)

add_subdirectory(vendor/fmt)

add_subdirectory(vendor/glfw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_compile_options(
     -Wall 
     -Wextra 
     -Wshadow
     -Wnon-virtual-dtor
     -Wold-style-cast
     -Wcast-align
     -Wunused
     -Woverloaded-virtual
     -Wpedantic 
     -Wconversion
     -Wsign-conversion 
     -Wnull-dereference
     -Wdouble-promotion
     #-Wformat=2
     #-Werror
)

file(GLOB PROJECT_HEADERS src/*.h
                          src/rtw/*.h
)
file(GLOB PROJECT_SOURCES src/*.cpp
                          src/rtw/*.cpp
                          vendor/glad/src/gl.c
)
add_executable(${PROJECT_NAME}
${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME} 
PUBLIC vendor/fmt/include
PUBLIC vendor/glfw/include
PUBLIC vendor/glad/include
)

target_link_libraries(${PROJECT_NAME} 
fmt
glfw
)