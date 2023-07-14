#include "bcjh_js.hpp"
#include "../data/data.hpp"
int main() {
    auto str = runjs(userData_CONST, ruleData_CONST, 3170000, 500, 100, false);
    std::cout << str << std::endl;
    return 0;
}