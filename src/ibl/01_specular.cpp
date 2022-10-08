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
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
vec3 SampleCosineHemisphere( vec2 Xi, vec3 N )
{
    float Phi = 2 * PI * Xi.x;
    float SinTheta = sqrt(Xi.y);
    float CosTheta = sqrt(1 - SinTheta * SinTheta);
    vec3 H;
    H.x = SinTheta * cos( Phi );
    H.y = SinTheta * sin( Phi );
    H.z = CosTheta;
    vec3 UpVector = vec3(0,1,0);
    vec3 TangentX = normalize( cross( UpVector, N ) );
    vec3 TangentY = cross( N, TangentX );
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}
void main()
{
    vec3 N = normalize(vPosition);
    vec3 irr = vec3(0);
    int numSamples = 1024;
    int mapSize = textureSize(uRadiance, 0).x;
    for (int i = 0; i < numSamples; i ++ )
    {
        vec2 Xi = Hammersley(i, numSamples);
        vec3 sam = SampleCosineHemisphere(Xi, N);
        float pdf = dot(N, sam) / PI;
        float saTexel = 4 * PI / (6 * mapSize * mapSize);
        float saSample = 1 / (numSamples * pdf);
        float lod = 0.5 * log2(saSample / saTexel) + 1;
        irr += textureLod(uRadiance, sam, lod).rgb / numSamples;
    }
    fragColor = vec4(irr, 1.0);
})";

const char *precomputeLDVS = R"(# version 430 core
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

const char *precomputeLDFS = R"(# version 430 core
#define PI 3.1415926
layout(binding = 0) uniform samplerCube uEnvironmentMap;
layout(location = 0) out vec4 fragColor;
in vec3 vPosition;
uniform float uRoughness;
mat3 create_tbn(vec3 n)
{
    vec3 a = vec3(0,1,0);
    vec3 t = normalize(cross(a, n));
    vec3 b = normalize(cross(n, t));
    return mat3(t, b, n);
}
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
float DistributionGGX(float NoH, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NoH2 = NoH*NoH;
    float nom   = a2;
    float denom = (NoH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}
vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N )
{
    float a = Roughness * Roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
    float SinTheta = sqrt( 1 - CosTheta * CosTheta );
    vec3 H;
    H.x = SinTheta * cos( Phi );
    H.y = SinTheta * sin( Phi );
    H.z = CosTheta;
    //vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 UpVector = vec3(0,1,0);
    vec3 TangentX = normalize( cross( UpVector, N ) );
    vec3 TangentY = cross( N, TangentX );
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}
vec3 integrate(vec3 R, float roughness)
{
    vec3 N = R, V = R;
    vec3 Lo = vec3(0);
    int numSamples = 1024;
    float totalWeight = 0.0;
    int mapSize = textureSize(uEnvironmentMap, 0).x;
    for (int i = 0; i < numSamples; i ++ )
    {
        vec2 sam = Hammersley(i, numSamples);
        vec3 H = ImportanceSampleGGX(sam, roughness, N);
        vec3 L = 2 * dot(V, H) * H - V;
        float NoV = clamp(dot(N, V), 0.0001, 1.0);
        float NoL = clamp(dot(N, L), 0, 1);
        float NoH = clamp(dot(N, H), 0, 1);
        float VoH = clamp(dot(V, H), 0, 1);

        if (NoL > 0)
        {
            float D = DistributionGGX(NoH, roughness);
            float pdf = D * NoH / (4 * VoH);
            float saTexel = 4 * PI / (6 * mapSize * mapSize);
            float saSample = 1 / (numSamples * pdf);
            float lod = 0.5 * log2(saSample / saTexel) + 1;

            vec3 Li = textureLod(uEnvironmentMap, L, lod).rgb;
            Lo += Li * NoL;
            totalWeight += NoL;
        }
    }
    return Lo / totalWeight;
}
void main()
{
    mat3 tbn = create_tbn(normalize(vPosition));
    vec3 R = normalize(vPosition);
    vec3 ld = integrate(R, uRoughness);
    fragColor = vec4(ld, 1.0);
})";


const char *precomputeDFGVS = R"(# version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
out vec2 vTexCoord;
void main()
{
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
})";

const char *precomputeDFGFS = R"(# version 430 core
#define PI 3.1415926
layout(binding = 0) uniform sampler2D uTexture;
layout(location = 0) out vec4 fragColor;
in vec2 vTexCoord;
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
float DistributionGGX(float NoH, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NoH2 = NoH*NoH;
    float nom   = a2;
    float denom = (NoH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}
vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N )
{
    float a = Roughness * Roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
    float SinTheta = sqrt( 1 - CosTheta * CosTheta );
    vec3 H;
    H.x = SinTheta * cos( Phi );
    H.y = SinTheta * sin( Phi );
    H.z = CosTheta;
    //vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 UpVector = vec3(0,1,0);
    vec3 TangentX = normalize( cross( UpVector, N ) );
    vec3 TangentY = cross( N, TangentX );
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}
float GeometrySchlickGGX(float NoV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;
    float nom   = NoV;
    float denom = NoV * (1.0 - k) + k;
    return nom / denom;
}
float GeometrySmith(float NoV, float NoL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);
    return ggx1 * ggx2;
}
vec2 integrate(float NoV, float roughness)
{
    float A = 0.0, B = 0.0;
    vec3 N = vec3(0,0,1);
    vec3 V = vec3(sqrt(1-NoV*NoV), 0, NoV);
    int numSamples = 1024;
    for (int i = 0; i < numSamples; i ++ )
    {
        vec2 sam = Hammersley(i, numSamples);
        vec3 H = ImportanceSampleGGX(sam, roughness, N);
        vec3 L = 2 * dot(V, H) * H - V;
        float NoL = clamp(dot(N, L), 0, 1);
        float NoH = clamp(dot(N, H), 0, 1);
        float VoH = clamp(dot(V, H), 0, 1);

        if (NoL > 0)
        {
            float G = GeometrySmith(NoV, NoL, roughness);
            float Fc = pow(1 - VoH, 5);
            float Gp = G * VoH / (NoH * NoV);
            A += (1 - Fc) * Gp;
            B += Fc * Gp;
        }
    }
    return vec2(A, B) / numSamples;
}
void main()
{
    vec2 ab = integrate(vTexCoord.x, vTexCoord.y);
    fragColor = vec4(ab, 0, 1.0);
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

const char *meshVS = R"(# version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 uModelMatrix;
uniform mat4 uVPMatrix;
out vec3 vPosition;
out vec3 vNormal;
void main()
{
    vPosition = vec3(uModelMatrix * aPosition);
    gl_Position = uVPMatrix * vec4(vPosition, 1.0);
    vNormal = transpose(inverse(mat3(uModelMatrix))) * aNormal;
})";

const char *meshFS = R"(# version 430 core
#define PI 3.1415926
layout(binding = 0) uniform samplerCube uIrradiance;
layout(binding = 1) uniform samplerCube uEnvironmentMap;
layout(binding = 2) uniform samplerCube uPrefilteredEnvMap;
layout(binding = 3) uniform sampler2D uLUT;
layout(location = 0) out vec4 fragColor;
uniform vec3 uCameraPos;
uniform float uRoughness;
uniform float uMetallic;
in vec3 vPosition;
in vec3 vNormal;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float VoH, vec3 F0)
{
    float fc = pow(clamp(1 - VoH, 0, 1), 5);
    return F0 * (1 - fc) + fc;
}
vec3 fresnelSchlickRoughness(float VoH, vec3 F0, float roughness)
{
    float fc = pow(clamp(1 - VoH, 0, 1), 5);
    vec3 fm = max(vec3(1.0 - roughness), F0);
    return F0 + (fm - F0) * fc;
}
vec3 SpecularIBLSplitSum(vec3 F0, float roughness, vec3 R, float NoV)
{
    float lod = clamp(roughness * 4, 0, 4);
    vec3 ld = textureLod(uPrefilteredEnvMap, R, lod).rgb;
    vec2 ab = texture(uLUT, vec2(NoV, roughness)).xy;
    vec3 col = ld * (F0 * ab.x + ab.y);
    return col;
}
void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vPosition);
    vec3 R = 2 * dot(N, V) * N - V;
    float NoV = clamp(dot(N, V), 0.0001, 1.0);

    vec3 baseColor = vec3(0.95, 0.93, 0.88);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, uMetallic);
    vec3 F = fresnelSchlickRoughness(NoV, F0, uRoughness);
    vec3 kd = (1 - F) * (1 - uMetallic);

    float lod = clamp(uRoughness * 4, 0, 4);
    vec3 ld = textureLod(uPrefilteredEnvMap, R, lod).rgb;
    vec2 dfg = texture(uLUT, vec2(NoV, uRoughness)).xy;
    vec3 spec = ld * (F * dfg.x + dfg.y);
    vec3 diff = baseColor * texture(uIrradiance, N).rgb;

    fragColor = vec4(kd * diff + spec, 1.0);
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


std::shared_ptr<CubeTexture>
    precompute_LD(const std::shared_ptr<CubeTexture> &texture, int size)
{
    auto shader = std::make_shared<Shader>("ld", precomputeLDVS, precomputeLDFS);
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
    cubeTexture->generate_mipmap();
    texture->bind(0);
    shader->bind();
    shader->set_uniform("uProjectionMatrix", captureProjection);
    int numMips = 5;
    for (int level = 0; level < numMips; level ++ )
    {
        if (level)
        {
            size >>= 1;
            fb->resize(size, size);
        }
        for (unsigned int i = 0; i < 6; ++i)
        {
            shader->set_uniform("uViewMatrix", captureViews[i]);
            float roughness = float(level) / (numMips - 1);
            roughness = std::min(std::max(roughness, 0.01f), 0.99f);
            fb->set_render_target(0, cubeTexture->handle, i, level);
            fb->bind();
            shader->set_uniform("uRoughness", roughness);
            GLCALL(glViewport(0, 0, size, size));
            GLCALL(glClearColor(0.2, 0.2, 0.2, 1));
            GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            cube->render(0);
            fb->unbind();
        }
    }
    return cubeTexture;
}

std::shared_ptr<Texture> precompute_DFG(int size)
{
    auto shader = std::make_shared<Shader>("dfg", precomputeDFGVS, precomputeDFGFS);
    auto quad = std::shared_ptr<Mesh>(Mesh::quad());
    auto quadRender = std::make_shared<MeshRender>(quad);
    auto fb = std::make_shared<Framebuffer>(size, size);
    fb->add_render_target(GL_RGBA16F);
    shader->bind();
    fb->bind();
    GLCALL(glViewport(0, 0, size, size));
    GLCALL(glClearColor(0.2, 0.2, 0.2, 1));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    quadRender->render(0);
    fb->unbind();
    return fb->nth_render_target(0);
}

int main()
{
    ycu::log::Log::Init();

    Window window(1200, 400);
    Keyboard keyboard;
    window.attach(static_cast<receiver_t<KeyDownEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyUpEvent>*>(&keyboard));
    window.attach(static_cast<receiver_t<KeyHoldEvent>*>(&keyboard));
    
    auto cube = std::make_shared<MeshRender>("../../../res/cube.obj");
    auto skyboxShader = std::make_shared<Shader>("skybox", skyboxVS, skyboxFS);
    
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<MeshRender>> meshRenders;
    for (int i = 0; i < 10; i ++)
    {
        meshes.push_back(std::make_shared<Mesh>("../../../res/sphere.obj", float3{ i * 2.f, 0.f, 0.f }));
        meshRenders.push_back(std::make_shared<MeshRender>(meshes.back()));
    }
    auto meshShader = std::make_shared<Shader>("mesh", meshVS, meshFS);

    auto quad = std::shared_ptr<Mesh>(Mesh::quad());
    auto quadRender = std::make_shared<MeshRender>(quad);
    auto quadShader = std::make_shared<Shader>("quad", quadVS, quadFS);

    GLCALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

    auto image = std::make_shared<Image>("../../../res/golf_course_sunrise_1k.hdr");
    auto texture = std::make_shared<Texture>(TextureDesc::without_mipmap(image->width, image->height, GL_RGBA16F));
    texture->set_data(image->width, image->height, image->channels, image->data, GL_FLOAT);
    auto cubeTexture = to_cubemap(texture, 512);

    auto irradianceMap = to_irradiance_map(cubeTexture, 128);
    auto prefilteredEnvMap = precompute_LD(cubeTexture, 512);
    auto lut = precompute_DFG(512);

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
        fb.bind();
            // render skybox
            glViewport(0, 0, window.width(), window.height());
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            skyboxShader->bind();
            skyboxShader->set_uniform("uViewMatrix", V);
            skyboxShader->set_uniform("uProjectionMatrix", P);
            //cubeTexture->bind(0);
            irradianceMap->bind(0);
            cube->render(0);
            // render mesh
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            meshShader->bind();
            meshShader->set_uniform("uVPMatrix", P * V);
            meshShader->set_uniform("uCameraPos", camera.pos_);
            irradianceMap->bind(0);
            cubeTexture->bind(1);
            prefilteredEnvMap->bind(2);
            lut->bind(3);
            for (int i = 0; i < meshes.size(); i ++)
            {
                meshShader->set_uniform("uModelMatrix", meshes[i]->modelMatrix());
                meshShader->set_uniform("uRoughness", float(i) / meshes.size());
                meshShader->set_uniform("uMetallic", 0.f);
                meshRenders[i]->render(0);
            }
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