#ifndef OMEGAPOINT_H
#define OMEGAPOINT_H
#include <iostream>

template <typename I, typename J, typename K>
struct OmegaPoint {
    I i {};
    J j {};
    K k {};

    constexpr OmegaPoint(I i, J j, K k) 
        : i { i }, j { j }, k { k } {
    
    }

    OmegaPoint() = default;
    
    constexpr OmegaPoint(const OmegaPoint<I, J, K>& point) 
        : i { point.i }, j { point.j }, k { point.k } {
    
    }

    template<typename II, typename JJ, typename KK>
    OmegaPoint(const OmegaPoint<II, JJ, KK>& point) 
        : i { (int)point.i }, j { (int)point.j }, k { (int)point.k } {
        
    }

    template<typename II, typename JJ, typename KK>
    OmegaPoint<int, int, int>& operator=(const OmegaPoint<II, JJ, KK>& point) {
        i = (int)point.i;
        j = (int)point.j;
        k = (int)point.k;

        return *this;
    }

    OmegaPoint<int, int, int>& operator=(const OmegaPoint<int, int, int>& point) {
        i = point.i;
        j = point.j;
        k = point.k;

        return *this;
    }
};

template<typename I, typename J, typename K>
inline std::ostream& operator<<(std::ostream& out, const OmegaPoint<I, J, K>& point) {
    out << "(" << std::to_string(point.i) << ", " << std::to_string(point.j) << ", " << std::to_string(point.k) << ")";
    return out;
}


template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
inline bool operator==(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return x.i != y.i ? false : x.j != y.j ? false : x.k != y.k ? false : true; // most readable code oat
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
inline bool operator!=(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return !(x == y);
}

struct CartesianPoint {
    double x {};
    double y {};

    CartesianPoint (double x, double y) 
        : x { x }, y { y } {
        
    }
};

inline std::ostream& operator<<(std::ostream& out, const CartesianPoint& point) {
    out << "(" << std::to_string(point.x) << ", " << std::to_string(point.y) << ")";
    return out;
}
#endif
