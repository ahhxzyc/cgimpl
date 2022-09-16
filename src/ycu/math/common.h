#pragma once

#define YCU_MATH_BEGIN namespace ycu::math {
#define YCU_MATH_END }

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

YCU_MATH_BEGIN

template<typename T> T radians(T deg);
template<typename T> T degrees(T deg);

template<typename T> 
T radians(T deg)
{
    return deg * T(M_PI) / T(180);
}
template<typename T>
T degrees(T rad)
{
    return rad * T(180) * T(M_1_PI);
}


YCU_MATH_END