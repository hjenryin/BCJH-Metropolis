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
class BanquetBaseRule {
  public:
    Skill skillBuff;   // 基础售价+%（有条件）
    int buff = 0;      // 基础售价+%（无条件）
    int directAdd = 0; // 基础售价+数额
    BanquetBaseRule() = default;
    void add(BanquetBaseRule &rule) {
        skillBuff.add(rule.skillBuff);
        buff += rule.buff;
        directAdd += rule.directAdd;
    }
};
/**
 * @brief 售价加成规则
 *
 */
class BanquetAddRule {
  public:
    Skill skillBuff;     // 售价+%（有条件）
    int full = 0;        // 饱食度加成
    bool fullAdd = true; // 饱食度加成是否为加法；否则为直接设置
    int buff = 0;        // 售价+%（无条件）
    BanquetAddRule() = default;
    void add(BanquetAddRule &rule) {
        skillBuff.add(rule.skillBuff);
        if (fullAdd) {
            full += rule.full;
        } else {
            full = rule.full;
        }
        buff += rule.buff;
    }
};
class BanquetRule {
  public:
    BanquetAddRule addRule;
    BanquetBaseRule baseRule;
    BanquetRule() = default;
    void add(BanquetRule &rule) {
        addRule.add(rule.addRule);
        baseRule.add(rule.baseRule);
    }
};
int getPrice(Chef &chef, Recipe &recipe, bool verbose = false);

BanquetInfo getPrice(Chef *chef, Recipe *recipe, BanquetRule rule,
                     bool verbose = false);

#endif