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
    int buff = 0;      // 基础售价+%（无条件）
    int directAdd = 0; // 基础售价+数额
    BanquetBaseRule() = default;
    void add(BanquetBaseRule &rule) {
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
    int full = 0;        // 饱食度加成
    bool fullAdd = true; // 饱食度加成是否为加法；否则为直接设置
    int buff = 0;        // 售价+%（无条件）
    BanquetAddRule() = default;
    void add(BanquetAddRule &rule) {
        if (fullAdd) {
            full += rule.full;
        } else {
            full = rule.full;
        }
        buff += rule.buff;
    }
};
struct BanquetRule;
/**
 * @brief 描述“下阶段blahblah”的规则，即不受“意图生效次数”的影响
 *
 */
struct BanquetStrictRule {
    BanquetAddRule addRule;
    BanquetBaseRule baseRule;
};
class BanquetLenientRule {
    friend BanquetInfo getPrice(Chef *, Recipe *, BanquetRule, bool);

  public:
    BanquetAddRule addRule;
    BanquetBaseRule baseRule;
    BanquetLenientRule() = default;
    void oneMore() { this->duplicateTime += 1; }

  private:
    int duplicateTime = 0;
    void execOneMore(BanquetLenientRule &rule) {
        while (this->duplicateTime > 0) {
            this->add(rule);
            this->duplicateTime -= 1;
        }
    }
    void merge(BanquetStrictRule &rule) {
        BanquetLenientRule oldRule = *this;
        execOneMore(oldRule);
        addRule.add(rule.addRule);
        baseRule.add(rule.baseRule);
    }
    void add(BanquetLenientRule &rule) {
        addRule.add(rule.addRule);
        baseRule.add(rule.baseRule);
    }
};
struct BanquetRule {
    BanquetLenientRule lenientRule;
    BanquetStrictRule
        strictRule; // 描述“下阶段blahblah”的规则，即不受“意图生效次数”的影响
};
struct ActivityBuff {
    std::map<int, int> materialBuff;
    std::map<int, int> rarityBuff; // 几火就是几，不用减一
    std::map<FlavorEnum, int> flavorBuff;
};
int getPrice(Chef &chef, Recipe &recipe, ActivityBuff *p = NULL,
             bool verbose = false);
BanquetInfo getPrice(Chef *chef, Recipe *recipe, BanquetRule rule,
                     bool verbose = false);

#endif