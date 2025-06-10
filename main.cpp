#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "omega.h"

template<typename I, typename J, typename K>
std::ostream& operator<<(std::ostream& out, const OmegaPoint<I, J, K>& point) {
    out << "(" << std::to_string(point.i) << ", " << std::to_string(point.j) << ", " << std::to_string(point.k) << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CartesianPoint& point) {
    out << "(" << std::to_string(point.x) << ", " << std::to_string(point.y) << ")";
    return out;
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
OmegaPoint<double, double, double> operator+(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    Omega cord; // this sucks but its my fault !
    return cord.vectorAddition(x, y);
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
OmegaPoint<double, double, double> operator-(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    Omega cord;
    return cord.vectorSubtraction(x, y);
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
bool operator==(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return x.i != y.i ? false : x.j != y.j ? false : x.k != y.k ? false : true; // most readable code oat
}

template <typename I1, typename J1, typename K1, typename I2, typename J2, typename K2>
bool operator!=(const OmegaPoint<I1, J1, K1>& x, const OmegaPoint<I2, J2, K2>& y) {
    return !(x == y);
}


class A2Tilde {
    Omega cord {};
    std::string tempword {};
    std::vector<std::string_view> words {};
    std::unordered_map<char, char> generatorRegions {
        {'B', 's'},
        {'C', 't'},
        {'A', 'u'}
    };

    const std::unordered_map<std::string_view, std::array<std::string_view, 2>> pathByRegion {
        {"1-11", {"BA", "CB"}},
        {"1-1-1", {"AC", "BA"}},
        {"11-1", {"CA", "BC"}},
        {"-11-1", {"AB", "CB"}},
        {"-111", {"AC", "BA"}},
        {"-1-11", {"AC", "BC"}}
    };

    void moveRegionA(OmegaPoint<int, int, int>& point, std::string& word) {
        --point.j; --point.k;
        word += generatorRegions['A'];
        std::swap(generatorRegions['B'], generatorRegions['C']);
    }

    void moveRegionB(OmegaPoint<int, int, int>& point, std::string& word) {
        --point.i; --point.k;
        word += generatorRegions['B'];
        std::swap(generatorRegions['A'], generatorRegions['C']);
    }

    void moveRegionC(OmegaPoint<int, int, int>& point, std::string& word) {
        --point.i; --point.j;
        word += generatorRegions['C'];
        std::swap(generatorRegions['A'], generatorRegions['B']);
    }

    void pathFind(OmegaPoint<int, int, int>& startAt, const std::string_view& path, const OmegaPoint<int, int, int>& endAt, std::string& word) {
        int pathIndex { 0 };
        while (startAt != endAt) {
            switch (path[pathIndex]) {
                case 'A': moveRegionA(startAt, word);
                case 'B': moveRegionB(startAt, word);
                case 'C': moveRegionC(startAt, word);
            } 
            pathIndex = !pathIndex;
        }
    }

public:
    std::string omegaPointToWord(const OmegaPoint<int, int, int>& point) {
        std::string word {};
        OmegaPoint<int, int, int> start { 0, -1, 0 };
        const std::string_view region { cord.getRegion(point) };
        const auto& path { pathByRegion.at(region) };
        const auto decomposedVector { cord.decomposeIntVector(point) };

        pathFind(start, path[0], decomposedVector[0], word);
        pathFind(start, path[1], point, word);

        return word;
    }

    OmegaPoint<int, int, int> wordToOmegaPoint(const std::string_view word) {
    
    }
};

int main() {
    Omega cord;
    /*
    OmegaPoint<int, double, double> m { 1, -0.2, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.2, -0.4) )" << '\n' << "Expected output: (1, 0, 0)" << '\n';
    std::cout << "Actual output: " << cord.closestMidpoint(m) << '\n' << '\n';

    OmegaPoint<int, double, double> n { 1, -0.8, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.8, -0.4) )" << '\n' << "Expected output: (1, -1, -1)" << '\n';
    std::cout << "Actual output: " << cord.closestMidpoint(n) << '\n' << '\n';

    {
        std::cout << "Sample (a)" << '\n';
        const OmegaPoint<double, int, double> v1 { 1.577, -2, 0.423 };
        const OmegaPoint<double, int, double> v2 { 1.005, 0, -1.305 };
        std::cout << "Expected output: (2.582, -2, -0.882)" << '\n';
        std::cout << "Actual output: " << v1 + v2 << '\n';
    }

    {
        std::cout << "Sample (b)" << '\n';
        const OmegaPoint<double, double, int> v1 { 1.155, -1.423, 0 };
        const OmegaPoint<double, int, double> v2 { 0.808, 0, -0.808 };
        std::cout << "Expected output: (2, -1.386, -0.771)" << '\n';
        std::cout << "Actual output: " << cord.vectorAddition(v1, v2) << '\n';
    }

    {
        std::cout << "Sample (c)" << '\n';
        const OmegaPoint<int, double, double> v1 { 1, -0.735, -0.270 };
        const OmegaPoint<int, double, double> v2 { 1, -0.966, -0.732 };
        std::cout << "Expected output: (2.002, -1.699, -1)" << '\n';
        std::cout << "Actual output: " << cord.vectorAddition(v1, v2) << '\n';
    }
    
    {
        std::cout << "Sample (d)" << '\n';
        const OmegaPoint<int, int, int> v1 { 3, -4, 0 };
        const OmegaPoint<int, int, int> v2 { 0, -1, 0 };
        std::cout << "Expected output: idk" << '\n';
        std::cout << "Actual output: " << cord.vectorSubtraction(v1, v2) << '\n';
    }
    */
    {
        std::cout << "Decomposition test 1" << '\n';
        const OmegaPoint<int, int, int> point { 2, -5, 2 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1] << '\n';
    }
    /*
    {
        std::cout << "Decomposition test 2" << '\n';
        const OmegaPoint<int, int, int> point { 3, -3, -1 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
    }

    {
        std::cout << "Decomposition test 3" << '\n';
        const OmegaPoint<int, int, int> point { -1, -3, 3 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
    }
    */
    {
        std::cout << "Decomposition test 4" << '\n';
        const OmegaPoint<int, int, int> point { 2, -3, 2 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1] << '\n';
    }
    /*
    {
        std::cout << "Decomposition test 5" << '\n';
        const OmegaPoint<int, int, int> point { 4, -3, 0 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        auto added {cord.vectorAddition(decomposed[0], decomposed[1]) };
        std::cout << "Added: " << added << '\n';
    }

    {
        std::cout << "Decomposition test 6" << '\n';
        const OmegaPoint<int, int, int> point { 0, -2, 3 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        auto added {cord.vectorAddition(decomposed[0], decomposed[1]) };
        std::cout << "Added: " << added << '\n';
    }

    {
        std::cout << "Decomposition test 7" << '\n';
        const OmegaPoint<int, int, int> point { 0, -4, 3 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        auto added {cord.vectorAddition(decomposed[0], decomposed[1]) };
        std::cout << "Added: " << added << '\n';
    }
    
    {
        // This test is to a positive triangle which has a direct path from the identity.
        // It decomposes in such a way that we get two vectors back, this is due to vector (a) always landing in a negative triangle.
        // I'm not sure if this is a problem as of right now, but do bear in mind.
        std::cout << "Decomposition test 8" << '\n';
        const OmegaPoint<int, int, int> point { 4, -4, 1 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1] << '\n';
    }
    
    {
        std::cout << "Decomposition test 9" << '\n';
        const OmegaPoint<int, int, int> point { 0, -4, 3 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1]<< '\n';
    }
    
    {
        std::cout << "Decomposition test 10" << '\n';
        const OmegaPoint<int, int, int> point { -2, 3, -2 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1]<< '\n';
    }
    
    {
        std::cout << "Decomposition test 11" << '\n';
        const OmegaPoint<int, int, int> point { -1, 3, -1 };
        auto decomposed { cord.decomposeIntVector(point) };
        std::cout << "A: " << decomposed[0] << '\n';
        std::cout << "B: " << decomposed[1] << '\n';
        std::cout << "Added: " << decomposed[0] + decomposed[1]<< '\n';
    }
    */
    return 0;
}
