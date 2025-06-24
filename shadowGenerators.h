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

    std::string getDirection(const OmegaInt& x) {
        std::string region { cord.getRegion(x) };

        // Null point check
        if (region == "111") return "N";

        if (region == "-1-11") return "L";
        if (region == "1-1-1") return "R";
        if (region == "-111") return "LD";
        if (region == "11-1") return "RD";
        
        if (x.i - x.k >= 0) return "RU";
        else return "LU";
    }

    const OmegaInt& max(const OmegaInt& x, const OmegaInt& y) {
        return cord.magnitude(x) >= cord.magnitude(y) ? x : y;
    }

    OmegaInt estimatePoint(const DecompArray& xArr, const DecompArray& yArr) {
        const OmegaInt& firstPossibleX { xArr[0] };
        auto firstPossibleY { std::find_if(yArr.begin(), yArr.end(), [this, &firstPossibleX](auto&& y) { return getDirection(firstPossibleX + translation) == getDirection(y + translation); })};
        
        const OmegaInt& secondPossibleX { xArr[1] };
        auto secondPossibleY { std::find_if_not(yArr.begin(), yArr.end(), [this, &secondPossibleX](auto&& y) { return getDirection(secondPossibleX + translation) == getDirection(y + translation); })};

        if (firstPossibleY == yArr.end()) {
            return firstPossibleX + max(secondPossibleX, *secondPossibleY);
        }

        if (secondPossibleY == yArr.end()) {
            return max(firstPossibleX, *firstPossibleY) + *secondPossibleY;
        }

        return max(firstPossibleX, *firstPossibleY) + max(secondPossibleX, *secondPossibleY);
    }
public:
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
            std::cout << possibleJoinPoint << '\n';
        }
    }
private:
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

#endif
