#pragma once

#include "common.h"
#include "vec2.h"
#include "vec3.h"

YCU_MATH_BEGIN

// assume y axis as world top, and use left hand coordinate system
template<typename T> auto to_spherical_dir(const vec3<T> &dir);
template<typename T> auto to_cartesian_dir(const vec2<T> &dir);

template<typename T>
auto to_spherical_dir(const vec3<T> &dir)
{
    auto u = std::acos(dir.y);
    auto v = std::atan2(dir.x, dir.z);
    return vec2<T>(u, v);
}

template<typename T>
auto to_cartesian_dir(const vec2<T> &dir)
{
    auto C = std::cos(dir.x);
    auto S = std::sin(dir.x);
    return vec3<T>(
        S * std::sin(dir.y), C, S * std::cos(dir.y));
}



YCU_MATH_END