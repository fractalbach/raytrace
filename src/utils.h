#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

#include <cstdlib>
#include <random>

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// Returns a random real in [min,max).
// inline double random_double(double min, double max)
// {
//     return min + (max - min) * random_double();
// }

// Returns a random real in [0,1).
// inline double random_double()
// {
//     return rand() / (RAND_MAX + 1.0);
// }

// Returns a random real in [0,1).
inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937                           generator;
    return distribution(generator);
}

#endif