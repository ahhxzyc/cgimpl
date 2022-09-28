#pragma once

#include <ycu/opengl/common.h>
#include <ycu/opengl/buffer.h>
#include <ycu/mesh/mesh.h>
#include <memory>

YCU_OPENGL_BEGIN

class MeshRender
{
public:
    MeshRender(const std::shared_ptr<ycu::mesh::Mesh> &mesh);
    ~MeshRender();

    void render(int mode = 0);

private:
    std::shared_ptr<ycu::mesh::Mesh> mesh_;
    GLuint vertexArray_;
    std::shared_ptr<Buffer> vertexBuffer_;
    std::shared_ptr<Buffer> indexBuffer_;
};

YCU_OPENGL_END