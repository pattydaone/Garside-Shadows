#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "OmegaPoint.h"
#include "omega.h"
#include "constants.h"
#include "groups.h"
#include "shadowGenerators.h"
#include "tests.cpp"


using OmegaInt = OmegaPoint<int, int, int>;
using DecompArray = std::array<OmegaInt, 2>;




int main() {
    joinTests(); // checked up through (-2, 3, 0)
    return 0;
}
