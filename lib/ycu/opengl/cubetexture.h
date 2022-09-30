#pragma once

#include "common.h"
#include <string>

class CubeTexture
{
public:
    CubeTexture(int size = 32);
    ~CubeTexture();

public:
    void bind(uint32_t slot) const;
    void generate_mipmap() const;
public:
    GLuint handle{};
private:
    int size_;
};
