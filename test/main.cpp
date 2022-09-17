#include <ycu/opengl/window.h>
#include <ycu/opengl/meshrender.h>
#include <ycu/opengl/shader.h>
#include <ycu/opengl/camera.h>
#include <ycu/math.h>
#include <ycu/mesh/mesh.h>
#include <ycu/log/log.h>
#include <ycu/event/keyboard.h>

using ycu::opengl::Window;
using ycu::opengl::MeshRender;
using ycu::opengl::Shader;
using ycu::opengl::Camera;
using ycu::mesh::Mesh;
using ycu::math::float3;
using ycu::math::float2;
using namespace ycu::event;

const char *vertexSource = R"(# version 430 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * aPosition;
})";

const char *fragmentSource = R"(# version 430 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(1,0,0,1);
})";

int main()
{
    ycu::log::Log::Init();

    Window window;
    Keyboard keyboard;
    window.attach(static_cast<receiver_t<KeyDownEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyUpEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyHoldEvent>*>(&keyboard));
    
    auto mesh       = std::make_shared<Mesh>("E:/vscodedev/ptres/cornell-box/cornell-box.obj");
    auto meshRender = std::make_shared<MeshRender>(mesh);
    auto shader     = std::make_shared<Shader>("", vertexSource, fragmentSource);

    Camera camera;

    while (!window.ShouldClose())
    {
        window.DoEvents();

        // IMGUI stuff
        // ...

        // clear canvas
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // update camera transform
        Camera::UpdateParams params;
        params.rotateLeft = -0.003f * window.relativeCursorX_;
        params.rotateUp   = -0.003f * window.relativeCursorY_;
        params.moveFront = keyboard.is_down(GLFW_KEY_W);
        params.moveLeft  = keyboard.is_down(GLFW_KEY_A);
        params.moveBack  = keyboard.is_down(GLFW_KEY_S);
        params.moveRight = keyboard.is_down(GLFW_KEY_D);
        camera.update(params);

        auto V = camera.viewMatrix();
        auto P = camera.projectionMatrix();

        // render
        shader->Bind();
        shader->SetUniform("uMVP", P*V);
        meshRender->Render(1);

        // rendering IMGUI
        // ...

        window.SwapBuffers();
    }

    return 0;
}