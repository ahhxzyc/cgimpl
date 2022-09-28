#pragma once

#include "common.h"
#include "texture.h"
#include <vector>
#include <memory>

YCU_OPENGL_BEGIN


class Framebuffer
{
public:
    Framebuffer(int w, int h);
    ~Framebuffer();

    void add_render_target(GLenum format);
    void set_render_target(GLint cubeTexture, int face, int rtIndex, int level = 0);
    std::shared_ptr<Texture> nth_render_target(int index);
    void bind();
    void unbind();

    void specify_drawbuffers();
    bool is_complete();
private:
    GLuint handle_;
    int width_, height_;
    std::vector<std::shared_ptr<Texture>> renderTargets_;
};


YCU_OPENGL_END