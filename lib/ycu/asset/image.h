#pragma once

#include "common.h"
#include <string>

YCU_ASSET_BEGIN

class Image
{
public:
    Image(int w, int h, int c);
    Image(const std::string &path);
    ~Image();
    void save(const std::string &path);
    int numBytes() const
    {
        return width * height * channels;
    }
public:
    int width, height, channels;
    unsigned char *data = nullptr;
};


YCU_ASSET_END