#include "camera.h"
#include "../log/log.h"
#include <fstream>

YCU_OPENGL_BEGIN

using namespace ycu::math;

float Camera::aspectRatio() const
{
    return static_cast<float>(screenWidth) / screenHeight;
}

mat4f Camera::viewMatrix() const
{
    return mat4f::left_transform::lookat( pos_, pos_ + front_, up_ );
}

mat4f Camera::projectionMatrix() const
{
    return mat4f::left_transform::perspective(60.f, aspectRatio(), nearPlane, farPlane);
}

void Camera::setDirection(const float2 &dir)
{
    // restrict camera vertical view
    auto sdir = dir;
    sdir.x = clamp_to(sdir.x, minimumTheta_, float(M_PI) - minimumTheta_);
    // update camera coordinates
    front_  = to_cartesian_dir(sdir);
    right_  = cross(front_, idealUp_).normalized();
    up_     = cross(right_, front_);
}

void Camera::update(const UpdateParams &params)
{
    // rotation
    static bool firstUpdate = true;
    if (firstUpdate)
    {
        firstUpdate = false;
    }
    else
    {
        auto dir = to_spherical_dir(front_) + float2(-params.rotateUp, params.rotateLeft);
        setDirection(dir);
    }
    // translation
    if (params.moveFront)   pos_ += front_ * speed;
    if (params.moveBack)    pos_ -= front_ * speed;
    if (params.moveRight)   pos_ += right_ * speed;
    if (params.moveLeft)    pos_ -= right_ * speed;
    if (params.moveUp)      pos_ += up_ * speed;
    if (params.moveDown)    pos_ -= up_ * speed;
}

void Camera::serialize()
{
    std::ofstream file("camera.txt");
    if (file)
    {
        file << pos_.x << ' ' << pos_.y << ' ' << pos_.z << std::endl;
        file << up_.x << ' ' << up_.y << ' ' << up_.z << std::endl;
        file << front_.x << ' ' << front_.y << ' ' << front_.z << std::endl;
        file << right_.x << ' ' << right_.y << ' ' << right_.z << std::endl;
        file.close();
    }
    else
    {
        LOG_ERROR("Failed to serialize camera.");
    }
}

void Camera::deserialize()
{
    std::ifstream file("camera.txt");
    if (file)
    {
        file >> pos_.x >> pos_.y >> pos_.z;
        file >> up_.x >> up_.y >> up_.z;
        file >> front_.x >> front_.y >> front_.z;
        file >> right_.x >> right_.y >> right_.z;
        file.close();
    }
    else
    {
        LOG_ERROR("Failed to deserialize camera.");
    }
}

YCU_OPENGL_END