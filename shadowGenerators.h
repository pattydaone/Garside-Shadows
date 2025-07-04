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

using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;

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

    dir getDirection(const OmegaInt& x) {
        switch (cord.getRegion(x)) {
            case (rgn::mmp): return dir::L;
            case (rgn::pmm): return dir::R;
            case (rgn::mpp): return dir::LD;
            case (rgn::ppm): return dir::RD;
            case (rgn::pmp): {
                if (x.i > x.k ) return dir::RU;
                else if (x.i < x.k) return dir::LU;
                else return dir::N;
            }
            default: throw(-1);
        }
    }

    const OmegaInt& max(const OmegaInt& x, const OmegaInt& y) {
        return cord.magnitude(x) >= cord.magnitude(y) ? x : y;
    }

    bool directionalCompatible(const OmegaInt& first, const OmegaInt& second) {
        dir firstDirection { getDirection(first + translation) };
        dir secondDirection { getDirection(second + translation) };

        if (firstDirection != dir::N && secondDirection != dir::N) return firstDirection == secondDirection;

        if (firstDirection == dir::N && first != nullPoint) {
            return secondDirection == dir::LU || secondDirection == dir::RU;
        }

        if (secondDirection == dir::N && second != nullPoint) {
            return firstDirection == dir::LU || firstDirection == dir::RU;
        }

        return firstDirection == secondDirection;
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

        /*
        if (secondPossibleX == nullPoint) {
            secondPossibleY = std::find_if_not(yArr.begin(), yArr.end(), 
                    [this, &firstPossibleX](auto&& y) { return getDirection(firstPossibleX + translation) == getDirection(y + translation); });
        }

        else {
            secondPossibleY = std::find_if(yArr.begin(), yArr.end(), 
                    [this, &secondPossibleX](auto&& y) { return getDirection(secondPossibleX + translation) == getDirection(y + translation); });
        }*/


        if (firstPossibleY == yArr.end()) {
            return firstPossibleX + max(*secondPossibleX, *secondPossibleY) + translation;
        }

        if (secondPossibleX == xArr.end()) {
            return max(firstPossibleX, *firstPossibleY) + *secondPossibleY + translation;
        }

        return max(firstPossibleX, *firstPossibleY) + max(*secondPossibleX, *secondPossibleY) + translation;
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

    void joinOperation() {
        auto focusPoint { toJoin.back() };
        toJoin.pop_back();
        auto decomposedFocus { cord.decomposeIntVector(focusPoint) };
        getAccessibleRegions(focusPointAccessRegions, decomposedFocus, focusPoint);
        for (auto i : toJoin) {
            auto decomposedI { cord.decomposeIntVector(i) };
            getAccessibleRegions(iAccessRegions, decomposedI, i);

            if (!joinCompatible(focusPointAccessRegions, iAccessRegions)) continue;

            auto possibleJoinPoint { estimatePoint(decomposedFocus, decomposedI) };
           
            if (possibleJoinPoint != i && possibleJoinPoint != focusPoint) {
                checkOvershooting(possibleJoinPoint);
            }

            std::cout << "Estimated join of " << focusPoint << " and " << i << ": " << possibleJoinPoint << '\n' << '\n';
        }
    }

    void reflection() {
        auto focusPoint { toReflect.back() };
        toReflect.pop_back();
        rgn region { cord.getRegion(focusPoint) };
        
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

    friend void joinTests();

    friend void joinTestsProblemPoints();
};

#endif
