#include "meshrender.h"

#include "buffer.h"

YCU_OPENGL_BEGIN

using ycu::mesh::Vertex;
using ycu::mesh::Mesh;
using ycu::math::uint3;

MeshRender::MeshRender(const std::shared_ptr<Mesh> &mesh) : mesh_(mesh)
{
    vertexBuffer_ = std::make_shared<Buffer>(BufferType::VERTEX);
    indexBuffer_ = std::make_shared<Buffer>(BufferType::INDEX);
    vertexBuffer_->Upload(mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
    indexBuffer_->Upload(mesh->indices.data(), mesh->indices.size() * sizeof(uint3));

    GLCALL(glCreateVertexArrays(1, &vertexArray_));
    GLCALL(glBindVertexArray(vertexArray_));
    vertexBuffer_->Bind();
    indexBuffer_->Bind();
    {
        auto offset = reinterpret_cast<const void *>(offsetof(Vertex, position));
        auto stride = sizeof(Vertex);
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset));
        GLCALL(glEnableVertexAttribArray(0));
    }
    {
        auto offset = reinterpret_cast<const void *>(offsetof(Vertex, normal));
        auto stride = sizeof(Vertex);
        GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset));
        GLCALL(glEnableVertexAttribArray(1));
    }
    {
        auto offset = reinterpret_cast<const void *>(offsetof(Vertex, texcoords));
        auto stride = sizeof(Vertex);
        GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, offset));
        GLCALL(glEnableVertexAttribArray(2));
    }
    GLCALL(glBindVertexArray(0));
}

void MeshRender::render(int mode)
{
    GLCALL(glBindVertexArray(vertexArray_));

    if (mode == 1)
    {
        GLCALL(glPointSize(2.f));
        GLCALL(glDrawElements(GL_POINTS, mesh_->indices.size() * 3, GL_UNSIGNED_INT, 0));
    }
    else
    {
        GLCALL(glDrawElements(GL_TRIANGLES, mesh_->indices.size() * 3, GL_UNSIGNED_INT, 0));
    }

    GLCALL(glBindVertexArray(0));
}

MeshRender::~MeshRender()
{
    GLCALL(glDeleteVertexArrays(1, &vertexArray_));
}

YCU_OPENGL_END