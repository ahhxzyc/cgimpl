#include "meshrender.h"

#include "buffer.h"

YCU_OPENGL_BEGIN

using ycu::mesh::Vertex;
using ycu::mesh::Mesh;
using ycu::math::uint3;

MeshRender::MeshRender(const std::shared_ptr<Mesh> &mesh) : m_Mesh(mesh)
{
    Buffer vb(BufferType::VERTEX);
    Buffer ib(BufferType::INDEX);
    vb.Upload(mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
    ib.Upload(mesh->indices.data(), mesh->indices.size() * sizeof(uint3));

    GLCALL(glCreateVertexArrays(1, &m_VertexArray));
    GLCALL(glBindVertexArray(m_VertexArray));
    vb.Bind();
    ib.Bind();
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
        GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, offset));
        GLCALL(glEnableVertexAttribArray(2));
    }
    GLCALL(glBindVertexArray(0));
}

void MeshRender::Render(int mode)
{
    GLCALL(glBindVertexArray(m_VertexArray));

    if (mode == 1)
    {
        GLCALL(glPointSize(2.f));
        GLCALL(glDrawElements(GL_POINTS, m_Mesh->vertices.size(), GL_UNSIGNED_INT, 0));
    }
    else
    {
        GLCALL(glDrawElements(GL_TRIANGLES, m_Mesh->vertices.size(), GL_UNSIGNED_INT, 0));
    }

    GLCALL(glBindVertexArray(0));
}


YCU_OPENGL_END