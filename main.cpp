#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "omega.h"
#include "tests.cpp"

const OmegaPoint<int, int, int> nullPoint { 0, 0, 0 };

/*
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
*/

class A2Tilde {
    const Omega cord {};
    std::string tempword {};
    std::vector<std::string_view> words {};
    std::unordered_map<char, char> generatorRegions {
        {'B', 's'},
        {'C', 't'},
        {'A', 'u'}
    };

    const std::unordered_map<std::string_view, std::array<std::string_view, 2>> pathByRegion {
        {"1-11", {"BC", "BA"}},
        {"1-1-1", {"CA", "BA"}},
        {"11-1", {"CA", "CB"}},
        {"-11-1", {"CB", "AB"}},
        {"-111", {"AC", "AB"}},
        {"-1-11", {"AC", "BC"}}
    };

    void moveRegionA(OmegaPoint<int, int, int>& point, std::string& word) {
        if (cord.componentSum(point) > 0) { --point.j; --point.k; }
        else { ++point.j; ++point.k; }
        word += generatorRegions['A'];
        std::swap(generatorRegions['B'], generatorRegions['C']);
    }

    void moveRegionB(OmegaPoint<int, int, int>& point, std::string& word) {
        if (cord.componentSum(point) > 0) { --point.i; --point.k; }
        else { ++point.i; ++point.k; }
        word += generatorRegions['B'];
        std::swap(generatorRegions['A'], generatorRegions['C']);
    }

    void moveRegionC(OmegaPoint<int, int, int>& point, std::string& word) {
        if (cord.componentSum(point) > 0) { --point.i; --point.j; }
        else { ++point.i; ++point.j; }
        word += generatorRegions['C'];
        std::swap(generatorRegions['A'], generatorRegions['B']);
    }

    void pathFind(OmegaPoint<int, int, int>& startAt, const std::string_view& path, const OmegaPoint<int, int, int>& endAt, std::string& word) {
        int pathIndex { 0 };
        while (startAt != endAt) {
            switch (path[pathIndex]) {
                case 'A': { moveRegionA(startAt, word); break; }
                case 'B': { moveRegionB(startAt, word); break; }
                case 'C': { moveRegionC(startAt, word); break; }
            } 
            pathIndex = !pathIndex;
        }
    }

    std::string determinePath(const OmegaPoint<int, int, int>& point) {
        if (cord.componentSum(point) > 0) {
            if ((point + OmegaPoint<int, int, int>{-1, 1, -1}).k == 0) return "BA";
            else return "BC";
        }

        else {
            if ((point + OmegaPoint<int, int, int>{0, 1, 0}).k == 0) return "BA";
            else return "BC";
        }
    }

public:
    std::string omegaPointToWord(const OmegaPoint<int, int, int>& point) {
        std::string word {};
        OmegaPoint<int, int, int> start { 0, -1, 0 };
        const std::string region { cord.getRegion(point) };
        const auto& path { pathByRegion.at(region) };
        auto decomposedVector { cord.decomposeIntVector(point) };
        decomposedVector[0] = decomposedVector[0] + OmegaPoint<int, int, int> {0, -1, 0};

        // More hacky fixing because edgecases are so fun!!!!!!!!!!!!!!!!!!!!!
        if (decomposedVector[1] == nullPoint) {
        
            if (region == "11-1") {
                pathFind(start, "CB", decomposedVector[0], word);
            }

            else if (region == "-111") {
                pathFind(start, "AB", decomposedVector[0], word);
            }

            else if (region == "1-11") {
                pathFind(start, determinePath(decomposedVector[0]), decomposedVector[0], word);
            }
        }

        else {
            pathFind(start, path[0], decomposedVector[0] , word);
            pathFind(start, path[1], point, word);
        }

        return word;
    }

    OmegaPoint<int, int, int> wordToOmegaPoint(const std::string_view word) {
        OmegaPoint<int, int, int> point { 0, -1, 0 };
        for (auto i : word) {
            switch (i) {
                case 's': {}
                case 't': {}
                case 'u': {}
            }
        }
        return point;
    }
};

void A2TildeTests() {
    A2Tilde group {};
    
    for (auto i : comprehensiveTestingPointsArray) {
        std::cout << "Finding word associated with point " << i << "; " << (testingCord.componentSum(i) > 0 ? "positive " : "negative ") << "triangle in region " << testingCord.getRegion(i) << '\n';
        std::string word { group.omegaPointToWord(i) };
        std::cout << "Word: " << word << '\n' << '\n';
    }
    
}

void A2TildeProblemPoints() {
    A2Tilde group {};
    OmegaPoint<int, int, int> problemPoint { 3, -4, 0 };
    std::cout << "Finding word associated with point " << problemPoint << "; " << (testingCord.componentSum(problemPoint) > 0 ? "positive " : "negative ") << "triangle in region " << testingCord.getRegion(problemPoint) << '\n';
    std::string word { group.omegaPointToWord(problemPoint) };
    std::cout << "Word: " << word << '\n' << '\n';
}

int main() {
    A2TildeTests();
    return 0;
}
