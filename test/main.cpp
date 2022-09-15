#include <ycu/opengl/window.h>
#include <ycu/opengl/meshrender.h>
#include <ycu/opengl/shader.h>
#include <ycu/math.h>
#include <ycu/mesh/mesh.h>
#include <ycu/log/log.h>

using ycu::opengl::Window;
using ycu::opengl::MeshRender;
using ycu::opengl::Shader;
using ycu::mesh::Mesh;

int main()
{
    ycu::log::Log::Init();
    Window window;

    auto mesh = std::make_shared<Mesh>("E:/vscodedev/ptres/cornell-box/cornell-box.obj");
    auto meshRender = std::make_shared<MeshRender>(mesh);

    const char *vertexSource = R"(# version 430 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

void main()
{
    gl_Position = aPosition;
})";

    const char *fragmentSource = R"(# version 430 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(1,0,0,1);
})";
    auto shader = std::make_shared<Shader>("", vertexSource, fragmentSource);

    while (!window.ShouldClose())
    {
        window.DoEvents();

        // IMGUI stuff

        // my rendering
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        shader->Bind();
        meshRender->Render(1);

        // rendering IMGUI

        window.SwapBuffers();
    }

    return 0;
}