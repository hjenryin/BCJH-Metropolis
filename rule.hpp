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
    if (s.recipe[0]->cookAbility.stirfry > 0 &&
        s.recipe[1]->cookAbility.stirfry > 0 &&
        s.recipe[2]->cookAbility.stirfry > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.knife += 100;
        }
    }
    if (s.recipe[0]->cookAbility.boil > 0 &&
        s.recipe[1]->cookAbility.boil > 0 &&
        s.recipe[2]->cookAbility.boil > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.steam += 100;
        }
    }
    rule[3].addRule.fullAdd = false;
    rule[3].addRule.full = 6;
    rule[4].baseRule.directAdd = 500;

    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.full += 3;
            break;
        }
    }
    rule[8].add(rule[8]);
    return 46;
}

#endif