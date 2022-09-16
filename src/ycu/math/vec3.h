#pragma once

#include "common.h"

YCU_MATH_BEGIN

template<typename T>
class vec3
{
public:
	using SelfType = vec3<T>;

public:
	union 
	{
		T data[3];
		struct {T x, y, z;};
		struct {T r, g, b;};
	};

	constexpr vec3();
	explicit constexpr vec3(T val);
	constexpr vec3(T x, T y, T z);

	auto normalized() const;
	auto normalize();
	auto length() const;
	auto length2() const;

	auto operator / (T val) const;
	auto operator * (T val) const;

	auto& operator *= (T val);
	auto& operator /= (T val);
	auto& operator += (const SelfType& v);
	auto& operator -= (const SelfType& v);
	auto& operator *= (const SelfType& v);
	auto& operator /= (const SelfType& v);

    auto &operator[] (size_t idx);
    auto &operator[] (size_t idx) const;
};

template<typename T> auto operator - (const vec3<T> &v);
template<typename T> auto operator - (const vec3<T> &lhs, const vec3<T> &rhs);
template<typename T> auto operator + (const vec3<T> &lhs, const vec3<T> &rhs);
template<typename T> auto operator * (const vec3<T> &lhs, const vec3<T> &rhs);
template<typename T> auto operator / (const vec3<T> &lhs, const vec3<T> &rhs);

template<typename T> auto operator * (T val, const vec3<T> &v);

template<typename T> auto dot(const vec3<T> &lhs, const vec3<T> &rhs);
template<typename T> auto cross(const vec3<T> &lhs, const vec3<T> &rhs);

using float3 = vec3<float>;
using int3 = vec3<int>;
using uint3 = vec3<uint32_t>;


template<typename T>
constexpr vec3<T>::vec3() : vec3(0)
{

}
template<typename T>
constexpr vec3<T>::vec3(T val) : vec3(val, val, val)
{

}
template<typename T>
constexpr vec3<T>::vec3(T x, T y, T z) : x(x), y(y), z(z)
{

}

template<typename T>
auto vec3<T>::normalized() const
{
    return *this / length();
}

template<typename T>
auto vec3<T>::normalize()
{
    *this /= length();
}

template<typename T>
auto vec3<T>::length() const
{
    return std::sqrt(length2());
}

template<typename T>
auto vec3<T>::length2() const
{
    return dot(*this, *this);
}

template<typename T>
auto vec3<T>::operator / (T val) const
{
    return SelfType(x / val, y / val, z / val);
}

template<typename T>
auto vec3<T>::operator * (T val) const
{
    return SelfType(x * val, y * val, z * val);
}

template<typename T>
auto &vec3<T>::operator *= (T val)
{
    x *= val; y *= val; z *= val;
    return *this;
}

template<typename T>
auto &vec3<T>::operator /= (T val)
{
    x /= val; y /= val; z /= val;
    return *this;
}

template<typename T>
auto& vec3<T>::operator += (const SelfType &v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}

template<typename T>
auto& vec3<T>::operator -= (const SelfType &v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

template<typename T>
auto &vec3<T>::operator *= (const SelfType &v)
{
    x *= v.x; y *= v.y; z *= v.z;
    return *this;
}

template<typename T>
auto &vec3<T>::operator /= (const SelfType &v)
{
    x /= v.x; y /= v.y; z /= v.z;
    return *this;
}

template<typename T>
auto &vec3<T>::operator[] (size_t idx)
{
    return data[idx];
}

template<typename T>
auto &vec3<T>::operator[] (size_t idx) const
{
    return data[idx];
}

template<typename T>
auto operator - (const vec3<T> &v)
{
    return vec3<T>(-v.x, -v.y, -v.z);
}

template<typename T>
auto operator - (const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}
template<typename T>
auto operator + (const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z);
}
template<typename T>
auto operator * (const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(rhs.x * lhs.x, rhs.y * lhs.y, rhs.z * lhs.z);
}
template<typename T>
auto operator / (const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

template<typename T>
auto operator * (T val, const vec3<T> &v)
{
    return v * val;
}

template<typename T>
auto dot(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
template<typename T>
auto cross(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.y * rhs.z - lhs.z * rhs.y, 
        lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}


YCU_MATH_END