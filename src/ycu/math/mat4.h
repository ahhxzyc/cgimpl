#pragma once

#include "common.h"
#include "vec3.h"
#include "vec4.h"

#include <cmath>

YCU_MATH_BEGIN

template<typename T>
class tmat4
{
public:
    using self_t = tmat4<T>;

    vec4<T> data[4];

public:
    tmat4();
    tmat4(const vec4<T> &c0, const vec4<T> &c1, const vec4<T> &c2, const vec4<T> &c3);
    tmat4(T x00, T x01, T x02, T x03,
        T x10, T x11, T x12, T x13,
        T x20, T x21, T x22, T x23,
        T x30, T x31, T x32, T x33);

    static const auto &identity();
    static auto diag(T val);

    static auto from_columns(const vec4<T> &v0, const vec4<T> &v1, const vec4<T> &v2, const vec4<T> &v3);
    static auto from_rows(const vec4<T> &v0, const vec4<T> &v1, const vec4<T> &v2, const vec4<T> &v3);

    auto &operator [] (size_t idx);
    auto &operator [] (size_t idx) const;

    auto &get_column(size_t idx);
    auto &get_column(size_t idx) const;
    auto get_row(size_t idx) const;

    struct right_transform
    {
        static auto translate(const vec3<T> &v);
        static auto rotate(const vec3<T> &axis, T angle);
        static auto rotate_x(T angle);
        static auto scale(const vec3<T> &ratio);
        static auto lookat(const vec3<T> &eye, const vec3<T> &target, const vec3<T> &up);
        static auto perspective(T fovy, T aspect_ratio, T near_plane, T far_plane);
    };

    
};

template<typename T> auto operator + (const tmat4<T> &lhs, const tmat4<T> &rhs);
template<typename T> auto operator - (const tmat4<T> &lhs, const tmat4<T> &rhs);
template<typename T> auto operator * (const tmat4<T> &lhs, const tmat4<T> &rhs);

template<typename T> auto operator * (const vec4<T> &lhs, const tmat4<T> &rhs);
template<typename T> auto operator * (const tmat4<T> &lhs, const vec4<T> &rhs);

using mat4f = tmat4<float>;
using mat4d = tmat4<double>;

template<typename T>
tmat4<T>::tmat4()
    :tmat4(identity())
{

}
template<typename T>
tmat4<T>::tmat4(const vec4<T> &c0, const vec4<T> &c1, const vec4<T> &c2, const vec4<T> &c3)
    : data{c0, c1, c2, c3}
{

}
template<typename T>
tmat4<T>::tmat4(
    T x00, T x01, T x02, T x03,
    T x10, T x11, T x12, T x13,
    T x20, T x21, T x22, T x23,
    T x30, T x31, T x32, T x33)
    : data{
        {x00, x10, x20, x30},
        {x01, x11, x21, x31},
        {x02, x12, x22, x32},
        {x03, x13, x23, x33}}
{

}

template<typename T> const auto& tmat4<T>::identity()
{
    static const self_t ret = diag(1);
    return ret;
}
template<typename T> auto tmat4<T>::diag(T val)
{
    return tmat4<T>(
        val, 0, 0, 0,
        0, val, 0, 0,
        0, 0, val, 0,
        0, 0, 0, val);
}
template<typename T> auto tmat4<T>::from_columns(
    const vec4<T> &v0, const vec4<T> &v1, const vec4<T> &v2, const vec4<T> &v3)
{
    return tmat4<T>(v0, v1, v2, v3);
}
template<typename T> auto tmat4<T>::from_rows(
    const vec4<T> &v0, const vec4<T> &v1, const vec4<T> &v2, const vec4<T> &v3)
{
    return tmat4<T>(
        v0.x, v0.y, v0.z, v0.w,
        v1.x, v1.y, v1.z, v1.w,
        v2.x, v2.y, v2.z, v2.w,
        v3.x, v3.y, v3.z, v3.w);

}
template<typename T>
auto &tmat4<T>::operator[](size_t idx)
{
    return data[idx];
}
template<typename T>
auto &tmat4<T>::operator[](size_t idx) const
{
    return data[idx];
}

template<typename T>
auto &tmat4<T>::get_column(size_t idx)
{
    return data[idx];
}
template<typename T>
auto &tmat4<T>::get_column(size_t idx) const
{
    return data[idx];
}
template<typename T>
auto tmat4<T>::get_row(size_t idx) const
{
    return vec4(data[0][idx], data[1][idx], data[2][idx], data[3][idx]);
}

template<typename T>
auto tmat4<T>::right_transform::translate(const vec3<T> &v)
{
    return self_t(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        v.x, v.y, v.z, 1);
}
template<typename T>
auto tmat4<T>::right_transform::rotate(const vec3<T> &axis, T angle)
{

}
template<typename T>
auto tmat4<T>::right_transform::rotate_x(T angle)
{
    auto cost = std::cos(angle), sint = std::sin(angle);
    return self_t(
        1, 0, 0, 0,
        0, cost, sint, 0,
        0, -sint, cost, 0,
        0, 0, 0, 1);
}

template<typename T>
auto tmat4<T>::right_transform::scale(const vec3<T> &ratio)
{
    return self_t(
        ratio.x, 0, 0, 0,
        0, ratio.y, 0, 0,
        0, 0, ratio.z, 0,
        0, 0, 0, 1);
}
template<typename T>
auto tmat4<T>::right_transform::lookat(const vec3<T> &eye, const vec3<T> &target, const vec3<T> &up)
{
    auto z = (target - eye).normalized();
    auto x = cross(z, up).normalized();
    auto y = cross(x, z);
    return translate(-eye) * from_columns(
        {x.x, x.y, x.z, 0},
        {y.x, y.y, y.z, 0},
        {z.x, z.y, z.z, 0},
        {0, 0, 0, 1});
}
template<typename T>
auto tmat4<T>::right_transform::perspective(T fovy, T aspect_ratio, T near_plane, T far_plane)
{
    auto C = T(1) / std::tan(fovy * T(0.5));
    auto A = far_plane / (far_plane - near_plane);
    auto B = -near_plane * far_plane / (far_plane - near_plane);
    return self_t(
        C / aspect_ratio, 0, 0, 0,
        0, C, 0, 0,
        0, 0, A, 1,
        0, 0, B, 0);
}

template<typename T> auto operator + (const tmat4<T> &lhs, const tmat4<T> &rhs)
{
    return tmat4<T>::from_columns(
        lhs[0] + rhs[0], 
        lhs[1] + rhs[1], 
        lhs[2] + rhs[2], 
        lhs[3] + rhs[3]);
}
template<typename T> auto operator - (const tmat4<T> &lhs, const tmat4<T> &rhs)
{
    return tmat4<T>::from_columns(
        lhs[0] - rhs[0],
        lhs[1] - rhs[1], 
        lhs[2] - rhs[2], 
        lhs[3] - rhs[3]);
}
template<typename T> auto operator * (const tmat4<T> &lhs, const tmat4<T> &rhs)
{
    tmat4<T> ret;
    for (int col = 0; col < 4; col ++ )
        for (int row = 0; row < 4; row ++ )
            ret[col][row] = dot(lhs.get_row(row), rhs.get_column(col));
    return ret;
}

template<typename T> auto operator * (const vec4<T> &lhs, const tmat4<T> &rhs)
{
    return vec4<T>(
        dot(lhs, rhs.get_column(0)),
        dot(lhs, rhs.get_column(1)),
        dot(lhs, rhs.get_column(2)),
        dot(lhs, rhs.get_column(3)));
}
template<typename T> auto operator * (const tmat4<T> &lhs, const vec4<T> &rhs)
{
    auto p0 = lhs.get_column(0) * rhs[0];
    auto p1 = lhs.get_column(1) * rhs[1];
    auto p2 = lhs.get_column(2) * rhs[2];
    auto p3 = lhs.get_column(3) * rhs[3];
    return p0 + p1 + p2 + p3;
}

YCU_MATH_END