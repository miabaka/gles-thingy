#include "framebuffer.h"

static GLuint createTexture(int width, int height);

static GLuint createFramebuffer(GLuint texture);

void Framebuffer_init(Framebuffer *this, int width, int height) {
    this->_textureHandle = createTexture(width, height);
    this->_handle = createFramebuffer(this->_textureHandle);
    this->_width = width;
    this->_height = height;
}

void Framebuffer_destroy(Framebuffer *this) {
    glDeleteFramebuffers(1, &this->_textureHandle);
    glDeleteTextures(1, &this->_textureHandle);
}

void Framebuffer_useForDrawing(Framebuffer *this, bool adjustViewport) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this ? this->_handle : 0);

    if (!(adjustViewport && this))
        return;

    glViewport(0, 0, this->_width, this->_height);
}

GLuint Framebuffer_getTextureHandle(Framebuffer *this) {
    return this->_textureHandle;
}

static GLuint createTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

static GLuint createFramebuffer(GLuint texture) {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}