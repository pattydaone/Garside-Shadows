#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "omega.h"
#include "tests.cpp"

constexpr OmegaPoint<int, int, int> nullPoint { 0, 0, 0 };

using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;
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
    const std::unordered_map<char, char> templateGeneratorRegions;
    std::unordered_map<char, char> generatorRegions;

    const std::unordered_map<std::string_view, std::array<std::string_view, 2>> pathByRegion {
        {"1-11", {"BC", "BA"}},
        {"1-1-1", {"CA", "BA"}},
        {"11-1", {"CA", "CB"}},
        {"-11-1", {"CB", "AB"}},
        {"-111", {"AC", "AB"}},
        {"-1-11", {"AC", "BC"}}
    };

    char moveRegionA(OmegaInt& point) {
        if (cord.componentSum(point) > 0) { --point.j; --point.k; }
        else { ++point.j; ++point.k; }
        std::swap(generatorRegions['B'], generatorRegions['C']);
        return generatorRegions['A'];
    }

    char moveRegionB(OmegaInt& point) {
        if (cord.componentSum(point) > 0) { --point.i; --point.k; }
        else { ++point.i; ++point.k; }
        std::swap(generatorRegions['A'], generatorRegions['C']);
        return generatorRegions['B'];
    }

    char moveRegionC(OmegaInt& point) {
        if (cord.componentSum(point) > 0) { --point.i; --point.j; }
        else { ++point.i; ++point.j; }
        std::swap(generatorRegions['A'], generatorRegions['B']);
        return generatorRegions['C'];
    }

    void pathFind(OmegaInt& startAt, const std::string_view& path, const OmegaInt& endAt, std::string& word) { int pathIndex { 0 };
        while (startAt != endAt) {
            switch (path[pathIndex]) {
                case 'A': { word += moveRegionA(startAt); break; }
                case 'B': { word += moveRegionB(startAt); break; }
                case 'C': { word += moveRegionC(startAt); break; }
            } 
            pathIndex = !pathIndex;
        }
    }

    std::string determinePath(const OmegaInt& point) {
        if (cord.componentSum(point) > 0) {
            if ((point + OmegaInt{-1, 1, -1}).k == 0) return "BA";
            else return "BC";
        }

        else {
            if ((point + OmegaInt{0, 1, 0}).k == 0) return "BA";
            else return "BC";
        }
    }

    void resetGeneratorRegions() {
        generatorRegions = templateGeneratorRegions;
    }

public:
    A2Tilde()
        : templateGeneratorRegions {{
                {'B', 's'}, {'C', 't'}, {'A', 'u'}
        }}, generatorRegions { templateGeneratorRegions } {
        
    }

    A2Tilde(const std::unordered_map<char, char>& userDefinedGeneratorRegions) 
        : templateGeneratorRegions { userDefinedGeneratorRegions }
        , generatorRegions { userDefinedGeneratorRegions } {
        
        }


    std::string omegaPointToWord(const OmegaInt& point) {
        resetGeneratorRegions();
        std::string word {};
        OmegaInt start { 0, -1, 0 };
        const std::string region { cord.getRegion(point) };
        const auto& path { pathByRegion.at(region) };
        auto decomposedVector { cord.decomposeIntVector(point) };
        decomposedVector[0] = decomposedVector[0] + OmegaInt {0, -1, 0};

        // More hacky fixing because edgecases are so fun!!!!!!!!!!!!!!!!!!!!!
        if (decomposedVector[1] == nullPoint) {
        
            if (region == "11-1") {
                pathFind(start, "CB", decomposedVector[0], word);
                return word;
            }

            else if (region == "-111") {
                pathFind(start, "AB", decomposedVector[0], word);
                return word;
            }

            else if (region == "1-11") {
                pathFind(start, determinePath(decomposedVector[0]), decomposedVector[0], word);
                return word;
            }
        }

        pathFind(start, path[0], decomposedVector[0], word);
        pathFind(start, path[1], point, word);


        return word;
    }

    OmegaInt wordToOmegaPoint(const std::string_view word) {
        resetGeneratorRegions();
        OmegaInt point { 0, -1, 0 };
        for (auto i : word) {
            auto it = std::find_if(generatorRegions.begin(), generatorRegions.end(), [&i](auto&& it) -> bool { return it.second == i; });
            switch (it -> first) {
                case 'A': { moveRegionA(point); break; }
                case 'B': { moveRegionB(point); break; }
                case 'C': { moveRegionC(point); break; }
            }
        }
        return point;
    }
};

class ShadowGenerator {
    const Omega cord;
    const OmegaInt identity { 0, -1, 0 };
    A2Tilde group {};
    std::vector<std::string_view> shadowAsWords;
    std::vector<OmegaInt> shadowAsPoints{{
            { 0, -1, 0 }, { 1, -1, 1 }, { 0, 0, 1 }, { 1, 0, 0 }
    }};
    std::vector<OmegaInt> toJoin {{
            { 1, -1, 1}, { 0, 0, 1 }, { 1, 0, 0 }
    }};
    std::vector<OmegaInt> toReflect;

    std::unordered_map<std::string_view, std::string_view> reflectionsByRegion;

    std::array<std::string, 3> focusPointAccessRegions {};
    std::array<std::string, 3> iAccessRegions {};

    void getAccessibleRegions(std::array<std::string, 3>& arr, const DecompArray& decomp, const OmegaInt& point) {
        std::string pointRegion { cord.getRegion(point) };
        arr[0] = pointRegion;
        arr[2] = "";
        if (decomp[0] != nullPoint && decomp[1] != nullPoint) {
            arr[1] = "";
            return;
        }

        if (pointRegion == "1-1-1") {
            arr[1] = "11-1";
            if (point == OmegaInt{1, 0, 0}) arr[2] = "-11-1";
            return;
        }

        if (pointRegion == "11-1") {
            arr[1] = "-11-1";
            return;
        }

        if (pointRegion == "-111") {
            arr[1] = "-11-1";
            return;
        }

        if (pointRegion == "-1-11") {
            arr[1] = "-111";
            if (point == OmegaInt{0, 0, 1}) arr[2] = "-11-1";
            return;
        }

        if (pointRegion == "1-11") {
            
            // We want to find the orientation of the point with respect to the identity, so we can check by finding the x cord in cartesian and checking pos/neg.
            // Since the conversion for the x cord is sqrt(3)/2 (i - k), we can throw out the constant as it doesnt say anything about sign, so 
            // i - k > 0 <=> i > k => right; i - k < 0 <=> i < k => left
            if (point.i < point.k) {
                arr[1] = "1-1-1";
                return;
            }

            else if (point.i > point.k) {
                arr[1] = "-1-11";
                return;
            }

            // If this executes, it means cartesian x == 0, so the point must be (0, -1, 0), which can join with either of the following regions
            else {
                arr[1] = "-1-11";
                arr[2] = "1-1-1";
                return;
            }
        }
    }

    bool joinCompatible(const std::array<std::string, 3>& xArray, const std::array<std::string, 3>& yArray) {
        for (auto i : xArray) {
            if (i == "") continue;
            if (std::find(yArray.begin(), yArray.end(), i) != yArray.end()) return true;
        }
        return false;
    }

    void joinOperation() {
        auto focusPoint { toJoin.back() };
        toJoin.pop_back();
        auto decomposedFocus { cord.decomposeIntVector(focusPoint) };
        getAccessibleRegions(focusPointAccessRegions, decomposedFocus, focusPoint);
        for (auto i : toJoin) {
            auto decomposedI { cord.decomposeIntVector(i) };
            getAccessibleRegions(iAccessRegions, decomposedI, i);

            if (!joinCompatible(focusPointAccessRegions, iAccessRegions)) continue;

            auto possibleJoinPoint { decomposedFocus[0] + decomposedI[0] };
            std::cout << possibleJoinPoint << '\n';
        }
    }

    void reflection() {
        auto focusPoint { toReflect.back() };
        toReflect.pop_back();
        std::string region { cord.getRegion(focusPoint) };
        
    }

public:
    ShadowGenerator()
        : reflectionsByRegion { {"1-11", "s"}, {"-11-1", "ut"}, {"-111", "u"},
                                {"11-1", "t"}, {"-1-11", "su"}, {"1-1-1", "st"} } {
        
        }

    ShadowGenerator(const std::unordered_map<char, char>& userDefinedGeneratorRegions) 
        : group { userDefinedGeneratorRegions }
        , reflectionsByRegion { {"1-11", std::to_string(userDefinedGeneratorRegions.at('B'))}, 
                                {"-11-1", std::to_string(userDefinedGeneratorRegions.at('C')) + userDefinedGeneratorRegions.at('A')},
                                {"-111", std::to_string(userDefinedGeneratorRegions.at('A'))},
                                {"11-1", std::to_string(userDefinedGeneratorRegions.at('C'))},
                                {"-1-11", std::to_string(userDefinedGeneratorRegions.at('B')) + userDefinedGeneratorRegions.at('A')},
                                {"1-1-1", std::to_string(userDefinedGeneratorRegions.at('B')) + userDefinedGeneratorRegions.at('C')} } {
        
    }

};

void A2TildeTests() {
    A2Tilde group {};
    
    for (auto i : comprehensiveTestingPointsArray) {
        std::cout << "Finding word associated with point " << i << "; " << (testingCord.componentSum(i) > 0 ? "positive " : "negative ") << "triangle in region " << testingCord.getRegion(i) << '\n';
        std::string word { group.omegaPointToWord(i) };
        std::cout << "Word: " << word << '\n';
        std::cout << "Finding point associated with word " << word << '\n';
        auto point { group.wordToOmegaPoint(word) };
        std::cout << "Point: " << point << '\n' << (point == i) << '\n' << '\n';
    }

    std::cout << "Testing word 'stustsu': " << group.wordToOmegaPoint("stustsu") << '\n' << '\n';
    
}

void A2TildeProblemPoints() {
    A2Tilde group {};
    std::array<OmegaInt, 4> problemPoints {{
            {-2, -1, 2}, {-1, 0, 2}, {2, 0, -1}, {2, -1, -2}
    }};

    for (auto i : problemPoints) {
        std::cout << "Finding word associated with point " << i << "; " << (testingCord.componentSum(i) > 0 ? "positive " : "negative ") << "triangle in region " << testingCord.getRegion(i) << '\n';
        std::string word { group.omegaPointToWord(i) };
        std::cout << "Word: " << word << '\n' << '\n';
    }
}

int main() {
    ShadowGenerator shadow {};
    return 0;
}
