project(engine)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

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
		src/game/playfield_fill.c
		src/graphics/framebuffer.c
		src/graphics/framebuffer.h
		src/graphics/game_state_renderer.c
		src/graphics/game_state_renderer.h
		src/graphics/gles_utils.c
		src/graphics/gles_utils.h
		src/utils/common.h
		src/main.c)

target_compile_options(gles-thingy PRIVATE
		-Wall -Wextra -Wpedantic
		-Wno-newline-eof -Wno-fixed-enum-extension -Wno-gnu-empty-initializer)

target_link_libraries(gles-thingy glad glfw)