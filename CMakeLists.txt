cmake_minimum_required(VERSION 3.10)
project(prmai VERSION 0.5.0)

add_subdirectory(vendor/fmt)

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
)
file(GLOB PROJECT_SOURCES src/*.cpp
)
add_executable(${PROJECT_NAME}
${PROJECT_SOURCES} ${PROJECT_HEADERS}
)

target_include_directories(${PROJECT_NAME} 
PUBLIC vendor/fmt/include
)

target_link_libraries(${PROJECT_NAME} 
fmt
)