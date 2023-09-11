
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(fmt)

add_subdirectory(glm)
# set(TGUI_BACKEND GLFW_OPENGL3)
# set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
# set(GLFW_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/glfw/src/libglfw3.a)
# add_subdirectory(TGUI)
# add_dependencies(tgui glfw)
# target_link_libraries(tgui PUBLIC glfw)
# find_package(TGUI)