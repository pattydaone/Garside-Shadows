#ifndef OMEGA_H
#define OMEGA_H

#include <cmath>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <array>
#include "OmegaPoint.h"
#include "constants.h"


class Omega {
    /*
     * Definitions and addition/subtraction algorithms are pulled from the following article: 
     * https://pmc.ncbi.nlm.nih.gov/articles/PMC8004019/#sec3-entropy-23-00373
     *
     * Decomposition algorithm can be found on my github TODO: Add link to this
     */

    static int sign(double x) {
        if (x < 0) return -1;
        if (x > 0) return 1;

        return 0;
    }

    static int regionSign(double x, double sum) {
        if (x != 0) return sign(x);
        return sum <= 0 ? 1 : -1;
    }

    static int integerPart(double x) {
        return (int)x;
    }

    static double fractionalPart(double x) {
        return x - integerPart(x);
    }

    static double signedFractionalPart(double x) {
        return fractionalPart(x) < 0 ? fractionalPart(x) : fractionalPart(x) - 1;
    }

    static int absRoundingUp(double x) {
        return integerPart(x) + sign(fractionalPart(x));
    }
    
    template<typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    static OmegaPoint<double, double, double> directSum(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
        return OmegaPoint<double, double, double> { x.i + y.i, x.j + y.j, x.k + y.k };
    }

    static int Rsum(const OmegaPoint<double, double, double>& s) {
        return absRoundingUp(s.i) + absRoundingUp(s.j) + absRoundingUp(s.k);
    }

    template<typename I, typename J, typename K>
    static char getType(const OmegaPoint<I, J, K>& x) {
        if (typeid(x.i) == typeid(int)) return 'A';
        else if (typeid(x.j) == typeid(int)) return 'B';
        else if (typeid(x.k) == typeid(int)) return 'C';
        return 'a';
    }

    static char sameTypeA(const rgn& region, const int rsum, const double signedJ, const double signedK) {
        switch (region) {
            case (rgn::pmm): {
                if (rsum == -1) return 'A';
                if (rsum == 0) return signedJ <= signedK ? 'B' : 'C';
                if (rsum == -2) return signedJ >= signedK ? 'B' : 'C';
            }
            
            case (rgn::mpp): {
                if (rsum == 1) return 'A';
                if (rsum == 0) return signedJ >= signedK ? 'B' : 'C';
                if (rsum == 2) return signedJ <= signedK ? 'B' : 'C';
            }
            
            default: {
                if (rsum == 0) return 'A';
                if (rsum == 1) return signedJ <= signedK ? 'B' : 'C';
                if (rsum == -1) return signedJ >= signedK ? 'B' : 'C';
            }
        }
        
        // I return A here because the given procedure does not succeed for only integer component vectors (lol),
        // or I've completely messed up the implementation (more likely and also lol).
        // As I have implemented in code, if a vector borders region A at all, it is classified as A. 
        // Therefore, dual-A vectors which only border A will be caught by this
        return 'A';
    }

    static char sameTypeB(const rgn& region, const int rsum, const double signedI, const double signedK) {
        switch (region) {
            case (rgn::mpm): {
                if (rsum == -1) return 'B';
                if (rsum == 0) return signedI <= signedK ? 'A' : 'C';
                if (rsum == 2) return signedI >= signedK ? 'A' : 'C';
            }

            case (rgn::pmp): {
                if (rsum == 1) return 'B';
                if (rsum == 0) return signedI >= signedK ? 'A' : 'C';
                if (rsum == 2) return signedI <= signedK ? 'A' : 'C';
            }

            default: {
                if (rsum == 0) return 'B';
                if (rsum == 1) return signedI <= signedK ? 'A' : 'C';
                if (rsum == -1) return signedI >= signedK ? 'A' : 'C';
            }
        }

        // Similar reason to sameTypeA
        return 'B';
    }

    static char sameTypeC(const rgn& region, const int rsum, const double signedI, const double signedJ) {
        switch (region) {
            case (rgn::ppm): {
                if (rsum == 1) return 'C';
                if (rsum == 0) return signedI >= signedJ ? 'A' : 'B';
                if (rsum == 2) return signedI <= signedJ ? 'A' : 'B';
            }

            case (rgn::mmp): {
                if (rsum == -1) return 'C';
                if (rsum == 0) return signedI <= signedJ ? 'A' : 'B';
                if (rsum == -2) return signedI >= signedJ ? 'A' : 'B';
            }

            default: {
                if (rsum == 0) return 'C';
                if (rsum == 1) return signedI <= signedJ ? 'A' : 'B';
                if (rsum == -1) return signedI >= signedJ ? 'A' : 'B';
            }
        }

        // I return 'a' here because it indicates "error" in my switch statement; I do not return 'C' because, as I understand 
        // my code, this return should never happen. Thus, if it does, I think it better to investigate why rather than just 
        // return 'C' now.
        return 'a';
    }

    static char minRegion(const double signedI, const double signedJ, const double signedK) {
        const double minimum { std::min({signedI, signedJ, signedK}) };
        return minimum == signedI ? 'A' : minimum == signedJ ? 'B' : 'C';
    }

    static char maxRegion(const double signedI, const double signedJ, const double signedK) {
        const double maximum { std::max({signedI, signedJ, signedK}) };
        return maximum == signedI ? 'A' : maximum == signedJ ? 'B' : 'C';
    }

    static char differentType(const rgn& region, const int rsum, const double signedI, const double signedJ, const double signedK) {
        const std::array<rgn, 3> regions { rgn::pmp, rgn::ppm, rgn::mpp };
        if (std::find(regions.begin(), regions.end(), region) != regions.end()) {
            if (rsum == 0) return maxRegion(signedI, signedJ, signedK);
            if (rsum == 1) return minRegion(signedI, signedJ, signedK);
        }

        if (rsum == 0) return minRegion(signedI, signedJ, signedK);
        if (rsum == -1) return maxRegion(signedI, signedJ, signedK);

        // Same reasoning as sameTypeC
        return 'a';
    }

    template<typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    static char getResultantType(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y, const OmegaPoint<double, double, double>& s, int rsum) {
        char xtype { getType(x) };
        char ytype { getType(y) };
        double signedI { signedFractionalPart(s.i) };
        double signedJ { signedFractionalPart(s.j) };
        double signedK { signedFractionalPart(s.k) };
        rgn region { getRegion(s) };

        if (xtype != ytype) return differentType(region, rsum, signedI, signedJ, signedK);

        if (xtype == 'A') return sameTypeA(region, rsum, signedJ, signedK);
        else if (xtype == 'B') return sameTypeB(region, rsum, signedI, signedK);
        else if (xtype == 'C') return sameTypeC(region, rsum, signedI, signedJ);

        // This can happen *if and only if* none of the data members in x are integers
        return 'a';
    }

    static void aboveRight(OmegaPoint<int, int, int>& vec) {
        while (vec.i != 0 && vec.j != 0 && vec.k != 0) {
            --vec.i; ++vec.j;
        }
    }

    static void aboveLeft(OmegaPoint<int, int, int>& vec) {
        while (vec.i != 0 && vec.j != 0 && vec.k != 0) {
            --vec.k; ++vec.j;
        }
    }

    static void belowRight(OmegaPoint<int, int, int>& vec) {
        while (vec.i != 0 && vec.j != 0 && vec.k != 0) {
            --vec.j; ++vec.k;
        }
    }

    static void belowLeft(OmegaPoint<int, int, int>& vec) {
        while (vec.i != 0 && vec.j != 0 && vec.k != 0) {
            --vec.j; ++vec.i;
        }
    }

    static const std::string placementRelativeToIdentity(const OmegaPoint<int, int, int>& vec) {
        std::string toReturn;
        vec.i > 0 ? toReturn += "R" : toReturn += "L";
        vec.j < 0 ? toReturn += "A" : toReturn += "B";
        return toReturn;
    }

    static std::array<OmegaPoint<int, int, int>, 2> primaryDecomposeAlgorithm(OmegaPoint<int, int, int>& vecFromOrigin) {
        OmegaPoint<int, int, int> a { vecFromOrigin };
        const std::string placement { placementRelativeToIdentity(a) };
        if (placement == "RA") aboveRight(a);
        else if (placement == "LA") aboveLeft(a);
        else if (placement == "RB") belowRight(a);
        else if (placement == "LB") belowLeft(a);
        else throw(-1);

        // This sucks but that's my fault
        // TODO: fix this shit
        const OmegaPoint<int, int, int> b { vectorSubtraction(vecFromOrigin, a) };

        std::array<OmegaPoint<int, int, int>, 2> toReturn { a, b };
        return toReturn;
    }

public:
    Omega() = delete;

    template <typename I, typename J, typename K>
    static double componentSum(const OmegaPoint<I, J, K>& m) {
        return double(m.i + m.j + m.k);
    }

    template<typename I, typename J, typename K>
    static rgn getRegion(const OmegaPoint<I, J, K>& s) {
        double sum { componentSum(s) };
        int iSign { regionSign(s.i, sum) };
        int jSign { regionSign(s.j, sum) };
        int kSign { regionSign(s.k, sum) };

        switch (iSign + jSign + kSign) {
            case (3): 
                return rgn::ppp;
            case (1): {
                if (iSign == -1) return rgn::mpp;
                if (jSign == -1) return rgn::pmp;
                return rgn::ppm;
            }
            default: {
                if (iSign == 1) return rgn::pmm;
                if (jSign == 1) return rgn::mpm;
                return rgn::mmp;
            }
        }
    }

    template <typename I, typename J, typename K>
    static CartesianPoint omegaToCartesian(const OmegaPoint<I, J, K>& m) {
        double x { 0.5 * std::sqrt(3)*(m.i - m.k ) };
        double y { 0.5 * ( m.i - 2*m.j + m.k) };

        return CartesianPoint { x, y };
    }

    // TODO: fix this for edgepoints
    template <typename I, typename J, typename K>
    static OmegaPoint<int, int, int> closestMidpoint(const OmegaPoint<I, J, K>& m) {
        OmegaPoint<int, int, int> final { (int)std::ceil(m.i), (int)std::ceil(m.j), (int)std::ceil(m.k) };
        if (componentSum(m) < 0) {
            if (typeid(m.i) == typeid(double)) { --final.i; }
            if (typeid(m.j) == typeid(double)) { --final.j; }
            if (typeid(m.k) == typeid(double)) { --final.k; }
        }

        return final;
    }

    template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    static OmegaPoint<double, double, double> vectorAddition(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
        OmegaPoint<double, double, double> s { (double)(x.i + y.i), (double)(x.j + y.j), (double)(x.k + y.k) };
        int rsum { Rsum(s) };

        switch (getResultantType(x, y, s, rsum)) {
            case 'A': 
                {
                    double addedI { fractionalPart(x.i) + fractionalPart(y.i) };
                    double i { s.i - addedI };
                    double j { s.j - addedI };
                    double k { s.k - addedI };
                    OmegaPoint<double, double, double> r { i, j, k };
                    double sum { componentSum(r) };
                    // The paper states this step as being ">" and "<". I suspect this is wrong, as I am getting exactly = 2,
                    // and exactly = 2 is not within our bounds. I believe these should be ">=" and "<=" respectively
                    if (sum >= 2) { --r.i; --r.j; --r.k; }
                    else if (sum <= -2) { ++r.i; ++r.j; ++r.k; }
                    return r;
                }
            case 'B':
                {
                    double addedJ { fractionalPart(x.j) + fractionalPart(y.j) };
                    double i { s.i - addedJ };
                    double j { s.j - addedJ };
                    double k { s.k - addedJ };
                    OmegaPoint<double, double, double> r { i, j, k };
                    double sum { componentSum(r) };
                    if (sum >= 2) { --r.i; --r.j; --r.k; }
                    else if (sum <= -2) { ++r.i; ++r.j; ++r.k; }
                    return r;
                }
            case 'C':
                {
                    double addedK { fractionalPart(x.k) + fractionalPart(y.k) };
                    double i { s.i - addedK };
                    double j { s.j - addedK };
                    double k { s.k - addedK };
                    OmegaPoint<double, double, double> r { i, j, k };
                    double sum { componentSum(r) };
                    if (sum >= 2) { --r.i; --r.j; --r.k; }
                    else if (sum <= -2) { ++r.i; ++r.j; ++r.k; }
                    return r;
                }
            default:
                throw -1;
        }
    
    }

    template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    static OmegaPoint<double, double, double> vectorSubtraction(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
        OmegaPoint<I2, J2, K2> yOpposite { -1*y.i, -1*y.j, -1*y.k };
        return vectorAddition(x, yOpposite);
    }
    
    static const std::array<OmegaPoint<int, int, int>, 2> decomposeIntVector(const OmegaPoint<int, int, int>& midpoint) {
        if (componentSum(midpoint) != 1 && componentSum(midpoint) != -1) throw(-1);

        if (componentSum(midpoint) < 0) {
            OmegaPoint<int, int, int> edgePoint { vectorSubtraction(midpoint, translation) };
            return primaryDecomposeAlgorithm(edgePoint);
        }

        else {
            // GUESS WHAT ANOTHER FREAKING EDGECASE EXCEPTION YIPPEEEEEEEE I LOVE THIS SO MUCH
            if (midpoint == OmegaPoint<int, int, int>{ 1, -1, 1 }) {
                return {{
                        { 0, -1, 0 }, { 0, 0, 0 }
                }};
            }

            // this is so dogshit i wanna throw up
            OmegaPoint<int, int, int> midpointBelow { vectorSubtraction(midpoint, translation) };
            OmegaPoint<int, int, int> edgePoint { vectorSubtraction(midpointBelow, translation) };

            // HARDCODED EDGECASE BECAUSE THIS SHIT MAKES ME WANNA DIE
            if (midpoint.j == 0) {
                return {{
                    midpointBelow, {0, 0, 0}
                }};
            }

            auto firstArray { primaryDecomposeAlgorithm(edgePoint) };

            // MORE HARDCODED EDGECASE! I WANNA DIE!!!!!!
            if (firstArray[1] == OmegaPoint<int, int, int> { 0, 0, 0 } && firstArray[0].j != 0) {
                firstArray[0] = vectorAddition(firstArray[0], translation);
                return firstArray;
            }

            OmegaPoint<int, int, int> translatedB { vectorAddition(firstArray[1], translation) };

            const std::array<OmegaPoint<int, int, int>, 2> toReturn { firstArray[0], translatedB };

            return toReturn;
        }
    }

    template<typename I1, typename J1, typename K1, typename K2, typename J2, typename I2>
    static double dotProduct(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
        CartesianPoint xCart { omegaToCartesian(x) };
        CartesianPoint yCart { omegaToCartesian(y) };
        return xCart.x*yCart.x + xCart.y*yCart.y;
    }

    template<typename I, typename J, typename K>
    static double magnitude(const OmegaPoint<I, J, K>& point) {
        const CartesianPoint cart { omegaToCartesian(point) };
        return std::hypot(cart.x, cart.y);
    }

};

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
inline OmegaPoint<double, double, double> operator+(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return Omega::vectorAddition(x, y);
}

inline OmegaPoint<int, int, int> operator+(const OmegaPoint<int, int, int>& x, const OmegaPoint<int, int, int>& y) {
    return Omega::vectorAddition(x, y);
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
inline OmegaPoint<double, double, double> operator-(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return Omega::vectorSubtraction(x, y);
}

#endif
