cmake_minimum_required(VERSION 3.24)
project(prmai VERSION 0.5.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COLOR_DIAGNOSTICS ON CACHE BOOL "" FORCE)
set(CMAKE_COLOR_MAKEFILE ON CACHE BOOL "" FORCE)

set($ENV{ASAN_OPTIONS} "detect_container_overflow=0")

add_subdirectory(vendor)
add_subdirectory(tests)

file(GLOB PROJECT_HEADERS src/*.h
     src/rtw/*.h
)
file(GLOB PROJECT_SOURCES src/*.cpp
     # src/rtw/*.cpp
     src/game/logic/*.cpp
     src/game/*.cpp
     src/logger/logger.cpp
     src/game/engine/*.cpp
     src/game/engine/io/*.cpp
     src/game/engine/basic-shapes/*.cpp
     src/game/ai/*.cpp
     src/game/ui/*.cpp
     src/game/utilities/*.cpp
     vendor/glad/src/gl.c
)
add_executable(${PROJECT_NAME}
     ${PROJECT_SOURCES}
)

target_compile_definitions(${PROJECT_NAME}
     PUBLIC $<$<CONFIG:DEBUG>:DEBUG_ >)

target_compile_options(${PROJECT_NAME}
PRIVATE $<$<CONFIG:DEBUG>:
     -Wall
     -Wextra
     -Wshadow
     -Wmove
     -Wnon-virtual-dtor
     -Wold-style-cast
     -Wcast-align
     -Wunused
     -Woverloaded-virtual
     -Wpedantic
     -Wconversion
     -Wsign-conversion
     -Wnull-dereference
     -Wdouble-promotion>
PUBLIC $<$<CONFIG:DEBUG>:
     -fno-omit-frame-pointer
     -fsanitize=address
     -fsanitize=undefined>
     $<$<CONFIG:RELEASE>:-flto>
)
target_link_options(${PROJECT_NAME}
     PUBLIC
     $<$<CONFIG:DEBUG>:-fsanitize=address>
)
target_include_directories(${PROJECT_NAME}
     PRIVATE
     vendor/fmt/include
     vendor/glfw/include
     vendor/glad/include
     vendor/stb
)
target_link_libraries(${PROJECT_NAME}
     PRIVATE fmt
     PUBLIC glfw
     PRIVATE glm
     # PRIVATE freetype
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_directory
     ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E create_symlink
     ${CMAKE_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

enable_testing()