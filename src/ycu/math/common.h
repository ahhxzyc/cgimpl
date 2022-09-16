#pragma once

#define YCU_MATH_BEGIN namespace ycu::math {
#define YCU_MATH_END }

#include <cmath>
#include <cstdint>

#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)

YCU_MATH_BEGIN

template<typename T> constexpr auto radians(T deg);
template<typename T> constexpr auto degrees(T deg);
template<typename T> constexpr auto clamp_to(T val, T left, T right);

template<typename T> 
constexpr auto radians(T deg)
{
    return deg * T(M_PI) / T(180);
}
template<typename T>
constexpr auto degrees(T rad)
{
    return rad * T(180) * T(M_1_PI);
}
template<typename T>
constexpr auto clamp_to(T val, T left, T right)
{
    return std::max(left, std::min(right, val));
}


YCU_MATH_END