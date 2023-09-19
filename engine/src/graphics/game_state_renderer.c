#include "game_state_renderer.h"

#include <stddef.h>

#include "../utils/common.h"
#include "gles_utils.h"

typedef struct __attribute__((packed)) {
	uint8_t r, g, b, a;
} Rgba32Color;

static const Rgba32Color TILE_ATLAS_PALETTE[] = {
	{0x00, 0x00, 0x00, 0xff},
	{0x00, 0xaa, 0xaa, 0xff},
	{0xaa, 0x00, 0xaa, 0xff},
	{0xaa, 0xaa, 0xaa, 0xff}
};

static const uint8_t TILE_ATLAS_INDICES[] = {
		// Sea
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,

		// Land
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,

		// Player's Trace
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2,

		// Player's Head
		3, 3, 3, 3,
		3, 2, 2, 3,
		3, 2, 2, 3,
		3, 3, 3, 3,

		// Sea Enemy
		0, 3, 3, 0,
		3, 1, 1, 3,
		3, 1, 1, 3,
		0, 3, 3, 0,

		// Land Enemy
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
};

static const float QUAD_VERTICES[][2] = {
		{-1.f, -1.f},
		{-1.f,  1.f},
		{ 1.f,  1.f},
		{ 1.f,  1.f},
		{ 1.f, -1.f},
		{-1.f, -1.f}
};

static const char VERTEX_SHADER_SOURCE[] =
		"#version 300 es\n"
		"\n"
		"layout (location = 0) in vec2 aPos;\n"
		"\n"
		"void main() {\n"
		"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
		"}\n";

static const char FRAGMENT_SHADER_SOURCE[] =
		"#version 300 es\n"
		"\n"
		"precision mediump float;\n"
		"precision mediump usampler2D;\n"
		"\n"
		"uniform sampler2D texTileAtlas;\n"
		"uniform usampler2D texFieldState;\n"
		"\n"
		"out vec4 outColor;\n"
		"\n"
		"const uvec2 TILE_SIZE = uvec2(4);\n"
		"\n"
		"void main() {\n"
		"    uvec2 screenPos = uvec2(gl_FragCoord.xy);\n"
		"    uvec2 tile = screenPos / TILE_SIZE;\n"
		"    uvec2 tilePos = screenPos % TILE_SIZE;\n"
		"\n"
		"    uint tileVariant = texelFetch(texFieldState, ivec2(tile), 0).r;\n"
		"\n"
		"    uvec2 atlasOffset = uvec2(0, TILE_SIZE.y * tileVariant);\n"
		"    uvec2 atlasTexCoords = tilePos + atlasOffset;\n"
		"\n"
		"    outColor = texelFetch(texTileAtlas, ivec2(atlasTexCoords), 0);\n"
		"}\n";

static GLuint createTexture(int width, int height, GLenum internalFormat) {
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

static GLuint createTileAtlasTexture(void) {
	Rgba32Color pixelData[4 * 32] = {0};

	for (size_t i = 0; i < ARRAY_SIZE(TILE_ATLAS_INDICES); i++)
		pixelData[i] = TILE_ATLAS_PALETTE[TILE_ATLAS_INDICES[i]];

	GLuint texture = createTexture(4, 32, GL_RGBA8);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 32, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

static GLuint createShaderProgram(void) {
	GLuint program = GlesUtils_createShaderProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "texTileAtlas"), 0);
	glUniform1i(glGetUniformLocation(program, "texFieldState"), 1);

	glUseProgram(0);

	return program;
}

void GameStateRenderer_init(GameStateRenderer *this) {
	this->_bufferInitialized = false;

	this->_lastFieldWidth = 0;
	this->_lastFieldHeight = 0;

	this->_texFieldState = 0;
	this->_texTileAtlas = createTileAtlasTexture();
	this->_shaderProgram = createShaderProgram();
}

void GameStateRenderer_destroy(GameStateRenderer *this) {
	glDeleteProgram(this->_shaderProgram);
	glDeleteTextures(1, &this->_texTileAtlas);

	if (!this->_bufferInitialized)
		return;

	glDeleteTextures(1, &this->_texFieldState);
	Framebuffer_destroy(&this->_fb);
}

static void prepareForState(GameStateRenderer *this, const GameState *state) {
	const Playfield *field = &state->field;

	bool sizeMatch =
			field->width == this->_lastFieldWidth
			&& field->height == this->_lastFieldHeight;

	if (this->_bufferInitialized && sizeMatch)
		return;

	if (this->_bufferInitialized) {
		glDeleteTextures(1, &this->_texFieldState);
		Framebuffer_destroy(&this->_fb);
	} else {
		this->_bufferInitialized = true;
	}

	this->_lastFieldWidth = field->width;
	this->_lastFieldHeight = field->height;

	Framebuffer_init(&this->_fb, field->width * 4, field->height * 4);

	this->_texFieldState = createTexture(field->width, field->height, GL_R8UI);
}

static void uploadFieldStateToGpu(const GameState *state) {
	const Playfield *field = &state->field;

	glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			field->width, field->height,
			GL_RED_INTEGER, GL_UNSIGNED_BYTE,
			field->tiles
	);
}

GLuint GameStateRenderer_render(GameStateRenderer *this, const GameState *state) {
	prepareForState(this, state);

	Framebuffer_useForDrawing(&this->_fb, true);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->_texTileAtlas);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->_texFieldState);

	uploadFieldStateToGpu(state);

	glUseProgram(this->_shaderProgram);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(QUAD_VERTICES[0]), QUAD_VERTICES);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(QUAD_VERTICES) / sizeof(QUAD_VERTICES[0]));

	glDisableVertexAttribArray(0);
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);

	Framebuffer_useForDrawing(NULL, false);

	return Framebuffer_getTextureHandle(&this->_fb);
}