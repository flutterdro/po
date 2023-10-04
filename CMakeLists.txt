cmake_minimum_required(VERSION 3.20)
project(prmai VERSION 0.5.0)

add_subdirectory(vendor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB PROJECT_HEADERS src/*.h
     src/rtw/*.h
)
file(GLOB PROJECT_SOURCES src/*.cpp
     src/rtw/*.cpp
     src/game/logic/*.cpp
     src/game/*.cpp
     src/logger/logger.cpp
     src/game/engine/*.cpp
     src/game/engine/io/*.cpp
     src/game/ai/*.cpp
     src/game/ui/*.cpp
     src/game/utilities/*.cpp
     vendor/glad/src/gl.c
)
add_executable(${PROJECT_NAME}
     ${PROJECT_SOURCES}
)



target_compile_definitions(${PROJECT_NAME}
     PRIVATE $<$<CONFIG:DEBUG>:DEBUG_>)

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
     -Wdouble-promotion
     -fsanitize=address>
     $<$<CONFIG:RELEASE>:-flto>
)
target_link_options(${PROJECT_NAME}
     PRIVATE
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
     PRIVATE Catch2::Catch2WithMain
     # PRIVATE freetype
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/vendor/Catch2/extras)
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

# add_custom_target(copy_shader_files
# ${CMAKE_COMMAND}
# -P ${CMAKE_SOURCE_DIR}/copyfiles.cmake
# COMMENT "Copying Files for target: prmAI" VERBATIM
# )
# add_dependencies(${PROJECT_NAME} copy_shader_files)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_directory
     ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E create_symlink
     ${CMAKE_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
