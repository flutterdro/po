
set(GLFW_BUILD_DOCS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(fmt)

add_subdirectory(glm)

set(BUILD_TESTING ON CACHE BOOL "" FORCE)
add_subdirectory(Catch2)

# add_subdirectory(freetype)
