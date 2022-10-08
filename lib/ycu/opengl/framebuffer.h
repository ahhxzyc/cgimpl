#pragma once

#include "common.h"
#include "texture.h"
#include <vector>
#include <memory>
#include <map>

YCU_OPENGL_BEGIN


class Framebuffer
{
public:
    Framebuffer(int w, int h);
    ~Framebuffer();

    void add_render_target(GLenum format);
    void set_render_target(int index, GLint cubeTexture, int face, int level = 0);
    void resize(int w, int h);
    std::shared_ptr<Texture> nth_render_target(int index);
    void bind();
    void unbind();

    void specify_drawbuffers();
    bool is_complete();
private:
    GLuint handle_;
    GLuint depthStencil_;
    int width_, height_;
    inline static const int maxNumRTs = 8;
    int numRTs = 0;
    std::shared_ptr<Texture> renderTargets_[maxNumRTs];
};


YCU_OPENGL_END