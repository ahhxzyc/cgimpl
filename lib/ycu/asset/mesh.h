#pragma once

#include "common.h"
#include <ycu/math.h>

#include <vector>
#include <memory>
#include <string>

YCU_ASSET_BEGIN

struct Vertex
{
    ycu::math::float3 position;
    ycu::math::float3 normal;
    ycu::math::float2 texcoords;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<ycu::math::uint3> indices;

    ycu::math::float3 translation;
    ycu::math::float3 scale;

    Mesh() = default;
    Mesh(const std::string &filename,
        const ycu::math::float3 &trans = ycu::math::float3(0), 
        const ycu::math::float3 &scale = ycu::math::float3(1));

    auto FaceCount() const
    {
        return indices.size();
    }
    auto VertexCount() const
    {
        return FaceCount() * 3;
    }

    auto modelMatrix() const
    {
        return ycu::math::mat4f::left_transform::translate(translation) * 
            ycu::math::mat4f::left_transform::scale(scale);
    }

    static Mesh* quad();
};

YCU_ASSET_END