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
    if (s.recipe[0]->cookAbility.fry > 0 && s.recipe[1]->cookAbility.fry > 0 &&
        s.recipe[2]->cookAbility.fry > 0) {
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->addRule.full += 1;
            }
        }
    }
    if (s.recipe[0]->cookAbility.knife > 0 &&
        s.recipe[1]->cookAbility.knife > 0 &&
        s.recipe[2]->cookAbility.knife > 0) {
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.stirfry > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->flavor.spicy == true) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    lenientRule[2]->oneMore();
    ////////
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 2) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->addRule.full += 3;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            lenientRule[i]->baseRule.directAdd += 500;
            break;
        }
    }
    for (int i = 3; i < 5; i++) {
        if (s.recipe[i]->rarity == 4) {
            lenientRule[i + 1]->oneMore();
            break;
        }
    }
    ////////
    for (int i = 6; i < 8; i++) {
        if (s.recipe[i]->cookAbility.steam > 0) {
            if (s.recipe[i + 1]->flavor.tasty == true) {
                lenientRule[i + 1]->addRule.full += 8;
            }
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            lenientRule[i]->addRule.buff += 150;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 4) {
            lenientRule[i]->addRule.buff += 50;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->baseRule.directAdd += 1000;
            break;
        }
    }
    return 46;
}

#endif