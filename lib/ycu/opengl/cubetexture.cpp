#include "cubetexture.h"

#include <cmath>

CubeTexture::CubeTexture(int size) : size_(size)
{
    GLCALL(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &handle));
    int levels = static_cast<int>(std::log2(size)) + 1;
    GLCALL(glTextureStorage2D(handle, levels, GL_RGBA16F, size, size));

    GLCALL(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

CubeTexture::~CubeTexture()
{
    GLCALL(glDeleteTextures(1, &handle));
}

void CubeTexture::bind(uint32_t slot) const
{
    GLCALL(glBindTextureUnit(slot, handle));
}

void CubeTexture::generate_mipmap() const
{
    GLCALL(glGenerateTextureMipmap(handle));
}
