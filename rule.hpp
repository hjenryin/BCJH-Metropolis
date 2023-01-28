// 兔年年兽第六关

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
    // 条件：三道切：下阶段炒技法料理售价+100%
    if (s.recipe[0]->cookAbility.knife > 0 &&
        s.recipe[1]->cookAbility.knife > 0 &&
        s.recipe[2]->cookAbility.knife > 0) {
        // 效果：三道切：下阶段炒技法料理售价+100%
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.stirfry > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道炸：下阶段烤技法料理饱腹感+1
    if (s.recipe[0]->cookAbility.fry > 0 && s.recipe[1]->cookAbility.fry > 0 &&
        s.recipe[2]->cookAbility.fry > 0) {
        // 效果：三道炸：下阶段烤技法料理饱腹感+1
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->addRule.full += 1;
            }
        }
    }
    // 第2轮
    // 条件：3火：本道料理饱腹感+3
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 3) {
            // 效果：3火：本道料理饱腹感+3
            lenientRule[i]->addRule.full += 3;
            break;
        }
    }
    // 条件：2火：本道料理基础售价+50%
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 2) {
            // 效果：2火：本道料理基础售价+50%
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 第3轮
    // 条件：炒：本道料理售价+150%
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            // 效果：炒：本道料理售价+150%
            lenientRule[i]->addRule.buff += 150;
            break;
        }
    }
    // 条件：蒸：下道料理为鲜时饱腹感+8
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.steam > 0) {
            // 效果：蒸：下道料理为鲜时饱腹感+8
            if (i + 1 < 9) {
                // 条件：鲜：本道料理饱腹感+8
                if (s.recipe[i + 1]->flavor.tasty == true) {
                    // 效果：鲜：本道料理饱腹感+8
                    lenientRule[i + 1]->addRule.full += 8;
                    break;
                }
            }
            break;
        }
    }
    return 46;
}

#endif
