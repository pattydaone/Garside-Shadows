#include <iostream>
#include <cmath>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <array>

template <typename I, typename J, typename K>
struct OmegaPoint {
    I i {};
    J j {};
    K k {};

    OmegaPoint(I i, J j, K k) 
        : i { i }, j { j }, k { k } {
    
    }

    std::string pointAsString() {
        return "(" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(k) + ")";
    }
};

struct CartesianPoint {
    double x {};
    double y {};

    CartesianPoint (double x, double y) 
        : x { x }, y { y } {
        
    }
};

class Omega {
    /*
     * Mathematically, definitions and algorithms are pulled from the following article: 
     * https://pmc.ncbi.nlm.nih.gov/articles/PMC8004019/#sec3-entropy-23-00373
     */
    const OmegaPoint<int, int, int> base { 0, 0, 0 };

    int sign(double x) {
        if (x < 0) return -1;
        if (x > 0) return 1;

        return 0;
    }

    int regionSign(double x, double sum) {
        if (x != 0) return sign(x);
        else {
            if (sum <= 0) return 1;
            else return -1;
        }
    }

    int integerPart(double x) {
        return (int)x;
    }

    double fractionalPart(double x) {
        return x - integerPart(x);
    }

    double signedFractionalPart(double x) {
        return fractionalPart(x) < 0 ? fractionalPart(x) : fractionalPart(x) - 1;
    }

    int absRoundingUp(double x) {
        return integerPart(x) + sign(fractionalPart(x));
    }
    
    template<typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    OmegaPoint<double, double, double> directSum(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
        return OmegaPoint<double, double, double> { x.i + y.i, x.j + y.j, x.k + y.k };
    }

    int Rsum(const OmegaPoint<double, double, double>& s) {
        return absRoundingUp(s.i) + absRoundingUp(s.j) + absRoundingUp(s.k);
    }

    template<typename I, typename J, typename K>
    std::string getRegion(const OmegaPoint<I, J, K>& s) {
        double sum { componentSum(s) };
        return std::to_string(regionSign(s.i, sum)) + std::to_string(regionSign(s.j, sum)) + std::to_string(regionSign(s.k, sum));
    }

    template<typename I, typename J, typename K>
    char getType(const OmegaPoint<I, J, K>& x) {
        if (typeid(x.i) == typeid(int)) return 'A';
        else if (typeid(x.j) == typeid(int)) return 'B';
        else if (typeid(x.k) == typeid(int)) return 'C';
        return 'a';
    }

    char sameTypeA(const std::string& region, const int rsum, const double signedJ, const double signedK) {
        if (region == "1-1-1") {
            if (rsum == -1) return 'A';
            if (rsum == 0) return signedJ <= signedK ? 'B' : 'C';
            if (rsum == -2) return signedJ >= signedK ? 'B' : 'C';
        }

        else if (region == "-111") {
            if (rsum == 1) return 'A';
            if (rsum == 0) return signedJ >= signedK ? 'B' : 'C';
            if (rsum == 2) return signedJ <= signedK ? 'B' : 'C';
        }

        else {
            if (rsum == 0) return 'A';
            if (rsum == 1) return signedJ <= signedK ? 'B' : 'C';
            if (rsum == -1) return signedJ >= signedK ? 'B' : 'C';
        }

        return 'a';
    }

    char sameTypeB(const std::string& region, const int rsum, const double signedI, const double signedK) {
        if (region == "-11-1") {
            if (rsum == -1) return 'B';
            if (rsum == 0) return signedI <= signedK ? 'A' : 'C';
            if (rsum == 2) return signedI >= signedK ? 'A' : 'C';
        }

        else if (region == "1-11") {
            if (rsum == 1) return 'B';
            if (rsum == 0) return signedI >= signedK ? 'A' : 'C';
            if (rsum == 2) return signedI <= signedK ? 'A' : 'C';
        }

        else {
            if (rsum == 0) return 'B';
            if (rsum == 1) return signedI <= signedK ? 'A' : 'C';
            if (rsum == -1) return signedI >= signedK ? 'A' : 'C';
        }

        return 'a';
    }

    char sameTypeC(const std::string& region, const int rsum, const double signedI, const double signedJ) {
        if (region == "11-1") {
            if (rsum == 1) return 'C';
            if (rsum == 0) return signedI >= signedJ ? 'A' : 'B';
            if (rsum == 2) return signedI <= signedJ ? 'A' : 'B';
        }

        else if (region == "-1-11") {
            if (rsum == -1) return 'C';
            if (rsum == 0) return signedI <= signedJ ? 'A' : 'B';
            if (rsum == -2) return signedI >= signedJ ? 'A' : 'B';
        }

        else {
            if (rsum == 0) return 'C';
            if (rsum == 1) return signedI <= signedJ ? 'A' : 'B';
            if (rsum == -1) return signedI >= signedJ ? 'A' : 'B';
        }

        return 'a';
    }

    char minRegion(const double signedI, const double signedJ, const double signedK) {
        const double minimum { std::min({signedI, signedJ, signedK}) };
        return minimum == signedI ? 'A' : minimum == signedJ ? 'B' : 'C';
    }

    char maxRegion(const double signedI, const double signedJ, const double signedK) {
        const double maximum { std::max({signedI, signedJ, signedK}) };
        return maximum == signedI ? 'A' : maximum == signedJ ? 'B' : 'C';
    }

    char differentType(const std::string& region, const int rsum, const double signedI, const double signedJ, const double signedK) {
        const std::array<std::string, 3> regions { "1-11", "11-1", "-111" };
        if (std::find(regions.begin(), regions.end(), region) != regions.end()) {
            if (rsum == 0) return maxRegion(signedI, signedJ, signedK);
            if (rsum == 1) return minRegion(signedI, signedJ, signedK);
        }

        if (rsum == 0) return minRegion(signedI, signedJ, signedK);
        if (rsum == -1) return maxRegion(signedI, signedJ, signedK);

        return 'a';
    }

    template<typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    char getResultantType(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y, const OmegaPoint<double, double, double>& s, int rsum) {
        char xtype { getType(x) };
        char ytype { getType(y) };
        double signedI { signedFractionalPart(s.i) };
        double signedJ { signedFractionalPart(s.j) };
        double signedK { signedFractionalPart(s.k) };
        std::string region { getRegion(s) };;

        if (xtype != ytype) return differentType(region, rsum, signedI, signedJ, signedK);

        if (xtype == 'A') return sameTypeA(region, rsum, signedJ, signedK);
        else if (xtype == 'B') return sameTypeB(region, rsum, signedI, signedK);
        else if (xtype == 'C') return sameTypeC(region, rsum, signedI, signedJ);

        return 'a';
    }

public:
    template <typename I, typename J, typename K>
    double componentSum(const OmegaPoint<I, J, K>& m) {
        return m.i + m.j + m.k;
    }

    template <typename I, typename J, typename K>
    CartesianPoint omegaToCartesian(const OmegaPoint<I, J, K>& m) {
        double x { 0.5 * (m.i - m.k ) };
        double y { 0.5 * ( m.i - 2*m.j + m.k) };

        return CartesianPoint { x, y };
    }

    template <typename I, typename J, typename K>
    OmegaPoint<int, int, int> closestMidpoint(const OmegaPoint<I, J, K>& m) {
        OmegaPoint<int, int, int> final { (int)std::ceil(m.i), (int)std::ceil(m.j), (int)std::ceil(m.k) };
        if (componentSum(m) < 0) {
            if (typeid(m.i) == typeid(double)) { --final.i; }
            if (typeid(m.j) == typeid(double)) { --final.j; }
            if (typeid(m.k) == typeid(double)) { --final.k; }
        }

        return final;
    }

    template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
    OmegaPoint<double, double, double> vectorAddition(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
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
                    if (sum > 2) { --r.i; --r.j; --r.k; }
                    else if (sum < -2) { ++r.i; ++r.j; ++r.k; }
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
                    if (sum > 2) { --r.i; --r.j; --r.k; }
                    else if (sum < -2) { ++r.i; ++r.j; ++r.k; }
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
                    if (sum > 2) { --r.i; --r.j; --r.k; }
                    else if (sum < -2) { ++r.i; ++r.j; ++r.k; }
                    return r;
                }
            default:
                throw -1;
        }
    
    }

    template <typename I, typename J, typename K>
    OmegaPoint<I, J, K> cartesianToOmega(CartesianPoint m) {
    
    }

};

int main() {
    Omega cord;
    OmegaPoint<int, double, double> m { 1, -0.2, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.2, -0.4) )" << '\n' << "Expected output: (1, 0, 0)" << '\n';
    std::cout << "Actual output: " << (cord.closestMidpoint(m)).pointAsString() << '\n' << '\n';

    OmegaPoint<int, double, double> n { 1, -0.8, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.8, -0.4) )" << '\n' << "Expected output: (1, -1, -1)" << '\n';
    std::cout << "Actual output: " << (cord.closestMidpoint(n)).pointAsString() << '\n' << '\n';

    {
        std::cout << "Sample (a)" << '\n';
        const OmegaPoint<double, int, double> v1 { 1.577, -2, 0.423 };
        const OmegaPoint<double, int, double> v2 { 1.005, 0, -1.305 };
        std::cout << "Expected output: (2.582, -2, -0.882)" << '\n';
        std::cout << "Actual output: " << (cord.vectorAddition(v1, v2)).pointAsString() << '\n';
    }

    {
        std::cout << "Sample (b)" << '\n';
        const OmegaPoint<double, double, int> v1 { 1.155, -1.423, 0 };
        const OmegaPoint<double, int, double> v2 { 0.808, 0, -0.808 };
        std::cout << "Expected output: (2, -1.386, -0.771)" << '\n';
        std::cout << "Actual output: " << (cord.vectorAddition(v1, v2)).pointAsString() << '\n';
    }

    {
        std::cout << "Sample (c)" << '\n';
        const OmegaPoint<int, double, double> v1 { 1, -0.735, -0.270 };
        const OmegaPoint<int, double, double> v2 { 1, -0.966, -0.732 };
        std::cout << "Expected output: (2.002, -1.699, -1)" << '\n';
        std::cout << "Actual output: " << (cord.vectorAddition(v1, v2)).pointAsString() << '\n';
    }
    return 0;
}
