#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE

#include <glad/gles2.h>
#include <GLFW/glfw3.h>

#include "game/game_state.h"
#include "graphics/game_state_renderer.h"
#include "graphics/gles_utils.h"

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
        "\n"
        "uniform sampler2D texSource;\n"
        "\n"
        "out vec4 outColor;\n"
        "\n"
        "void main() {\n"
        "    outColor = texture2D(texSource, vec2(gl_FragCoord) / vec2(640, 400));\n"
        "}\n";

static void printGlDebugMessage(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar *message, const void *userParam) {
    printf("(gles debug) %s\n", message);
}

static void handleKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

    GameState_applyInputState(gameState, &inputState);
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(640, 400, "Thingy?!", NULL, NULL);

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double time = glfwGetTime();
        double timeDelta = time - lastTime;

        lastTime = time;

        GameState_update(&state, (float) timeDelta);

        GLuint texGameRender = GameStateRenderer_render(&stateRenderer, &state);

        glViewport(0, 0, 640, 400);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(QUAD_VERTICES[0]), QUAD_VERTICES);

        glBindTexture(GL_TEXTURE_2D, texGameRender);

        glUseProgram(passthroughProgram);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(QUAD_VERTICES) / sizeof(QUAD_VERTICES[0]));

        glfwSwapBuffers(window);
    }

    GameStateRenderer_destroy(&stateRenderer);
    GameState_destroy(&state);

    glDeleteProgram(passthroughProgram);

    glfwTerminate();

    return 0;
}