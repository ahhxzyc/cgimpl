#pragma once

#include <ycu/math/common.h>

YCU_MATH_BEGIN

template<typename T>
class vec4
{
public:
	using self_t = vec4<T>;

public:
	union 
	{
		T data[4];
		struct {T x, y, z, w;};
		struct {T r, g, b, a;};
	};

	vec4();
	explicit vec4(T val);
	vec4(T x, T y, T z, T w);

	auto normalized() const;
	auto normalize();
	auto length() const;
	auto length2() const;

	auto operator / (T val) const;
	auto operator * (T val) const;

	auto& operator *= (T val);
	auto& operator /= (T val);
	auto& operator += (const self_t& v);
	auto& operator -= (const self_t& v);
	auto& operator *= (const self_t& v);
	auto& operator /= (const self_t& v);

    auto& operator[] (size_t idx);
    auto& operator[] (size_t idx) const;
};



template<typename T> auto operator - (const vec4<T> &v);
template<typename T> auto operator - (const vec4<T> &lhs, const vec4<T> &rhs);
template<typename T> auto operator + (const vec4<T> &lhs, const vec4<T> &rhs);
template<typename T> auto operator * (const vec4<T> &lhs, const vec4<T> &rhs);
template<typename T> auto operator / (const vec4<T> &lhs, const vec4<T> &rhs);

template<typename T> auto operator * (T val, const vec4<T> &v);

template<typename T> auto dot(const vec4<T> &lhs, const vec4<T> &rhs);

using float4 = vec4<float>;
using int4 = vec4<int>;

template<typename T>
vec4<T>::vec4() : vec4(0)
{

}
template<typename T>
vec4<T>::vec4(T val) : vec4(val, val, val)
{

}
template<typename T>
vec4<T>::vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w)
{

}

template<typename T>
auto vec4<T>::normalized() const
{
    return *this / length();
}

template<typename T>
auto vec4<T>::normalize()
{
    *this /= length();
}

template<typename T>
auto vec4<T>::length() const
{
    return std::sqrt(length2());
}

template<typename T>
auto vec4<T>::length2() const
{
    return dot(*this, *this);
}

template<typename T>
auto vec4<T>::operator / (T val) const
{
    return self_t(x / val, y / val, z / val, w / val);
}

template<typename T>
auto vec4<T>::operator * (T val) const
{
    return self_t(x * val, y * val, z * val, w * val);
}

template<typename T>
auto &vec4<T>::operator *= (T val)
{
    x *= val; y *= val; z *= val; w *= val;
    return *this;
}

template<typename T>
auto &vec4<T>::operator /= (T val)
{
    x /= val; y /= val; z /= val; w /= val;
    return *this;
}

template<typename T>
auto& vec4<T>::operator += (const self_t &v)
{
    x += v.x; y += v.y; z += v.z; w += v.w;
    return *this;
}

template<typename T>
auto& vec4<T>::operator -= (const self_t &v)
{
    x -= v.x; y -= v.y; z -= v.z; w -= v.w;
    return *this;
}

template<typename T>
auto &vec4<T>::operator *= (const self_t &v)
{
    x *= v.x; y *= v.y; z *= v.z; w *= v.w;
    return *this;
}

template<typename T>
auto &vec4<T>::operator[] (size_t idx)
{
    return data[idx];
}
template<typename T>
auto &vec4<T>::operator[] (size_t idx) const
{
    return data[idx];
}

template<typename T>
auto &vec4<T>::operator /= (const self_t &v)
{
    x /= v.x; y /= v.y; z /= v.z; w /= v.w;
    return *this;
}

template<typename T>
auto operator - (const vec4<T> &v)
{
    return vec4<T>(-v.x, -v.y, -v.z, -v.w);
}

template<typename T>
auto operator - (const vec4<T> &lhs, const vec4<T> &rhs)
{
    return vec4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}
template<typename T>
auto operator + (const vec4<T> &lhs, const vec4<T> &rhs)
{
    return vec4<T>(rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z, rhs.w + lhs.w);
}
template<typename T>
auto operator * (const vec4<T> &lhs, const vec4<T> &rhs)
{
    return vec4<T>(rhs.x * lhs.x, rhs.y * lhs.y, rhs.z * lhs.z, rhs.w * lhs.w);
}
template<typename T>
auto operator / (const vec4<T> &lhs, const vec4<T> &rhs)
{
    return vec4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

template<typename T>
auto operator * (T val, const vec4<T> &v)
{
    return v * val;
}

template<typename T>
auto dot(const vec4<T> &lhs, const vec4<T> &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

YCU_MATH_END