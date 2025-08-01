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

void testing() {
    ShadowGenerator gen;
    std::vector<OmegaInt> points {{
        { 2, -2, 1 }, { -1, 0, 2 }, { 1, 1, -1 }
    }};

    std::vector<OmegaInt> pointsTwo {{
        { -1, 3, -1 }
    }};

    std::vector<OmegaInt> uniqueShadow {{
            { -1, -2, 2 }
    }};

    gen.addPoints(uniqueShadow);
    gen.generateShadow();
    const std::vector<PointInfo>& shadowpoints { gen.getShadowAsPoints() };
    // std::cout << shadowpoints.size() << '\n';
    std::ofstream pointFile;
    pointFile.open("points.txt");
    for (auto i : shadowpoints) {
        pointFile << i.point << '\n';
    }

    
    std::ofstream file;
    file.open("words.txt");
    const std::vector<std::string>& words {gen.getShadowAsWords()};
    for (auto i = (words.begin()); i < words.end(); ++i) {
        file << *i << '\n';
    }
    file.close();
}

void convertToPoints() {
    std::ifstream file;
    file.open("~/Documents/Summer Research/Picture Code/points.txt");
}

int main() {
    ShadowGenerator gen;

    std::vector<OmegaInt> points {{
        { -1, 3, -1 }
    }};

    std::vector<std::string> inputWords {
        "stustsut"
    };

    gen.addPoints(points);
    gen.generateShadow();

    const std::vector<PointInfo>& shadowpoints { gen.getShadowAsPoints() };

    std::ofstream pointFile;
    pointFile.open("points.txt");

    pointFile << shadowpoints.size() << '\n';
    for (auto i : shadowpoints) {
        pointFile << i.point << '\n';
    }

    
    std::ofstream file;
    file.open("words.txt");

    const std::vector<std::string>& words {gen.getShadowAsWords()};
    for (auto i = (words.begin()); i < words.end(); ++i) {
        file << *i << '\n';
    }
    file.close();
    
    return 0;
}
