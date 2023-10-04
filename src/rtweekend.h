#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include "constants.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#include <cmath>
#include <memory>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// Intervals

#endif