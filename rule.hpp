// 兔年年兽第七关


// 本代码由 ./src/generateRule.py 自动生成
#ifndef RULE_HPP
#define RULE_HPP
#include "src/Calculator.hpp"

/**
 * @brief
 *
 * @param rule
 * @param s
 * @return int : the optimal `full` value
 */
int banquetRule(BanquetRule *const &rule, States &s) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }

    // 第1轮

    // 条件：三道切：下阶段烤技法料理售价+100%
    if (s.recipe[0]->cookAbility.knife > 0 &&
        s.recipe[1]->cookAbility.knife > 0 &&
        s.recipe[2]->cookAbility.knife > 0) {
        // 效果：三道切：下阶段烤技法料理售价+100%

        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }

    // 条件：三道炒：下阶段煮技法料理饱腹感-1
    if (s.recipe[0]->cookAbility.stirfry > 0 &&
        s.recipe[1]->cookAbility.stirfry > 0 &&
        s.recipe[2]->cookAbility.stirfry > 0) {
        // 效果：三道炒：下阶段煮技法料理饱腹感-1
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.boil > 0) {
                strictRule[i]->addRule.full += -1;
            }
        }
    }
    // 第2轮
    // 条件：1火：本道料理基础售价+50%
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 1) {
            // 效果：1火：本道料理基础售价+50%
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：第一道菜：第一道菜意图生效次数加一
    for (int i = 3; i < 6; i++) {
        if (i == 3) {
            // 效果：第一道菜：第一道菜意图生效次数加一
            lenientRule[i]->oneMore();
            break;
        }
    }
    // 第3轮
    // 条件：切：本道料理售价+100%
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            // 效果：切：本道料理售价+100%
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：炒：本道料理基础售价+500
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            // 效果：炒：本道料理基础售价+500
            lenientRule[i]->baseRule.directAdd += 500;

            break;
        }
    }
    return 12;
}

#endif
