#pragma once

#include <ycu/opengl/common.h>
#include <ycu/opengl/buffer.h>
#include <ycu/mesh/mesh.h>

YCU_OPENGL_BEGIN

class MeshRender
{
public:
    MeshRender(const std::shared_ptr<ycu::mesh::Mesh> &mesh);

    void Render(int mode = 0);

private:
    std::shared_ptr<ycu::mesh::Mesh> m_Mesh;
    GLuint m_VertexArray;
};

YCU_OPENGL_END