#ifndef GROUPS_H
#define GROUPS_H

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <array>
#include "omega.h"
#include "constants.h"
#include "pointInfo.h"

using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;

class A2Tilde {
    const std::unordered_map<char, char> templateGeneratorRegions;
    std::unordered_map<char, char> generatorRegions;

    const std::unordered_map<rgn, std::array<std::string_view, 2>> pathByRegion {
        {rgn::pmp, {"BC", "BA"}},
        {rgn::pmm, {"CA", "BA"}},
        {rgn::ppm, {"CA", "CB"}},
        {rgn::mpm, {"CB", "AB"}},
        {rgn::mpp, {"AC", "AB"}},
        {rgn::mmp, {"AC", "BC"}}
    };

    char moveRegionA(OmegaInt& point) {
        if (Omega::componentSum(point) > 0) { --point.j; --point.k; }
        else { ++point.j; ++point.k; }
        std::swap(generatorRegions['B'], generatorRegions['C']);
        return generatorRegions['A'];
    }

    char moveRegionB(OmegaInt& point) {
        if (Omega::componentSum(point) > 0) { --point.i; --point.k; }
        else { ++point.i; ++point.k; }
        std::swap(generatorRegions['A'], generatorRegions['C']);
        return generatorRegions['B'];
    }

    char moveRegionC(OmegaInt& point) {
        if (Omega::componentSum(point) > 0) { --point.i; --point.j; }
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
        if (Omega::componentSum(point) > 0) {
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
        const rgn region { Omega::getRegion(point) };
        const auto& path { pathByRegion.at(region) };
        auto decomposedVector { Omega::decomposeIntVector(point) };
        decomposedVector[0] = (decomposedVector[0] + OmegaInt {0, -1, 0});

        // More hacky fixing because edgecases are so fun!!!!!!!!!!!!!!!!!!!!!
        if (decomposedVector[1] == nullPoint) {
            switch (region) {
                case (rgn::ppm): {
                    pathFind(start, "CB", decomposedVector[0], word);
                    return word;
                }
                                 
                case (rgn::mpp): {
                    pathFind(start, "AB", decomposedVector[0], word);
                    return word;
                }

                case (rgn::pmp): {
                    pathFind(start, determinePath(decomposedVector[0]), decomposedVector[0], word);
                    return word;
                }

                default: break;
            }
        }

        pathFind(start, path[0], decomposedVector[0], word);
        pathFind(start, path[1], point, word);


        return word;
    }

    std::string omegaPointToWord(const PointInfo& info) {
        resetGeneratorRegions();
        std::string word {};
        OmegaInt start { 0, -1, 0 };
        const rgn region { Omega::getRegion(info.point) };
        const auto& path { pathByRegion.at(region) };
        OmegaInt translatedDecomp { (info.decomp[0].point + OmegaInt {0, -1, 0}) };

        if (info.decomp[1].point == nullPoint) {
            switch (region) {
                case (rgn::ppm): {
                    pathFind(start, "CB", translatedDecomp, word);
                    return word;
                }
                                 
                case (rgn::mpp): {
                    pathFind(start, "AB", translatedDecomp, word);
                    return word;
                }

                case (rgn::pmp): {
                    pathFind(start, determinePath(translatedDecomp), translatedDecomp, word);
                    return word;
                }

                default: break;
            }
        }

        pathFind(start, path[0], translatedDecomp, word);
        pathFind(start, path[1], info.point, word);


        return word;
    }

    OmegaInt wordToOmegaPoint(const std::string_view word) {
        resetGeneratorRegions();
        OmegaInt point { 0, -1, 0 };
        for (auto i : word) {
            auto it { std::find_if(generatorRegions.begin(), generatorRegions.end(), [&i](auto&& it) -> bool { return it.second == i; }) };
            switch (it -> first) {
                case 'A': { moveRegionA(point); break; }
                case 'B': { moveRegionB(point); break; }
                case 'C': { moveRegionC(point); break; }
            }
        }
        return point;
    }

    std::string reduceWord(const std::string& word) {
        return omegaPointToWord(wordToOmegaPoint(word));
    }
};

#endif
