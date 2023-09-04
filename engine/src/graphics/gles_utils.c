#include "gles_utils.h"

#ifdef _WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    InfoLogSourceType_Shader,
    InfoLogSourceType_ShaderProgram
} InfoLogSourceType;

static const char *getShaderTypeName(GLenum type) {
    switch (type) {
        case GL_FRAGMENT_SHADER:
            return "fragment";

        case GL_VERTEX_SHADER:
            return "vertex";

        default:
            return "unknown";
    }
}

static void printInfoLog(GLuint source, InfoLogSourceType sourceType) {
    PFNGLGETSHADERIVPROC getIv;
    PFNGLGETSHADERINFOLOGPROC getInfoLog;

    switch (sourceType) {
        case InfoLogSourceType_Shader:
            getIv = glGetShaderiv;
            getInfoLog = glGetShaderInfoLog;
            break;

        case InfoLogSourceType_ShaderProgram:
            getIv = glGetProgramiv;
            getInfoLog = glGetProgramInfoLog;
            break;

        default:
            return;
    }

    GLint infoLogLength = 0;
    getIv(source, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength < 1)
        return;

    char *infoLogBuf = alloca(infoLogLength);

    if (!infoLogBuf)
        return;

    GLsizei logBytesWritten = 0;
    getInfoLog(source, infoLogLength, &logBytesWritten, infoLogBuf);

    infoLogBuf[logBytesWritten] = '\0';

    puts(infoLogBuf);
}

static bool checkShaderCompilationStatus(GLuint shader, GLenum shaderType) {
    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus)
        return true;

    printf("-- %s shader compilation failed --\n", getShaderTypeName(shaderType));
    printInfoLog(shader, InfoLogSourceType_Shader);

    return false;
}

static bool checkShaderProgramLinkageStatus(GLuint program) {
    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus)
        return true;

    puts("-- shader program linkage failed --");
    printInfoLog(program, InfoLogSourceType_ShaderProgram);

    return false;
}

static void attachShaderToProgram(GLuint program, GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    if (checkShaderCompilationStatus(shader, type))
        glAttachShader(program, shader);

    glDeleteShader(shader);
}

GLuint GlesUtils_createShaderProgram(const char *vertexSource, const char *fragmentSource) {
    GLuint program = glCreateProgram();

    attachShaderToProgram(program, GL_VERTEX_SHADER, vertexSource);
    attachShaderToProgram(program, GL_FRAGMENT_SHADER, fragmentSource);

    glLinkProgram(program);
    checkShaderProgramLinkageStatus(program);

    return program;
}