#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE

#include <glad/gles2.h>
#include <GLFW/glfw3.h>

#include "game/game_state.h"
#include "graphics/game_state_renderer.h"
#include "graphics/gles_utils.h"
#include "utils/common.h"
#include "utils/frame_counter.h"

typedef struct __attribute__((packed)) {
	float x, y, u, v;
} Vertex;

static const Vertex QUAD_VERTICES[] = {
		{-1.f, -1.f, 0.f, 1.f},
		{-1.f,  1.f, 0.f, 0.f},
		{ 1.f,  1.f, 1.f, 0.f},
		{ 1.f,  1.f, 1.f, 0.f},
		{ 1.f, -1.f, 1.f, 1.f},
		{-1.f, -1.f, 0.f, 1.f}
};

static const char VERTEX_SHADER_SOURCE[] =
		"#version 300 es\n"
		"\n"
		"layout (location = 0) in vec4 aPosTexCoords;\n"
		"\n"
		"out vec2 vTexCoords;\n"
		"\n"
		"void main() {\n"
		"    gl_Position = vec4(aPosTexCoords.xy, 0.0, 1.0);\n"
		"    vTexCoords = aPosTexCoords.zw;\n"
		"}\n";

static const char FRAGMENT_SHADER_SOURCE[] =
		"#version 300 es\n"
		"\n"
		"precision mediump float;\n"
		"\n"
		"uniform sampler2D texSource;\n"
		"\n"
		"in vec2 vTexCoords;\n"
		"out vec4 outColor;\n"
		"\n"
		"void main() {\n"
		"    outColor = texture(texSource, vTexCoords);\n"
		"}\n";

static void printGlDebugMessage(
		GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message, const void *userParam) {
	UNUSED(source);
	UNUSED(type);
	UNUSED(id);
	UNUSED(severity);
	UNUSED(length);
	UNUSED(userParam);

	printf("(gles debug) %s\n", message);
}

static void handleKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(mods);

	if (action != GLFW_PRESS)
		return;

	Direction newDirection;

	switch (key) {
		case GLFW_KEY_LEFT:
		case GLFW_KEY_A:
			newDirection = Direction_Left;
			break;

		case GLFW_KEY_RIGHT:
		case GLFW_KEY_D:
			newDirection = Direction_Right;
			break;

		case GLFW_KEY_UP:
		case GLFW_KEY_W:
			newDirection = Direction_Up;
			break;

		case GLFW_KEY_DOWN:
		case GLFW_KEY_S:
			newDirection = Direction_Down;
			break;

		default:
			return;
	}

	GameState *gameState = (GameState *) glfwGetWindowUserPointer(window);

	InputState inputState = {
			.direction = newDirection
	};

	GameState_setInputState(gameState, &inputState);
}

int main(void) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow *window = glfwCreateWindow(640, 400, "Thingy?!", NULL, NULL);

	if (!window) {
		const char *errorDesc;
		glfwGetError(&errorDesc);

		printf("Failed to create window: %s\n", errorDesc ? errorDesc : "");

		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGLES2(glfwGetProcAddress);

	glDebugMessageCallbackKHR(printGlDebugMessage, NULL);

	printf("Renderer: %s on %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

	GLuint passthroughProgram = GlesUtils_createShaderProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

	GameStateRenderer stateRenderer;
	GameStateRenderer_init(&stateRenderer);

	GameState state;
	GameState_init(&state, 80, 50);

	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback(window, handleKeyPress);

	double lastTime = glfwGetTime();

	FrameCounter frameCounter;
	FrameCounter_init(&frameCounter, 1.);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		double time = glfwGetTime();
		double timeDelta = time - lastTime;

		lastTime = time;

		FrameCounter_newFrame(&frameCounter, timeDelta);

		GameState_update(&state, (float) timeDelta);
		GameState_bakeDynamicObjects(&state);

		GLuint texGameRender = GameStateRenderer_render(&stateRenderer, &state);

		GameState_unbakeDynamicObjects(&state);

		glViewport(0, 0, 640, 400);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(QUAD_VERTICES[0]), QUAD_VERTICES);

		glBindTexture(GL_TEXTURE_2D, texGameRender);

		glUseProgram(passthroughProgram);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, ARRAY_SIZE(QUAD_VERTICES));

		glfwSwapBuffers(window);
	}

	GameStateRenderer_destroy(&stateRenderer);
	GameState_destroy(&state);

	glDeleteProgram(passthroughProgram);

	glfwTerminate();

	return 0;
}