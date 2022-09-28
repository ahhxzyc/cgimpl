#pragma once

#include "common.h"
#include <string>

class CubeTexture
{
public:
    CubeTexture(int size = 32);
    ~CubeTexture();

public:
    void Bind(uint32_t slot) const;
    void GenerateMipmap() const;
public:
    GLuint handle{};
private:
    int m_Size;
    GLenum m_DataFormat{};
};
