#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "omega.h"

constexpr OmegaInt nullPoint { 0, 0, 0 };
constexpr OmegaPoint<int, int, int> translation { 0, -1, 0 };
constexpr Omega testingCord;
enum rgn {
    pmp,
    pmm,
    ppm,
    mpm,
    mpp,
    mmp,
    ppp
};

#endif
