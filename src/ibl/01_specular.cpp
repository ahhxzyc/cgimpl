#include <ycu/opengl.h>
#include <ycu/math.h>
#include <ycu/asset/mesh.h>
#include <ycu/asset/image.h>
#include <ycu/log/log.h>
#include <ycu/event/keyboard.h>
#include <ycu/random/random.h>

#include <iostream>
#include <stb_image.h>

using namespace ycu::math;
using namespace ycu::asset;
using namespace ycu::event;
using namespace ycu::opengl;
using namespace ycu::random;

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
layout(binding = 0) uniform sampler2D uEnvMap;
layout(location = 0) out vec4 FragColor;
#define PI 3.1415926
in vec3 vPos;
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
    //FragColor = vec4(ACES(col), 1.0);
    FragColor = vec4(col, 1.0);
})";

const char *irrVS = R"(# version 430 core
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

const char *irrFS = R"(# version 430 core
#define PI 3.1415926
layout(binding = 0) uniform samplerCube uRadiance;
layout(location = 0) out vec4 fragColor;
in vec3 vPosition;
mat3 create_tbn(vec3 n)
{
    vec3 a = vec3(0,1,0);
    vec3 t = normalize(cross(a, n));
    vec3 b = normalize(cross(n, t));
    return mat3(t, b, n);
}
void main()
{
    mat3 tbn = create_tbn(normalize(vPosition));
    float sampleDelta = radians(1.0);
    vec3 irr = vec3(0);
    int numSamples = 0;
    for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    {
        float cosTheta = cos(theta), sinTheta = sin(theta);
        for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
        {
            vec3 samp = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
            samp = tbn * samp;
            irr += texture(uRadiance, samp).rgb * cosTheta * sinTheta;
            numSamples ++ ;
        }
    }
    irr /= float(numSamples);
    fragColor = vec4(irr, 1.0);
})";

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

const char *diffuseVS = R"(# version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 uMVP;
out vec3 vNormal;
void main()
{
    gl_Position = uMVP * aPosition;
    vNormal = aNormal;
})";

const char *diffuseFS = R"(# version 430 core
layout(binding = 0) uniform samplerCube uIrradiance;
layout(location=0) out vec4 fragColor;
in vec3 vNormal;
void main()
{
    vec3 albedo = vec3(1);
    vec3 N = normalize(vNormal);
    vec3 L = texture(uIrradiance, N).rgb * albedo;
    fragColor = vec4(L, 1.0);
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
vec3 ACES(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), vec3(0), vec3(1));
}
void main()
{
    vec3 col = ACES(texture(uTexture, vTexCoord).rgb);
    fragColor = vec4(col, 1.0);
})";

std::shared_ptr<CubeTexture> to_cubemap(const std::shared_ptr<Texture> &texture, int size)
{
    auto shader = std::make_shared<Shader>("tocube", toCubeVS, toCubeFS);

    auto cube = std::make_shared<MeshRender>("../../../res/cube.obj");

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
        fb->set_render_target(0, cubeTexture->handle, i);
        fb->bind();

        GLCALL(glViewport(0, 0, size, size));
        GLCALL(glClearColor(0.2, 0.2, 0.2, 1));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        cube->render(0);
        fb->unbind();
    }
    cubeTexture->generate_mipmap();
    return cubeTexture;
}

std::shared_ptr<CubeTexture> 
    to_irradiance_map(const std::shared_ptr<CubeTexture> &texture, int size)
{
    auto shader = std::make_shared<Shader>("irr", irrVS, irrFS);

    auto cube = std::make_shared<MeshRender>("../../../res/cube.obj");

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
        fb->set_render_target(0, cubeTexture->handle, i);
        fb->bind();

        GLCALL(glViewport(0, 0, size, size));
        GLCALL(glClearColor(0.2, 0.2, 0.2, 1));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        cube->render(0);
        fb->unbind();
    }
    cubeTexture->generate_mipmap();
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
    
    auto cube = std::make_shared<MeshRender>("../../../res/cube.obj");
    auto skyboxShader = std::make_shared<Shader>("skybox", skyboxVS, skyboxFS);
    
    auto sphere = std::make_shared<Mesh>("../../../res/sphere.obj");
    auto sphereRender = std::make_shared<MeshRender>(sphere);
    auto diffuseShader = std::make_shared<Shader>("diffuse", diffuseVS, diffuseFS);

    auto quad = std::shared_ptr<Mesh>(Mesh::quad());
    auto quadRender = std::make_shared<MeshRender>(quad);
    auto quadShader = std::make_shared<Shader>("quad", quadVS, quadFS);

    GLCALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

    auto image = std::make_shared<Image>("../../../res/solitude_interior_1k.hdr");
    auto texture = std::make_shared<Texture>(TextureDesc::without_mipmap(image->width, image->height, GL_RGBA16F));
    texture->set_data(image->width, image->height, image->channels, image->data, GL_FLOAT);
    auto cubeTexture = to_cubemap(texture, 512);
    auto irradianceMap = to_irradiance_map(cubeTexture, 32);

    Camera camera;
    Framebuffer fb(window.width(), window.height());
    fb.add_render_target(GL_RGBA16F);

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
        camera.screenWidth = window.width();
        camera.screenHeight = window.height();

        // save / load camera
        if (keyboard.is_down(GLFW_KEY_Y)) camera.serialize();
        if (keyboard.is_down(GLFW_KEY_U)) camera.deserialize();

        // render to a quad
        //GLCALL(glDisable(GL_FRAMEBUFFER_SRGB));
        fb.bind();
            // render skybox
            glViewport(0, 0, window.width(), window.height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            skyboxShader->bind();
            skyboxShader->set_uniform("uViewMatrix", V);
            skyboxShader->set_uniform("uProjectionMatrix", P);
            cubeTexture->bind(0);
            cube->render(0);
            // render mesh
            auto M = sphere->modelMatrix();
            glEnable(GL_DEPTH_TEST);
            diffuseShader->bind();
            diffuseShader->set_uniform("uMVP", P*V*M);
            irradianceMap->bind(0);
            sphereRender->render(0);
        fb.unbind();

        // post process
        GLCALL(glEnable(GL_FRAMEBUFFER_SRGB));
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        fb.nth_render_target(0)->bind(0);
        quadShader->bind();
        quadRender->render(0);

        // rendering IMGUI
        // ...

        window.SwapBuffers();
    }

    return 0;
}