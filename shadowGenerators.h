#ifndef SHADOWGENERATORS_H
#define SHADOWGENERATORS_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "omega.h"
#include "constants.h"
#include "groups.h"

using OmegaInt = OmegaInt;
using DecompArray = std::array<OmegaInt, 2>;

class ShadowGenerator {
    const Omega cord;
    const OmegaInt identity { 0, -1, 0 };
    A2Tilde group {};
    std::vector<std::string> shadowAsWords;
    std::vector<OmegaInt> shadowAsPoints{{
            { 0, -1, 0 }, { 1, -1, 1 }, { 0, 0, 1 }, { 1, 0, 0 }
    }};
    std::vector<OmegaInt> toMerge {};
    std::vector<OmegaInt> toJoin {{
            { 1, -1, 1}, { 0, 0, 1 }, { 1, 0, 0 }
    }};
    std::vector<OmegaInt> toReflect {};

    std::unordered_map<rgn, std::string_view> reflectionsByRegion;
    char constA;
    char constB;
    char constC;

    std::array<rgn, 3> focusPointAccessRegions {};
    std::array<rgn, 3> iAccessRegions {};

    enum class dir {
        N,
        L,
        R,
        LD,
        RD,
        LU,
        RU
    };

    std::array<dir, 2> firstDirArr {};
    std::array<dir, 2> secondDirArr {};

    bool isInShadow(const OmegaInt& point) {
        return std::find(shadowAsPoints.begin(), shadowAsPoints.end(), point) != shadowAsPoints.end();
    }

    bool isInMerge(const OmegaInt& point) {
        return std::find(toMerge.begin(), toMerge.end(), point) != toMerge.end();
    }

    void getAccessibleRegions(std::array<rgn, 3>& arr, const DecompArray& decomp, const OmegaInt& point) {
        rgn pointRegion { cord.getRegion(point) };
        arr[0] = pointRegion;
        arr[2] = rgn::null;
        if (decomp[0] != nullPoint && decomp[1] != nullPoint) {
            arr[1] = rgn::null;
            return;
        }

        switch (pointRegion) {
        
            case (rgn::pmm): {
                arr[1] = rgn::ppm;
                if (point == OmegaInt{1, 0, 0}) arr[2] = rgn::mpm;
                break;
            }

            case (rgn::ppm): {
                arr[1] = rgn::mpm;
                break;
            }

            case (rgn::mpp): {
                arr[1] = rgn::mpm;
                break;
            }

            case (rgn::mmp): {
                arr[1] = rgn::mpp;
                if (point == OmegaInt{0, 0, 1}) arr[2] = rgn::mpm;
                break;
            }

            case (rgn::pmp): {
                
                // We want to find the orientation of the point with respect to the identity, so we can check by finding the x cord in cartesian and checking pos/neg.
                // Since the conversion for the x cord is sqrt(3)/2 (i - k), we can throw out the constant as it doesnt say anything about sign, so 
                // i - k > 0 <=> i > k => right; i - k < 0 <=> i < k => left
                if (point.i > point.k) {
                    arr[1] = rgn::pmm;
                    break;
                }

                else if (point.i < point.k) {
                    arr[1] = rgn::mmp;
                    break;
                }

                // If this executes, it means cartesian x == 0, so the point must be (0, -1, 0), which can join with either of the following regions
                else {
                    arr[1] = rgn::mmp;
                    arr[2] = rgn::pmm;
                    break;
                }
            }

            default: throw(-1);
        }
    }

    bool joinCompatible(const std::array<rgn, 3>& xArray, const std::array<rgn, 3>& yArray) {
        for (auto i : xArray) {
            if (i == rgn::null) continue;
            if (std::find(yArray.begin(), yArray.end(), i) != yArray.end()) return true;
        }
        return false;
    }

    void getDirection(const OmegaInt& x, std::array<dir, 2>& dirArr) {
        dirArr[1] = dir::N;
        switch (cord.getRegion(x)) {
            case (rgn::mmp): dirArr[0] = dir::L; break;
            case (rgn::pmm): dirArr[0] = dir::R; break;
            case (rgn::mpp): dirArr[0] = dir::LD; break;
            case (rgn::ppm): dirArr[0] = dir::RD; break;
            case (rgn::pmp): {
                if (x.i > x.k ) dirArr[0] = dir::RU;
                else if (x.i < x.k) dirArr[0] = dir::LU;
                else if (x != translation) {
                    dirArr[0] = dir::RU;
                    dirArr[1] = dir::LU;
                }
                else dirArr[0] = dir::N;

                break;
            }
            default: throw(-1);
        }

        if (x == OmegaInt{0, 0, 1}) {
            dirArr[1] = dir::LD;
        }

        else if (x == OmegaInt{1, 0, 0}) {
            dirArr[1] = dir::RD;
        }
    }

    const OmegaInt* max(const OmegaInt* x, const OmegaInt* y) {
        return cord.magnitude(*x) >= cord.magnitude(*y) ? x : y;
    }

    bool directionalCompatible(const OmegaInt& first, const OmegaInt& second) {
        getDirection(first + translation, firstDirArr);
        getDirection(second + translation, secondDirArr);

        for (auto i : firstDirArr) {
            if (i == dir::N) continue;

            if (std::find(secondDirArr.begin(), secondDirArr.end(), i) != secondDirArr.end()) return true;
        }

        return false;
    }

    OmegaInt estimatePoint(const DecompArray& xArr, const DecompArray& yArr) {
        // TODO: reconsider this entire dogshit
        // i reconsidered and now its worse.
        const OmegaInt& firstPossibleX { xArr[0] };
        auto firstPossibleY { std::find_if(yArr.begin(), yArr.end(), [this, &firstPossibleX](auto&& y) { return directionalCompatible(firstPossibleX, y); })};
        
        const OmegaInt* secondPossibleY { &yArr[0] };
        const OmegaInt* secondPossibleX {};
        if (firstPossibleY == secondPossibleY) {
            secondPossibleY = &yArr[1];
        }

        if (*secondPossibleY == nullPoint) {
            secondPossibleX = std::find_if_not(xArr.begin(), xArr.end(), [this, &firstPossibleY](auto&& x) { return directionalCompatible(*firstPossibleY, x); });
        }

        else {
            secondPossibleX = std::find_if(xArr.begin(), xArr.end(), [this, &secondPossibleY](auto&& x) { return directionalCompatible(*secondPossibleY, x); });
        }

        const OmegaInt* firstFinal {};
        const OmegaInt* secondFinal {};

        if (firstPossibleY == yArr.end()) {
            firstFinal = &firstPossibleX;
        }

        else {
            firstFinal = max(&firstPossibleX, firstPossibleY);
        }

        if (secondPossibleX == xArr.end()) {
            secondFinal = secondPossibleY;
        }

        else {
            secondFinal = max(secondPossibleX, secondPossibleY);
        }

        return *firstFinal + *secondFinal + translation;
    }

    void checkOvershooting(OmegaInt& estimate) {
        switch(cord.getRegion(estimate)) {
            case (rgn::pmp): {
                if (cord.componentSum(estimate) == 1) {
                    --estimate.i; --estimate.k;
                }
                break;
            }

            case (rgn::mpp): {
                if (cord.componentSum(estimate) == 1) {
                    --estimate.j; --estimate.k;
                }
                break;
            }

            case (rgn::ppm): {
                if (cord.componentSum(estimate) == 1) {
                    --estimate.i; --estimate.j;
                }
                break;
            }

            case (rgn::mmp): {
                if (cord.componentSum(estimate) == -1) {
                    ++estimate.i; ++estimate.j;
                }
                break;
            }

            case (rgn::mpm): {
                if (cord.componentSum(estimate) == -1) {
                    ++estimate.i; ++estimate.k;
                }
                break;
            }

            case (rgn::pmm): {
                if (cord.componentSum(estimate) == -1) {
                    ++estimate.j; ++estimate.k;
                }
                break;
            }

            default: break;
        }
    }

    void checkSides(OmegaInt& estimate) {
        switch (cord.getRegion(estimate)) {
            case (rgn::pmp): {
                if (estimate.k == 0) {
                    ++estimate.i;
                    break;
                }

                else if (estimate.i == 0) {
                    ++estimate.k;
                    break;
                }

                ++estimate.j;
                break;
            }

            case (rgn::ppm): {
                ++estimate.k;
                break;
            }

            case (rgn::mpp): {
                ++estimate.i;
                break;
            }

            case (rgn::pmm): {
                ++estimate.i;
                break;
            }

            case (rgn::mpm): {
                ++estimate.j;
                break;
            }

            case (rgn::mmp): {
                ++estimate.k;
                break;
            }

            case (rgn::ppp): {
                ++estimate.j;
                break;
            }

            default: throw(-1);
            
        }
    }

    void joinOperation() {
        auto focusPoint { toJoin.back() };
        toJoin.pop_back();
        auto decomposedFocus { cord.decomposeIntVector(focusPoint) };
        getAccessibleRegions(focusPointAccessRegions, decomposedFocus, focusPoint);
        for (auto i { shadowAsPoints.begin() + 1}; i != shadowAsPoints.end(); ++i) {
            auto decomposedI { cord.decomposeIntVector(*i) };
            getAccessibleRegions(iAccessRegions, decomposedI, *i);

            if (!joinCompatible(focusPointAccessRegions, iAccessRegions)) continue;

            auto possibleJoinPoint { estimatePoint(decomposedFocus, decomposedI) };

            if (cord.componentSum(possibleJoinPoint) == 0) {
                checkSides(possibleJoinPoint);
            }

            if (possibleJoinPoint == *i || possibleJoinPoint == focusPoint) continue;

            checkOvershooting(possibleJoinPoint);

            // std::cout << "Estimated join of " << focusPoint << " and " << i << ": " << possibleJoinPoint << '\n' << '\n';

            if (!isInShadow(possibleJoinPoint) && !isInMerge(possibleJoinPoint)) {
                toMerge.push_back(possibleJoinPoint);
                // toReflect.push_back(possibleJoinPoint);
                // toJoin.push_back(possibleJoinPoint); // ?
            }
            if (possibleJoinPoint == OmegaInt{-1, -2, 2} || possibleJoinPoint == OmegaInt{2, -2, -1}) {
                continue;
            }
        }

    }

    const OmegaInt reflectA(const OmegaInt& point) {
        OmegaInt toReturn { -1*point.i, -1*point.k, -1*point.j };

        return toReturn;
    }

    const OmegaInt reflectB(const OmegaInt& point) {
        OmegaInt toReturn { -1*point.k, -1*point.j, -1*point.i };

        ++toReturn.i;
        ++toReturn.k;
        toReturn.j -=2;

        return toReturn;
    }

    const OmegaInt reflectC(const OmegaInt& point) {
        OmegaInt toReturn { -1*point.j, -1*point.i, -1*point.k };

        return toReturn;
    }

    const OmegaInt reflection(const OmegaInt& point, const char plane) {
        switch (plane) {
            case ('A'): {
                return reflectA(point);
            }

            case ('B'): {
                return reflectB(point);
            }

            case ('C'): {
                return reflectC(point);
            }

            default: throw(-1);
        }
    }

    void reflectionOperation() {
        auto focusPoint { toReflect.back() };
        toReflect.pop_back();
        rgn region { cord.getRegion(focusPoint) };
        std::string_view reflections { reflectionsByRegion[region] };
        if ((focusPoint.j == 0 && cord.componentSum(focusPoint) == 1) || (focusPoint.j == -1 && cord.componentSum(focusPoint) == -1)) {
            if (region == rgn::pmm) reflections = "C";
            else if (region == rgn::mmp) reflections = "A";
            else throw(-1);
        }

        for (auto i : reflections) {
            OmegaInt reflected { reflection(focusPoint, i) };
            if (!isInShadow(reflected) && !isInMerge(reflected)) {
                toMerge.push_back(reflected);
                // toReflect.push_back(reflected);
                // toJoin.push_back(reflected);
            }
            if (reflected == OmegaInt{-1, -2, 2} || reflected == OmegaInt{2, -2, -1}) {
                continue;
            }
        }
    }

public:
    ShadowGenerator() 
        : reflectionsByRegion { {rgn::pmp, "B"}, {rgn::mpm, "AC"}, {rgn::mpp, "A"},
                                {rgn::ppm, "C"}, {rgn::mmp, "AB"}, {rgn::pmm, "BC"} }
        , constA {'u'}
        , constB {'s'}
        , constC {'t'} {
        
        }

    // TODO: fix this
    ShadowGenerator(const std::unordered_map<char, char>& userDefinedGeneratorRegions) 
        : group { userDefinedGeneratorRegions }
        , reflectionsByRegion { {rgn::pmp, std::to_string(userDefinedGeneratorRegions.at('B'))}, 
                                {rgn::mpm, std::to_string(userDefinedGeneratorRegions.at('C')) + userDefinedGeneratorRegions.at('A')},
                                {rgn::mpp, std::to_string(userDefinedGeneratorRegions.at('A'))},
                                {rgn::ppm, std::to_string(userDefinedGeneratorRegions.at('C'))},
                                {rgn::mmp, std::to_string(userDefinedGeneratorRegions.at('B')) + userDefinedGeneratorRegions.at('A')},
                                {rgn::pmm, std::to_string(userDefinedGeneratorRegions.at('B')) + userDefinedGeneratorRegions.at('C')} }
        , constA { userDefinedGeneratorRegions.at('A') }
        , constB { userDefinedGeneratorRegions.at('B') }
        , constC { userDefinedGeneratorRegions.at('C') } {
        
    }

    friend void joinTests();

    friend void joinTestsProblemPoints();

    friend void firstReflectionTests();

    friend void shadowGenerationTests(const std::vector<OmegaInt>& points);

    const std::vector<OmegaInt>& getShadowAsPoints() {
        return shadowAsPoints;
    }

    const std::vector<std::string>& getShadowAsWords() {
        shadowAsWords.reserve(shadowAsPoints.size());
        for (auto i : shadowAsPoints) {
            shadowAsWords.push_back(group.omegaPointToWord(i));
        }
        return shadowAsWords;
    }

    void addPoints(const std::vector<OmegaInt>& points) {
        shadowAsPoints.insert(shadowAsPoints.end(), points.begin(), points.end());
        toJoin.insert(toJoin.end(), points.begin(), points.end());
        toReflect.insert(toReflect.end(), points.begin(), points.end());
    }

    void generateShadow() {
        while (toJoin.size() != 0 || toReflect.size() != 0) {
            while (toJoin.size() != 0) {
                joinOperation();
            }
            shadowAsPoints.insert(shadowAsPoints.end(), toMerge.begin(), toMerge.end());
            toJoin.insert(toJoin.end(), toMerge.begin(), toMerge.end());
            toReflect.insert(toReflect.end(), toMerge.begin(), toMerge.end());
            toMerge.clear();
            
            while (toReflect.size() != 0) {
                reflectionOperation();
            }
            shadowAsPoints.insert(shadowAsPoints.end(), toMerge.begin(), toMerge.end());
            toJoin.insert(toJoin.end(), toMerge.begin(), toMerge.end());
            toReflect.insert(toReflect.end(), toMerge.begin(), toMerge.end());
            toMerge.clear();
        }
    }
};

#endif
