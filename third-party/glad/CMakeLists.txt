project(glad)

add_library(glad STATIC
        include/glad/gles2.h
        include/KHR/khrplatform.h
        src/gles2.c)

target_include_directories(glad PUBLIC include)