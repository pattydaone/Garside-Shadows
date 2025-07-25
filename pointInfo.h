#ifndef POINTINFO_H
#define POINTINFO_H

#include <iostream>
#include <array>
#include "constants.h"
#include "OmegaPoint.h"
#include "omega.h"

using OmegaInt = OmegaPoint<int, int, int>;
struct DecompInfo {
    OmegaInt point;
    std::array<dir, 2> dirs;

    DecompInfo(const OmegaInt& suppliedPoint, void(*getDirection)(const OmegaInt& x, std::array<dir, 2>& dirArr))
    : point { suppliedPoint } {
        getDirection(suppliedPoint + translation, dirs);
    }

    DecompInfo() = default;

    DecompInfo(const DecompInfo& copy)
    : point { copy.point }
    , dirs { copy.dirs } {
    }

    DecompInfo& operator=(const DecompInfo& supplied) {
        point = supplied.point;
        dirs = supplied.dirs;
        
        return *this;
    }
};

struct PointInfo {
    OmegaPoint<int, int, int> point;
    rgn region;
    std::array<DecompInfo, 2> decomp {};
    std::array<rgn, 3> accessibleRegions;

    PointInfo(int i, int j, int k, void(*getAccessRegions)(std::array<rgn, 3>& arr, const std::array<OmegaInt, 2>& decomp, const OmegaInt& point),
              void(*getDirection)(const OmegaInt& x, std::array<dir, 2>& dirArr)) 
    : point ( {i, j, k } ) {
        std::array<OmegaInt, 2> temp { Omega::decomposeIntVector(point) };
        region = Omega::getRegion(point);
        getAccessRegions(accessibleRegions, temp, point);
        decomp[0] = DecompInfo { temp[0], getDirection };
        decomp[1] = DecompInfo { temp[1], getDirection };
    }

    PointInfo(const OmegaInt& suppliedPoint, void(*getAccessRegions)(std::array<rgn, 3>& arr, const std::array<OmegaInt, 2>& decomp, const OmegaInt& point),
              void(*getDirection)(const OmegaInt& x, std::array<dir, 2>& dirArr)) 
    : point ( suppliedPoint ) {
        std::array<OmegaInt, 2> temp { Omega::decomposeIntVector(point) };
        region = Omega::getRegion(point);
        getAccessRegions(accessibleRegions, temp, point);
        decomp[0] = DecompInfo { temp[0], getDirection };
        decomp[1] = DecompInfo { temp[1], getDirection };
    }
};

inline bool operator==(const PointInfo& info, const OmegaInt& point) {
    return info.point == point;
}

#endif
