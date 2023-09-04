#pragma once

#include <stdbool.h>
#include "glad/gles2.h"

typedef struct {
    GLuint _handle;
    GLuint _textureHandle;
    int _width;
    int _height;
} Framebuffer;

void Framebuffer_init(Framebuffer *, int width, int height);

void Framebuffer_destroy(Framebuffer *);

void  Framebuffer_useForDrawing(Framebuffer *, bool adjustViewport);

GLuint Framebuffer_getTextureHandle(Framebuffer *);