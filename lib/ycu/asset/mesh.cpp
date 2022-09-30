#include "Mesh.h"
#include "ycu/log/log.h"

#include <string>
#include <tiny_obj_loader.h>

using ycu::math::float3;
using ycu::math::float2;
using ycu::math::uint3;

YCU_ASSET_BEGIN

Mesh::Mesh(const std::string &filename,
    const float3 &trans , const float3 &scale )
    : translation(trans), scale(scale)
{
    // Get obj file directory
    auto slash_pos = filename.find_last_of('/');
    auto dir = filename.substr(0, slash_pos);
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = dir;

    // Error and warning
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename, reader_config))
    {
        if (!reader.Error().empty())
        {
            LOG_ERROR("TinyObjReader: {}", reader.Error());
        }
    }
    if (!reader.Warning().empty())
    {
        LOG_ERROR("TinyObjReader: {}", reader.Warning());
    }

    // Get geometry of the mesh
    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    // Allocate memory for mesh data
    auto faceCount = 0;
    for (int s = 0; s < shapes.size(); s ++)
    {
        auto shapeFaceCount = shapes[s].mesh.num_face_vertices.size();
        faceCount += shapeFaceCount;
    }
    vertices.reserve(faceCount * 3);
    indices.resize(faceCount);
    for (int i = 0; i < indices.size(); i++ )
        indices[i] = uint3(i*3, i*3+1, i*3+2);
        

    // Fill mesh data
    for (size_t s = 0; s < shapes.size(); s++)
    {
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            ASSERT(shapes[s].mesh.num_face_vertices[f] == 3);

            for (size_t v = 0; v < 3; v++)
            {
                auto &ver = vertices.emplace_back();
                auto idx = shapes[s].mesh.indices[f*3 + v];

                if (idx.vertex_index != -1)
                {
                    auto vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    auto vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    auto vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    new (&ver.position) float3(vx, vy, vz);
                }

                if (idx.normal_index != -1)
                {
                    auto nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    auto ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    auto nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    new (&ver.normal) float3(nx, ny, nz);
                }

                if (idx.texcoord_index != -1)
                {
                    auto tu = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    auto tv = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    new (&ver.texcoords) float2(tu, tv);
                }
            }
        }
    }

    LOG_INFO("Loaded mesh, face count: {}", FaceCount());
}

Mesh *Mesh::quad()
{
    auto mesh = new Mesh;
    mesh->vertices.assign({
        {{-1.f, -1.f, 0.f}, {0.f,0.f,1.f}, {0.f,0.f}},
        {{ 1.f, -1.f, 0.f}, {0.f,0.f,1.f}, {1.f,0.f}},
        {{ 1.f,  1.f, 0.f}, {0.f,0.f,1.f}, {1.f,1.f}},
        {{-1.f,  1.f, 0.f}, {0.f,0.f,1.f}, {0.f,1.f}} });
    mesh->indices.assign({{0,1,2}, {0,2,3}});
    return mesh;
}

YCU_ASSET_END