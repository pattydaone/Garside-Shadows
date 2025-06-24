#include <iostream>
#include "constants.h"
#include "omega.h"
#include "groups.h"
#include "shadowGenerators.h"

const std::array<OmegaPoint<int, int, int>, 36> comprehensiveTestingPointsArray {
    {
        {4, -3, 0}, {3, -4, 0}, {3, -3, 1}, {2, -4, 1}, {2, -3, 2},
        {1, -4, 2}, {1, -3, 3}, {0, -4, 3}, {0, -3, 4}, {-1, -3, 3},
        {0, -2, 3}, {-1, -2, 2}, {-1, -1, 3}, {-2, -1, 2}, {-1, 0, 2},
        {-2, 0, 1}, {-2, 1, 2}, {-3, 1, 1}, {-2, 2, 1}, {-3, 2, 0},
        {-2, 3, 0}, {-2, 2, -1}, {-1, 3, -1}, {-1, 2, -2}, {0, 3, -2},
        {0, 2, -3}, {1, 2, -2}, {1, 1, -3}, {2, 1, -2}, {1, 0, -2},
        {2, 0, -1}, {2, -1, -2}, {3, -1, -1}, {2, -2, -1}, {3, -2, 0},
        {3, -3, -1}
    }
};

void midpointTests() {
    OmegaPoint<int, double, double> m { 1, -0.2, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.2, -0.4) )" << '\n' << "Expected output: (1, 0, 0)" << '\n';
    std::cout << "Actual output: " << testingCord.closestMidpoint(m) << '\n' << '\n';
    OmegaPoint<int, double, double> n { 1, -0.8, -0.4 };
    std::cout << "Evaluating: closestMidpoint( (1, -0.8, -0.4) )" << '\n' << "Expected output: (1, -1, -1)" << '\n';
    std::cout << "Actual output: " << testingCord.closestMidpoint(n) << '\n' << '\n';
}

template<typename I, typename J, typename K>
void manualMidpointTest(const OmegaPoint<I, J, K>& point) {
    std::cout << "Evaluating closest midpoint to: " << point << '\n';
    std::cout << "Closest midpoint: " << testingCord.closestMidpoint(point) << '\n';
}

void vectorAdditionTests() {
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
        std::cout << "Actual output: " << v1 + v2 << '\n';
    }

    {
        std::cout << "Sample (c)" << '\n';
        const OmegaPoint<int, double, double> v1 { 1, -0.735, -0.270 };
        const OmegaPoint<int, double, double> v2 { 1, -0.966, -0.732 };
        std::cout << "Expected output: (2.002, -1.699, -1)" << '\n';
        std::cout << "Actual output: " << v1 + v2 << '\n';
    }
    
    {
        std::cout << "Sample (d)" << '\n';
        const OmegaPoint<int, int, int> v1 { 3, -4, 0 };
        const OmegaPoint<int, int, int> v2 { 0, -1, 0 };
        std::cout << "Expected output: idk" << '\n';
        std::cout << "Actual output: " << v1 + v2 << '\n';
    }
}

template<typename I1, typename I2, typename J1, typename J2, typename K1, typename K2>
void manualVectorAdditionTest(const OmegaPoint<I1, J1, K1>& v1, const OmegaPoint<I2, J2, K2>& v2, const std::string& message) {
    std::cout << "Evaluating " << v1 << " + " << v2 << "(" << message << ")" << '\n';
    std::cout << v1 + v2 << '\n';
}

void vectorDecompositionTests() {
    const OmegaPoint<int, int, int> translation { 0, -1, 0 };

    for (auto i : comprehensiveTestingPointsArray) {
        std::cout << "Decomposing point " << i << "; " << (testingCord.componentSum(i) > 0 ? "positive " : "negative ") << "triangle in region " << testingCord.getRegion(i) << '\n';
        auto decomposed { testingCord.decomposeIntVector(i) };
        std::cout << "a: " << decomposed[0] << '\n';
        std::cout << "b: " << decomposed[1] << '\n';
        OmegaPoint<int, int, int> added { decomposed[0] + decomposed[1] };
        std::cout << "Added: " << added << '\n';
        if ((i - added) != translation) {
            std::cout << "POINT " << i << " FAILED" << '\n';
        }
        std::cout << '\n';
    }

}

void manualDecompositionTest(const OmegaPoint<int, int, int>& point) {
    std::cout << "Decomposing vector " << point << '\n';
    auto decomposed { testingCord.decomposeIntVector(point) };
    std::cout << "A: " << decomposed[0] << '\n';
    std::cout << "B: " << decomposed[1] << '\n';
    std::cout << "Added: " << decomposed[0] + decomposed[1] << '\n';
}

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
