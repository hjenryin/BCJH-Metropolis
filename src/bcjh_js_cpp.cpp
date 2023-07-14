#include "bcjh_js.hpp"
#include "../data/data.hpp"
int main() {
    int8_t progress = 0;
    auto str =
        runjs(userData_CONST, ruleData_CONST, 3170000, 50000, 100, false);
    std::cout << str << std::endl;
    return 0;
}