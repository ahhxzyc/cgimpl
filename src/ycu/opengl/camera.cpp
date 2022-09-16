#include "camera.h"


YCU_OPENGL_BEGIN

using ycu::math::float3;
using ycu::math::mat4f;


void Camera::Move(const float3 &disp)
{
    transform.eye += speed * disp;
}

void Camera::RotateYaw(float ang)
{
    //ang *= rotateSpeed;
    //transform.front = glm::rotate(transform.front, ang, transform.up);
    //transform.right = glm::rotate(transform.right, ang, transform.up);
}

void Camera::RotatePitch(float ang)
{

}

float Camera::AspectRatio() const
{
    return static_cast<float>(screenWidth) / screenHeight;
}

mat4f Camera::ViewMatrix() const
{
    return mat4f::left_transform::lookat(
        //transform.eye, transform.eye + transform.front, transform.up);
        transform.eye, float3(0), transform.up);
}

mat4f Camera::ProjectionMatrix() const
{
    return mat4f::left_transform::perspective(60.f, AspectRatio(), nearPlane, farPlane);
}

YCU_OPENGL_END