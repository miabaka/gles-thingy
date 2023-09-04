project(glad)

add_library(glad STATIC src/gles2.c)

target_include_directories(glad PUBLIC include)