#pragma once

#include "common.h"
#include <string>

YCU_ASSET_BEGIN

class Image
{
public:
    Image(const std::string &path);
    ~Image();
public:
    int width, height, channels;
    unsigned char *data = nullptr;
};


YCU_ASSET_END