#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "OmegaPoint.h"

constexpr OmegaPoint<int, int, int> nullPoint { 0, 0, 0 };
constexpr OmegaPoint<int, int, int> translation { 0, -1, 0 };
enum class rgn {
    null = 0,
    pmp,
    pmm,
    ppm,
    mpm,
    mpp,
    mmp,
    ppp
};

#endif
