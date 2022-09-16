#pragma once

#include <ycu/opengl/common.h>
#include <ycu/math.h>
#include <vector>

YCU_OPENGL_BEGIN

struct Camera
{
    inline static constexpr auto idealUp_       = ycu::math::float3(0, 1, 0);
    inline static constexpr auto minimumTheta_  = ycu::math::radians(15.f);

    ycu::math::float3 pos_      = ycu::math::float3(0, 0, 1);
    ycu::math::float3 up_       = ycu::math::float3(0, 1, 0);
    ycu::math::float3 front_    = ycu::math::float3(0, 0, -1);
    ycu::math::float3 right_    = ycu::math::float3(1, 0, 0);

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

    void setDirection(const ycu::math::float2 &dir);

    ycu::math::mat4f ViewMatrix() const;
    ycu::math::mat4f ProjectionMatrix() const;
};

YCU_OPENGL_END