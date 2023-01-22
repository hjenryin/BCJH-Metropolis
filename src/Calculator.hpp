#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include "Recipe.hpp"
#include "Chef.hpp"
#include <cmath>
struct BanquetInfo {
    int price; // 不加饱食度的原售价
    int full;  // 饱食度
};
/**
 * @brief 基础售价加成规则
 *
 */
struct BanquetBaseRule {
    Skill skillBuff;   // 基础售价+%（有条件）
    int buff = 0;      // 基础售价+%（无条件）
    int directAdd = 0; // 基础售价+数额
};
/**
 * @brief 售价加成规则
 *
 */
struct BanquetAddRule {
    Skill skillBuff; // 售价+%（有条件）
    int full = 0;    // 饱食度加成
    int buff = 0;    // 售价+%（无条件）
};
struct BanquetRule {
    BanquetAddRule addRule;
    BanquetBaseRule baseRule;
};
int getPrice(Chef &chef, Recipe &recipe, bool verbose = false);

BanquetInfo getPrice(Chef *chef, Recipe *recipe, BanquetRule rule,
                     bool verbose = false);

#endif