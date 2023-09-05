cmake_minimum_required(VERSION 3.25)

project(gles_thingy)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory(third-party/genvector)
add_subdirectory(third-party/glad)
add_subdirectory(third-party/glfw)

add_subdirectory(engine)