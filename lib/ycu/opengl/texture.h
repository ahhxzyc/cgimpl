#pragma once

#include "common.h"

YCU_OPENGL_BEGIN

struct TextureDesc
{
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
    GLint minFilter;
    GLint magFilter;
    GLint wrapS;
    GLint wrapT;

    static TextureDesc without_mipmap(int w, int h, GLenum format);
    static TextureDesc with_mipmap(int w, int h, GLenum format);
};

class Texture
{
public:
    Texture(const TextureDesc &desc);
    void set_data(int w, int h, int channels, void *data, GLenum dataType);
    void bind(int slot);
    auto handle() const {return handle_;}
private:
    int mipmap_levels(int w, int h);
private:
    GLuint handle_;
};

YCU_OPENGL_END