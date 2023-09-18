#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../game/game_state.h"
#include "framebuffer.h"

#include <glad/gles2.h>

typedef struct {
	Framebuffer _fb;
	GLuint _texFieldState;
	GLuint _texTileAtlas;
	GLuint _shaderProgram;
	uint8_t _lastFieldWidth;
	uint8_t _lastFieldHeight;
	bool _bufferInitialized;
} GameStateRenderer;

void GameStateRenderer_init(GameStateRenderer *);

void GameStateRenderer_destroy(GameStateRenderer *);

GLuint GameStateRenderer_render(GameStateRenderer *, const GameState *state);