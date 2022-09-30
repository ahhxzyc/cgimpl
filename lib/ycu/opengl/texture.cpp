#include "texture.h"
#include <algorithm>

YCU_OPENGL_BEGIN


Texture::Texture(const TextureDesc &desc)
{
    GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &handle));

    int levels = 1;
    if (desc.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
        desc.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        desc.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        desc.minFilter == GL_LINEAR_MIPMAP_LINEAR)
    {
        levels = mipmap_levels(desc.width, desc.height);
    }

    GLCALL(glTextureStorage2D(handle, levels, desc.internalFormat, desc.width, desc.height));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, desc.minFilter));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, desc.magFilter));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, desc.wrapS));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_WRAP_T, desc.wrapT));
}

void Texture::set_data(int w, int h, int channels, void *data, GLenum dataType)
{
    GLenum format{};
    switch (channels)
    {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: ;
    }
    GLCALL(glTextureSubImage2D(handle, 0, 0, 0, w, h, format, dataType, data));
    GLCALL(glGenerateTextureMipmap(handle));
}

int Texture::mipmap_levels(int w, int h)
{
    w = std::max(w, h);
    int ret = 0;
    while (w)
    {
        w >>= 1;
        ret ++ ;
    }
    return ret;
}

void Texture::bind(int slot)
{
    GLCALL(glBindTextureUnit(slot, handle));
}

TextureDesc TextureDesc::without_mipmap(int w, int h, GLenum format)
{
    TextureDesc desc;
    desc.width = w;
    desc.height = h;
    desc.internalFormat = format;
    desc.minFilter = GL_NEAREST;
    desc.magFilter = GL_NEAREST;
    desc.wrapS = GL_CLAMP_TO_EDGE;
    desc.wrapT = GL_CLAMP_TO_EDGE;
    return desc;
}

TextureDesc TextureDesc::with_mipmap(int w, int h, GLenum format)
{
    TextureDesc desc;
    desc.width = w;
    desc.height = h;
    desc.internalFormat = format;
    desc.minFilter = GL_NEAREST_MIPMAP_NEAREST;
    desc.magFilter = GL_NEAREST;
    desc.wrapS = GL_REPEAT;
    desc.wrapT = GL_REPEAT;
    return desc;
}

YCU_OPENGL_END