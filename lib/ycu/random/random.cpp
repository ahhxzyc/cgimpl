#include "random.h"
#include <random>

YCU_RANDOM_BEGIN


float random_float()
{
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(0, 1);
    return dist(rng);
}

YCU_RANDOM_END