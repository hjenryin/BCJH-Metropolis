#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include "Recipe.hpp"
#include "Chef.hpp"
#include <cmath>
#include <cassert>
extern double calculatePriceTime;
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
        assert((this->full == rule.full) || (this->fullAdd || rule.fullAdd));
        // 要么二者数值相同（如execOneMore），要么至少有一个是加成（即不同时直接设置）
        if (!this->fullAdd) {
            ;
        } else if (!rule.fullAdd) {
            full = rule.full;
        } else {
            full += rule.full;
        }
        buff += rule.buff;
    }
};
struct BanquetRuleTogether;

class BanquetRule {
  public:
    BanquetAddRule addRule;
    BanquetBaseRule baseRule;
};
/**
 * @brief 描述“下阶段blahblah”的规则，即不受“意图生效次数”的影响
 *
 */
struct BanquetStrictRule : public BanquetRule {
    void oneMore() {
        std::cout << "BanquetStrictRule has no oneMore()";
        exit(1);
    }
};
struct BanquetRuleTogether;

class BanquetLenientRule : public BanquetRule {
    // friend BanquetInfo getPrice(Skill &skill, Recipe *recipe,
    //                             BanquetRuleTogether &r, bool verbose);

  public:
    BanquetLenientRule() = default;
    void oneMore() { this->duplicateTime += 1; }

  private:
    int duplicateTime = 0;
    friend BanquetRuleTogether;
    void execOneMore() {
        while (this->duplicateTime > 0) {
            this->add(*this);
            this->duplicateTime -= 1;
        }
    }

    void add(BanquetLenientRule &rule) {
        addRule.add(rule.addRule);
        baseRule.add(rule.baseRule);
    }
};
struct BanquetRuleTogether {
    BanquetLenientRule lenientRule;
    BanquetStrictRule
        strictRule; // 描述“下阶段blahblah”的规则，即不受“意图生效次数”的影响
    const BanquetRule &merge();
};
struct ActivityBuff {
    std::map<int, int> materialBuff;
    std::map<int, int> rarityBuff; // 几火就是几，不用减一
    std::map<FlavorEnum, int> flavorBuff;
};

BanquetInfo getPrice(const Skill &skill, Recipe *recipe, BanquetRuleTogether &r,
                     bool verbose = false);

#endif