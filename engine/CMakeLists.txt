project(engine)

add_executable(gles-thingy
        src/game/direction.c
        src/game/direction.h
        src/game/enemy.c
        src/game/enemy.h
        src/game/game_state.c
        src/game/game_state.h
        src/game/input_state.h
        src/game/player.c
        src/game/player.h
        src/game/playfield.c
        src/game/playfield.h
        src/graphics/framebuffer.c
        src/graphics/framebuffer.h
        src/graphics/game_state_renderer.c
        src/graphics/game_state_renderer.h
        src/graphics/gles_utils.c
        src/graphics/gles_utils.h
        src/utils/array.h
        src/utils/logger.c
        src/utils/logger.h
        src/utils/running_average.c
        src/utils/running_average.h
        src/main.c)

target_link_libraries(gles-thingy glad glfw)