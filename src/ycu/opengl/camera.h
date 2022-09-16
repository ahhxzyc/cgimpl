#pragma once

#include <ycu/opengl/common.h>
#include <ycu/math.h>
#include <vector>

YCU_OPENGL_BEGIN

struct Camera
{
    struct Transform
    {
        ycu::math::float3 eye   = ycu::math::float3(0, 0, 1);
        ycu::math::float3 up    = ycu::math::float3(0, 1, 0);
        ycu::math::float3 front = ycu::math::float3(0, 0, -1);
        ycu::math::float3 right = ycu::math::float3(1, 0, 0);
    } transform;

    float speed = 0.5f;
    float rotateSpeed = 0.3f;

    int screenWidth;
    int screenHeight;

    float nearPlane = 0.1f;
    float farPlane = 10.f;

    void    Move        (const ycu::math::float3 &disp);
    void    RotateYaw   (float ang);
    void    RotatePitch (float ang);
    float   AspectRatio () const;

    ycu::math::mat4f                ViewMatrix() const;
    ycu::math::mat4f                ProjectionMatrix() const;
    std::vector<ycu::math::float3>  FrustumCorners() const;
};

YCU_OPENGL_END