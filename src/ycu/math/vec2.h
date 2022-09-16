#pragma once

#include "common.h"

YCU_MATH_BEGIN

template<typename T>
class vec2
{
public:
    using SelfType = vec2<T>;
public:
    union
    {
        T data[2];
        struct { T x, y; };
        struct { T u, v; };
    };

    constexpr vec2();
    explicit constexpr vec2(T val);
    constexpr vec2(T x, T y);

    auto normalized() const;
    auto normalize();

    auto length() const;
    auto length2() const;

    auto operator / (T val) const;
    auto operator * (T val) const;

    auto &operator *= (T val);
    auto &operator /= (T val);
    auto &operator += (const SelfType &v);
    auto &operator -= (const SelfType &v);
    auto &operator *= (const SelfType &v);
    auto &operator /= (const SelfType &v);

    auto &operator[] (size_t idx);
    auto &operator[] (size_t idx) const;
};

template<typename T> auto operator - (const vec2<T> &v);
template<typename T> auto operator - (const vec2<T> &lhs, const vec2<T> &rhs);
template<typename T> auto operator + (const vec2<T> &lhs, const vec2<T> &rhs);
template<typename T> auto operator * (const vec2<T> &lhs, const vec2<T> &rhs);
template<typename T> auto operator / (const vec2<T> &lhs, const vec2<T> &rhs);

template<typename T> auto operator * (T val, const vec2<T> &v);

template<typename T> auto dot(const vec2<T> &lhs, const vec2<T> &rhs);

using float2 = vec2<float>;
using int2 = vec2<int>;
using uint2 = vec2<uint32_t>;


template<typename T>
constexpr vec2<T>::vec2() : vec2(0)
{

}
template<typename T>
constexpr vec2<T>::vec2(T val) : vec2(val, val)
{

}
template<typename T>
constexpr vec2<T>::vec2(T x, T y) : x(x), y(y)
{

}

template<typename T>
auto vec2<T>::normalized() const
{
    return *this / length();
}

template<typename T>
auto vec2<T>::normalize()
{
    *this /= length();
}

template<typename T>
auto vec2<T>::length() const
{
    return std::sqrt(length2());
}

template<typename T>
auto vec2<T>::length2() const
{
    return dot(*this, *this);
}

template<typename T>
auto vec2<T>::operator / (T val) const
{
    return SelfType(x / val, y / val);
}

template<typename T>
auto vec2<T>::operator * (T val) const
{
    return SelfType(x * val, y * val);
}

template<typename T>
auto &vec2<T>::operator *= (T val)
{
    x *= val; y *= val;
    return *this;
}

template<typename T>
auto &vec2<T>::operator /= (T val)
{
    x /= val; y /= val;
    return *this;
}

template<typename T>
auto &vec2<T>::operator += (const SelfType &v)
{
    x += v.x; y += v.y;
    return *this;
}

template<typename T>
auto &vec2<T>::operator -= (const SelfType &v)
{
    x -= v.x; y -= v.y;
    return *this;
}

template<typename T>
auto &vec2<T>::operator *= (const SelfType &v)
{
    x *= v.x; y *= v.y;
    return *this;
}

template<typename T>
auto &vec2<T>::operator /= (const SelfType &v)
{
    x /= v.x; y /= v.y;
    return *this;
}

template<typename T>
auto &vec2<T>::operator[] (size_t idx)
{
    return data[idx];
}

template<typename T>
auto &vec2<T>::operator[] (size_t idx) const
{
    return data[idx];
}

template<typename T>
auto operator - (const vec2<T> &v)
{
    return vec2<T>(-v.x, -v.y);
}

template<typename T>
auto operator - (const vec2<T> &lhs, const vec2<T> &rhs)
{
    return vec2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}
template<typename T>
auto operator + (const vec2<T> &lhs, const vec2<T> &rhs)
{
    return vec2<T>(rhs.x + lhs.x, rhs.y + lhs.y);
}
template<typename T>
auto operator * (const vec2<T> &lhs, const vec2<T> &rhs)
{
    return vec2<T>(rhs.x * lhs.x, rhs.y * lhs.y);
}
template<typename T>
auto operator / (const vec2<T> &lhs, const vec2<T> &rhs)
{
    return vec2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template<typename T>
auto operator * (T val, const vec2<T> &v)
{
    return v * val;
}

template<typename T>
auto dot(const vec2<T> &lhs, const vec2<T> &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

YCU_MATH_END