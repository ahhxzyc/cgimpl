#include <ycu/opengl.h>
#include <ycu/math.h>
#include <ycu/mesh/mesh.h>
#include <ycu/log/log.h>
#include <ycu/event/keyboard.h>
#include <ycu/random/random.h>

#include <iostream>
#include <stb_image.h>

using ycu::mesh::Mesh;
using ycu::math::float3;
using ycu::math::float2;
using namespace ycu::event;
using namespace ycu::opengl;
using namespace ycu::random;

const char *vertexSource = R"(# version 430 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vPosition;
out vec3 vNormal;

void main()
{
    vPosition = (uViewMatrix * aPosition).xyz;
    mat3 normalTransform = transpose(inverse(mat3(uViewMatrix)));
    vNormal = normalTransform * aNormal;
    gl_Position = uProjectionMatrix * vec4(vPosition, 1.0);
})";

const char *fragmentSource = R"(# version 430 core

layout(location=1) out vec3 fragPosition;
layout(location=2) out vec3 fragNormal;

in vec3 vPosition;
in vec3 vNormal;

void main()
{
    fragPosition = vPosition;
    fragNormal = normalize(vNormal);
})";

int main()
{
    ycu::log::Log::Init();

    Window window;
    Keyboard keyboard;
    window.attach(static_cast<receiver_t<KeyDownEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyUpEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyHoldEvent>*>(&keyboard));

    auto mesh = std::make_shared<Mesh>("E:/vscodedev/ycutils/res/cornell-box.obj");
    auto meshRender = std::make_shared<MeshRender>(mesh);
    auto meshShader = std::make_shared<Shader>("", vertexSource, fragmentSource);

    auto quad = std::shared_ptr<Mesh>(Mesh::quad());
    auto quadRender = std::make_shared<MeshRender>(quad);
    auto quadShader = std::make_shared<Shader>("", quadVS, quadFS);

    Camera camera;

    Framebuffer framebuffer(window.width(), window.height());
    framebuffer.add_render_target(GL_RGBA16F); // view position gbuffer
    framebuffer.add_render_target(GL_RGBA16F); // normal gbuffer

    // random numbers
    int numSamples = 64;
    std::vector<float3> samples;
    for (int i = 0; i < numSamples; i ++)
    {
        float3 sample(
            random_float() * 2 - 1,
            random_float() * 2 - 1,
            random_float());
        sample = sample.normalized();

        while (dot(sample, float3(0, 0, 1)) < 0.5f)
        {
            sample = float3(random_float() * 2 - 1, random_float() * 2 - 1, random_float());
            sample = sample.normalized();
        }

        sample *= random_float();
        samples.emplace_back(sample);
    }

    // noise texture
    int noiseTextureSize = 4;
    std::vector<float3> noise;
    for (int i = 0; i < noiseTextureSize * noiseTextureSize; i ++)
    {
        noise.emplace_back(random_float() * 2 - 1, random_float() * 2 - 1, 0);
    }
    auto noiseTextureDesc = TextureDesc::without_mipmap(noiseTextureSize, noiseTextureSize, GL_RGB16F);
    noiseTextureDesc.wrapS = GL_REPEAT;
    noiseTextureDesc.wrapT = GL_REPEAT;
    auto noiseTexture = std::make_shared<Texture>(noiseTextureDesc);
    noiseTexture->set_data(noiseTextureSize, noiseTextureSize, noise.data(), GL_FLOAT);

    while (!window.ShouldClose())
    {
        window.DoEvents();

        // IMGUI stuff
        // ...

        // update camera transform
        Camera::UpdateParams params;
        params.rotateLeft = -0.003f * window.relativeCursorX_;
        params.rotateUp = -0.003f * window.relativeCursorY_;
        params.moveFront = keyboard.is_down(GLFW_KEY_W);
        params.moveLeft = keyboard.is_down(GLFW_KEY_A);
        params.moveBack = keyboard.is_down(GLFW_KEY_S);
        params.moveRight = keyboard.is_down(GLFW_KEY_D);
        camera.update(params);
        auto V = camera.viewMatrix();
        auto P = camera.projectionMatrix();

        // save / load camera
        if (keyboard.is_down(GLFW_KEY_Y)) camera.serialize();
        if (keyboard.is_down(GLFW_KEY_U)) camera.deserialize();

        // start first pass
        framebuffer.bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // render scene
        meshShader->bind();
        meshShader->set_uniform("uViewMatrix", V);
        meshShader->set_uniform("uProjectionMatrix", P);
        meshRender->render(0);

        // second pass : quad rendering
        framebuffer.unbind();
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        framebuffer.nth_render_target(1)->bind(0);
        framebuffer.nth_render_target(2)->bind(1);
        noiseTexture->bind(2);
        quadShader->bind();
        quadShader->set_uniform("uViewMatrix", V);
        quadShader->set_uniform("uProjectionMatrix", P);
        quadShader->set_uniform("uNumSamples", numSamples);
        for (int i = 0; i < numSamples; i ++)
        {
            std::string name = "uSamples[" + std::to_string(i) + "]";
            quadShader->set_uniform(name, samples[i]);
        }
        quadRender->render(0);

        // rendering IMGUI
        // ...

        window.SwapBuffers();
    }

    return 0;
}