#pragma once

#include "common.h"
#include "../asset/image.h"
#include <memory>

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
    std::shared_ptr<ycu::asset::Image> image();
private:
    int mipmap_levels(int w, int h);
public:
    GLuint handle;
private:
    TextureDesc desc_;
};

YCU_OPENGL_END