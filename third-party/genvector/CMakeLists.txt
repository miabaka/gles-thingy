project(genvector)

add_library(genvector STATIC
        include/genvector/routines/internal/common.h
        include/genvector/routines/iterator/iterator.h
        include/genvector/routines/global.h
        include/genvector/routines/shared.h
        include/genvector/genvector.h
        src/genvector.c
        src/iterator.c)

target_include_directories(genvector PUBLIC include)