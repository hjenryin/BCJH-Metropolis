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
    if (s.recipe[0]->cookAbility.fry > 0 && s.recipe[1]->cookAbility.fry > 0 &&
        s.recipe[2]->cookAbility.fry > 0) {
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                rule[i].addRule.full += 1;
            }
        }
    }
    if (s.recipe[0]->cookAbility.knife > 0 &&
        s.recipe[1]->cookAbility.knife > 0 &&
        s.recipe[2]->cookAbility.knife > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.stirfry += 100;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->flavor.spicy == true) {
            rule[i].addRule.buff += 100;
            break;
        }
    }
    rule[2].add(rule[2]);
    ////////
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 2) {
            rule[i].baseRule.buff += 50;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.full += 3;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            rule[i].baseRule.directAdd += 500;
            break;
        }
    }
    for (int i = 3; i < 5; i++) {
        if (s.recipe[i]->rarity == 4) {
            rule[i + 1].add(rule[i + 1]);
            break;
        }
    }
    ////////
    for (int i = 6; i < 8; i++) {
        if (s.recipe[i]->cookAbility.steam > 0 &&
            s.recipe[i + 1]->flavor.tasty == true) {
            rule[i].addRule.full += 8;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            rule[i].addRule.buff += 150;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 4) {
            rule[i].addRule.buff += 50;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 1) {
            rule[i].baseRule.directAdd += 1000;
            break;
        }
    }
    return 46;
}

#endif