#pragma once

#include <ycu/math.h>

#include <vector>
#include <memory>
#include <string>

#define YCU_MESH_BEGIN namespace ycu::mesh {
#define YCU_MESH_END }

YCU_MESH_BEGIN

struct Vertex
{
    ycu::math::float3 position;
    ycu::math::float3 normal;
    ycu::math::float3 texcoords;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<ycu::math::uint3> indices;

    ycu::math::float3 translation;
    ycu::math::float3 scale;

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

    //ycu::math::mat4f ModelMatrix() const
    //{
    //    return glm::translate(glm::mat4(1.f), translation) * glm::scale(glm::mat4(1.f), scale);
    //}

};

YCU_MESH_END