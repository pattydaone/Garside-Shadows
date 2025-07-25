#ifndef SHADOWGENERATORS_H
#define SHADOWGENERATORS_H

#include <algorithm>
#include <cstddef>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "omega.h"
#include "constants.h"
#include "groups.h"
#include "pointInfo.h"

using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;

class ShadowGenerator {
    const OmegaInt identity { 0, -1, 0 };
    A2Tilde group {};
    std::vector<std::string> shadowAsWords;
    std::vector<PointInfo> shadowAsPoints{{
            { 0, -1, 0, &getAccessibleRegions, &getDirection }, 
            { 1, -1, 1, &getAccessibleRegions, &getDirection }, 
            { 0, 0, 1, &getAccessibleRegions, &getDirection }, 
            { 1, 0, 0, &getAccessibleRegions, &getDirection }
    }};
    std::vector<PointInfo> toMerge {};
    std::vector<PointInfo*> toJoin {
        &shadowAsPoints[1], &shadowAsPoints[2], &shadowAsPoints[3]
    };
    std::vector<PointInfo*> toReflect {};

    std::unordered_map<rgn, std::string_view> reflectionsByRegion;
    char constA;
    char constB;
    char constC;

    bool isInShadow(const OmegaInt& point) {
        return std::find(shadowAsPoints.begin(), shadowAsPoints.end(), point) != shadowAsPoints.end();
    }

    bool isInMerge(const OmegaInt& point) {
        return std::find(toMerge.begin(), toMerge.end(), point) != toMerge.end();
    }

    static void getAccessibleRegions(std::array<rgn, 3>& arr, const DecompArray& decomp, const OmegaInt& point) {
        rgn pointRegion { Omega::getRegion(point) };
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

    static void getDirection(const OmegaInt& x, std::array<dir, 2>& dirArr) {
        dirArr[1] = dir::N;
        switch (Omega::getRegion(x)) {
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
        return Omega::magnitude(*x) >= Omega::magnitude(*y) ? x : y;
    }

    bool directionalCompatible(const std::array<dir, 2>& firstDirArr, const std::array<dir, 2>& secondDirArr) {
        for (auto i : firstDirArr) {
            if (i == dir::N) continue;

            if (std::find(secondDirArr.begin(), secondDirArr.end(), i) != secondDirArr.end()) return true;
        }

        return false;
    }

    OmegaInt estimatePoint(const PointInfo& x, const PointInfo& y) {
        // TODO: reconsider this entire dogshit
        // i reconsidered and now its worse.
        const DecompInfo& firstPossibleX { x.decomp[0] };
        auto firstPossibleY { std::find_if(y.decomp.begin(), y.decomp.end(), [this, &x](auto&& y) { return directionalCompatible(x.decomp[0].dirs, y.dirs); })};

        const DecompInfo* secondPossibleY { &y.decomp[0] };
        const DecompInfo* secondPossibleX {};
        if (firstPossibleY == secondPossibleY) {
            secondPossibleY = &y.decomp[1];
        }

        if ((*secondPossibleY).point == nullPoint) {
            secondPossibleX = std::find_if_not(x.decomp.begin(), x.decomp.end(), [this, &firstPossibleY](auto&& x) { return directionalCompatible((*firstPossibleY).dirs, x.dirs); });
        }

        else {
            secondPossibleX = std::find_if(x.decomp.begin(), x.decomp.end(), [this, &secondPossibleY](auto&& x) { return directionalCompatible((*secondPossibleY).dirs, x.dirs); });
        }

        const OmegaInt* firstFinal {};
        const OmegaInt* secondFinal {};

        if (firstPossibleY == y.decomp.end()) {
            firstFinal = &firstPossibleX.point;
        }

        else {
            firstFinal = max(&firstPossibleX.point, &(firstPossibleY -> point));
        }

        if (secondPossibleX == x.decomp.end()) {
            secondFinal = &(secondPossibleY -> point);
        }

        else {
            secondFinal = max(&(secondPossibleX -> point), &(secondPossibleY -> point));
        }

        return *firstFinal + *secondFinal + translation;
    }

    void checkOvershooting(OmegaInt& estimate) {
        switch(Omega::getRegion(estimate)) {
            case (rgn::pmp): {
                if (Omega::componentSum(estimate) == 1) {
                    --estimate.i; --estimate.k;
                }
                break;
            }

            case (rgn::mpp): {
                if (Omega::componentSum(estimate) == 1) {
                    --estimate.j; --estimate.k;
                }
                break;
            }

            case (rgn::ppm): {
                if (Omega::componentSum(estimate) == 1) {
                    --estimate.i; --estimate.j;
                }
                break;
            }

            case (rgn::mmp): {
                if (Omega::componentSum(estimate) == -1) {
                    ++estimate.i; ++estimate.j;
                }
                break;
            }

            case (rgn::mpm): {
                if (Omega::componentSum(estimate) == -1) {
                    ++estimate.i; ++estimate.k;
                }
                break;
            }

            case (rgn::pmm): {
                if (Omega::componentSum(estimate) == -1) {
                    ++estimate.j; ++estimate.k;
                }
                break;
            }

            default: break;
        }
    }

    void checkSides(OmegaInt& estimate) {
        switch (Omega::getRegion(estimate)) {
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
        for (auto i { shadowAsPoints.begin() + 1}; i != shadowAsPoints.end(); ++i) {
            if (!joinCompatible(focusPoint -> accessibleRegions, i -> accessibleRegions)) continue;

            auto possibleJoinPoint { estimatePoint(*focusPoint, *i) };

            if (Omega::componentSum(possibleJoinPoint) == 0) {
                checkSides(possibleJoinPoint);
            }

            if (possibleJoinPoint == i -> point || possibleJoinPoint == focusPoint -> point) continue;

            checkOvershooting(possibleJoinPoint);

            if (!isInShadow(possibleJoinPoint) && !isInMerge(possibleJoinPoint)) {
                toMerge.push_back(PointInfo { possibleJoinPoint, &getAccessibleRegions, &getDirection });
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
        std::string_view reflections { reflectionsByRegion[focusPoint -> region] };
        if (((focusPoint -> point).j == 0 && Omega::componentSum(focusPoint -> point) == 1) || ((focusPoint -> point).j == -1 && Omega::componentSum(focusPoint -> point) == -1)) {
            if (focusPoint -> region == rgn::pmm) reflections = "C";
            else if (focusPoint -> region == rgn::mmp) reflections = "A";
            else throw(-1);
        }

        for (auto i : reflections) {
            OmegaInt reflected { reflection(focusPoint -> point, i) };
            if (!isInShadow(reflected) && !isInMerge(reflected)) {
                toMerge.push_back(PointInfo { reflected, &getAccessibleRegions, &getDirection });
                // toReflect.push_back(reflected);
                // toJoin.push_back(reflected);
            }
            if (reflected == OmegaInt{2, 0, -1}) {
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

    const std::vector<PointInfo>& getShadowAsPoints() {
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
        std::size_t offset { shadowAsPoints.size() };
        for (auto i : points) {
            shadowAsPoints.push_back(PointInfo { i, &getAccessibleRegions, &getDirection });
        }
        for (std::size_t i = offset; i < shadowAsPoints.size(); ++i) {
            toJoin.push_back(&shadowAsPoints[i]);
            toReflect.push_back(&shadowAsPoints[i]);
        }
    }

    void addPoints(const std::vector<std::string>& words) {
        shadowAsPoints.reserve(4 + words.size());
        for (auto i : words) {
            OmegaInt wordAsPoint { group.wordToOmegaPoint(i) };
            shadowAsPoints.push_back(PointInfo {wordAsPoint, &getAccessibleRegions, &getDirection});
            toJoin.push_back(&shadowAsPoints.back());
            toReflect.push_back(&shadowAsPoints.back());
        }
    }

    void generateShadow() {
        std::size_t offset;
        while (toJoin.size() != 0 || toReflect.size() != 0) {
            while (toJoin.size() != 0) {
                joinOperation();
            }
            offset = shadowAsPoints.size();
            shadowAsPoints.insert(shadowAsPoints.end(), toMerge.begin(), toMerge.end());
            for (std::size_t i = offset; i < shadowAsPoints.size(); ++i) {
                toJoin.push_back(&shadowAsPoints[i]);
                toReflect.push_back(&shadowAsPoints[i]);
            }
            toMerge.clear();
            
            while (toReflect.size() != 0) {
                reflectionOperation();
            }
            offset = shadowAsPoints.size();
            shadowAsPoints.insert(shadowAsPoints.end(), toMerge.begin(), toMerge.end());
            for (std::size_t i = offset; i < shadowAsPoints.size(); ++i) {
                toJoin.push_back(&shadowAsPoints[i]);
                toReflect.push_back(&shadowAsPoints[i]);
            }
            toMerge.clear();
        }
    }
};

#endif
