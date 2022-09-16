#include "camera.h"


YCU_OPENGL_BEGIN

using namespace ycu::math;


void Camera::Move(const float3 &disp)
{
    pos_ += speed * disp;
}

void Camera::RotateYaw(float ang)
{
    //ang *= rotateSpeed;
    //front_ = glm::rotate(front_, ang, up_);
    //right_ = glm::rotate(right_, ang, up_);
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
    return mat4f::left_transform::lookat( pos_, pos_ + front_, up_ );
}

mat4f Camera::ProjectionMatrix() const
{
    return mat4f::left_transform::perspective(60.f, AspectRatio(), nearPlane, farPlane);
}

void Camera::setDirection(const float2 &dir)
{
    // restrict camera vertical view
    auto sdir = dir;
    sdir.x = clamp_to(sdir.x, minimumTheta_, float(M_PI) - minimumTheta_);
    auto cdir = to_cartesian_dir(sdir);
    // update camera coordinates
    front_  = cdir;
    right_  = cross(front_, idealUp_).normalized();
    up_     = cross(right_, front_);
}

YCU_OPENGL_END