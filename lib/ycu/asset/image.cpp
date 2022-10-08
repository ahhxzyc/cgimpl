#include "image.h"
#include "../log/log.h"
#include <stb_image.h>
#include <stb_image_write.h>

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
    {
        auto dataf = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
        for (int i = 0; i < width * height * channels; i ++ )
            dataf[i] = std::min(dataf[i], 6e4f);
        data = reinterpret_cast<unsigned char*>(dataf);
    }
    else
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        LOG_ERROR("Failed to load image.");
    }
}

Image::Image(int w, int h, int c) : width(w), height(h), channels(c)
{
    int numBytes = w * h * c;
    data = new unsigned char[numBytes];
}

Image::~Image()
{
    stbi_image_free(data);
}

void Image::save(const std::string &path)
{
    stbi_flip_vertically_on_write(true);
    auto success = stbi_write_png(path.c_str(), width, height, channels, data, 0);
    if (!success)
    {
        LOG_INFO("Failed to write image {}", path);
    }
}

YCU_ASSET_END