#include <cstddef>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "OmegaPoint.h"
#include "groups.h"
#include "shadowGenerators.h"
#include "tests.cpp"

using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;

void inputByWords(const std::vector<std::string>& input) {
    ShadowGenerator gen;

    gen.addPoints(input);
    gen.generateShadow();

    const std::vector<PointInfo>& shadowpoints { gen.getShadowAsPoints() };

    std::ofstream file;
    file.open("points.txt");

    for (auto i : shadowpoints) {
        file << i.point << '\n';
    }
    file.close();

    file.open("inputPoints.txt");
    for (std::size_t i = 4; i < 4 + input.size(); ++i) {
        file << shadowpoints[i].point << '\n';
    }
    file.close();
    
    file.open("words.txt");
    const std::vector<std::string>& words {gen.getShadowAsWords()};
    for (auto i = (words.begin()); i < words.end(); ++i) {
        file << *i << '\n';
    }
    file.close();
}

void inputByPoints(const std::vector<OmegaInt>& input) {
    ShadowGenerator gen;

    gen.addPoints(input);
    gen.generateShadow();

    const std::vector<PointInfo>& shadowpoints { gen.getShadowAsPoints() };

    std::ofstream file;
    file.open("points.txt");

    for (auto i : shadowpoints) {
        file << i.point << '\n';
    }
    file.close();

    file.open("inputPoints.txt");
    for (std::size_t i = 4; i < 4 + input.size(); ++i) {
        file << shadowpoints[i].point << '\n';
    }
    file.close();
    
    file.open("words.txt");
    const std::vector<std::string>& words {gen.getShadowAsWords()};
    for (auto i = (words.begin()); i < words.end(); ++i) {
        file << *i << '\n';
    }
    file.close();
}

int main() {
    std::vector<OmegaInt> points {{
        { -1, 3, -1 }
    }};

    std::vector<std::string> inputWords {
        "stustsut"
    };

    inputByPoints(points);
    // inputByWords(inputWords);

    return 0;
}
