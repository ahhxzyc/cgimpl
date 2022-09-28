#include <ycu/opengl.h>
#include <ycu/math.h>
#include <ycu/mesh/mesh.h>
#include <ycu/log/log.h>
#include <ycu/event/keyboard.h>
#include <ycu/random/random.h>

#include <iostream>
#include <stb_image.h>

using ycu::mesh::Mesh;
using namespace ycu::math;
using namespace ycu::event;
using namespace ycu::opengl;
using namespace ycu::random;

const char *skyboxVS = R"(# version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
out vec3 vPosition;
void main()
{
    vPosition = aPosition.xyz;
    mat4 viewRotate = mat4(mat3(uViewMatrix));
    gl_Position = uProjectionMatrix * viewRotate * aPosition;
})";

const char *skyboxFS = R"(# version 430 core
layout(binding = 0) uniform samplerCube uSkybox;
layout(location=0) out vec4 fragColor;
in vec3 vPosition;
void main()
{
    fragColor = texture(uSkybox, vPosition);
})";

const char *quadVS = R"(# version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
out vec2 vTexCoord;
void main()
{
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
})";

const char *quadFS = R"(# version 430 core
layout(binding = 0) uniform sampler2D uTexture;
layout(location = 0) out vec4 fragColor;
in vec2 vTexCoord;
void main()
{
    fragColor = texture(uTexture, vTexCoord);
})";

const char *toCubeVS = R"(#version 430 core
layout (location = 0) in vec4 aPosition;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

out vec3 vPos;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * aPosition;
    vPos = aPosition.xyz;
})";
const char *toCubeFS = R"(#version 430 core

out vec4 FragColor;

in vec3 vPos;
  
layout(binding = 0) uniform sampler2D uEnvMap;

#define PI 3.1415926

vec3 sampleEnvMap(sampler2D map, vec3 dir)
{
    float theta = acos(dir.y);
    float phi = atan(dir.x, dir.z);
    float u = phi / PI * 0.5 + 0.5;
    float v = 1 - theta / PI;
    vec3 col = texture(map, vec2(u,v)).rgb;
    return col;
}

void main()
{
    vec3 N = normalize(vPos);
    vec3 col = sampleEnvMap(uEnvMap, N);
    FragColor = vec4(col, 1.0);
})";

std::shared_ptr<CubeTexture> to_cubemap(const std::shared_ptr<Texture> &texture, int size)
{
    auto shader = std::make_shared<Shader>("", toCubeVS, toCubeFS);

    auto cube = std::make_shared<Mesh>("../../../res/cube.obj");
    auto cubeRender = std::make_shared<MeshRender>(cube);

    auto captureProjection = mat4f::left_transform::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    mat4f captureViews[] =
    {
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}),
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}),
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}),
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {0.0f, -1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}),
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}, {0.0f, -1.0f,  0.0f}),
        mat4f::left_transform::lookat({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}, {0.0f, -1.0f,  0.0f})
    };

    auto fb = std::make_shared<Framebuffer>(size, size);
    auto cubeTexture = std::make_shared<CubeTexture>(size);
    texture->bind(0);
    shader->bind();
    shader->set_uniform("uProjectionMatrix", captureProjection);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shader->set_uniform("uViewMatrix", captureViews[i]);
        fb->set_render_target(cubeTexture->handle, i, 0);
        fb->bind();

        GLCALL(glViewport(0, 0, size, size));
        GLCALL(glClearColor(0.2, 0.2, 0.2, 1));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        cubeRender->render(0);
        fb->unbind();
    }
    cubeTexture->GenerateMipmap();
    return cubeTexture;
}


int main()
{
    ycu::log::Log::Init();

    Window window;
    Keyboard keyboard;
    window.attach(static_cast<receiver_t<KeyDownEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyUpEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyHoldEvent>*>(&keyboard));
    
    auto cube = std::make_shared<Mesh>("../../../res/cube.obj");
    auto cubeRender = std::make_shared<MeshRender>(cube);
    auto skyboxShader = std::make_shared<Shader>("", skyboxVS, skyboxFS);

    auto quad = std::shared_ptr<Mesh>(Mesh::quad());
    auto quadRender = std::make_shared<MeshRender>(quad);
    auto quadShader = std::make_shared<Shader>("", quadVS, quadFS);

    int w, h, c;
    stbi_set_flip_vertically_on_load(true);
    auto data = stbi_loadf("../../../res/solitude_interior_1k.hdr", &w, &h, &c, 0);
    if (!data)
    {
        LOG_ERROR("Failed to load image.");
    }
    else
    {
        // Make sure data falls within the range of half floating point representation
        for (int i = 0; i < w * c * c; i ++)
            data[i] = std::min(data[i], 6e4f);
    }
    auto texture = std::make_shared<Texture>(TextureDesc::without_mipmap(w, h, GL_RGBA16F));
    texture->set_data(w, h, c, data, GL_FLOAT);
    auto cubeTexture = to_cubemap(texture, 512);

    Camera camera;
    Framebuffer framebuffer(window.width(), window.height());
    framebuffer.add_render_target(GL_RGBA16F); // view position gbuffer

    while (!window.ShouldClose())
    {
        window.DoEvents();

        // IMGUI stuff
        // ...

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

        // save / load camera
        if (keyboard.is_down(GLFW_KEY_Y)) camera.serialize();
        if (keyboard.is_down(GLFW_KEY_U)) camera.deserialize();

        // start first pass
        glViewport(0, 0, window.width(), window.height());
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        skyboxShader->bind();
        skyboxShader->set_uniform("uViewMatrix", V);
        skyboxShader->set_uniform("uProjectionMatrix", P);
        cubeTexture->Bind(0);
        cubeRender->render(0);

        // rendering IMGUI
        // ...

        window.SwapBuffers();
    }

    return 0;
}