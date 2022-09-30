#include "image.h"
#include "../log/log.h"
#include <stb_image.h>

YCU_ASSET_BEGIN

std::string extension(const std::string &path)
{
    auto dotIndex = path.find_last_of('.');
    if (dotIndex == std::string::npos)
        return "";
    return path.substr(dotIndex + 1);
}

Image::Image(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    auto ext = extension(path);
    if (ext == "hdr")
        data = reinterpret_cast<unsigned char*>(stbi_loadf(path.c_str(), &width, &height, &channels, 0));
    else
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        LOG_ERROR("Failed to load image.");
    }
}

Image::~Image()
{
    stbi_image_free(data);
}

YCU_ASSET_END