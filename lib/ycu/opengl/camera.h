#pragma once

#include <ycu/opengl/common.h>
#include <ycu/math.h>
#include <vector>

YCU_OPENGL_BEGIN



struct Camera
{
    struct UpdateParams
    {
        float rotateLeft = 0.f;
        float rotateUp   = 0.f;
        bool moveFront = false;
        bool moveBack  = false;
        bool moveRight = false;
        bool moveLeft  = false;
        bool moveUp    = false;
        bool moveDown  = false;
    };

    inline static constexpr auto idealUp_       = ycu::math::float3(0, 1, 0);
    inline static constexpr auto minimumTheta_  = ycu::math::radians(15.f);

    ycu::math::float3 pos_      = ycu::math::float3(0, 0, 1);
    ycu::math::float3 up_       = ycu::math::float3(0, 1, 0);
    ycu::math::float3 front_    = ycu::math::float3(0, 0, -1);
    ycu::math::float3 right_    = ycu::math::float3(1, 0, 0);

    float speed = 0.01f;
    float rotateSpeed = 0.3f;

    int screenWidth;
    int screenHeight;

    float nearPlane = 0.1f;
    float farPlane = 100.f;

    float aspectRatio() const;

    void setDirection(const ycu::math::float2 &dir);
    void update(const UpdateParams &params);

    void serialize();
    void deserialize();

    ycu::math::mat4f viewMatrix() const;
    ycu::math::mat4f projectionMatrix() const;
    ycu::math::mat4f ortho() const;
};

YCU_OPENGL_END