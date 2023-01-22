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
    if (s.recipe[0]->cookAbility.steam > 0 &&
        s.recipe[1]->cookAbility.steam > 0 &&
        s.recipe[2]->cookAbility.steam > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].baseRule.skillBuff.abilityBuff.boil += 100;
        }
    }
    if (s.recipe[0]->cookAbility.bake > 0 &&
        s.recipe[1]->cookAbility.bake > 0 &&
        s.recipe[2]->cookAbility.bake > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].baseRule.skillBuff.abilityBuff.fry += 100;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->flavor.sweet == true) {
            rule[i].baseRule.buff += 3;
            break;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->rarity == 4) {
            rule[i].baseRule.buff += -100;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 1) {
            rule[i].baseRule.directAdd += 300;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->flavor.spicy == true) {
            rule[i].addRule.skillBuff.flavorBuff.spicy += 100;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 4) {
            rule[i].addRule.buff += -100;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 5) {
            rule[i].addRule.buff += -100;
            break;
        }
    }
    for (int i = 6; i < 8; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            rule[i + 1].baseRule.skillBuff.flavorBuff.spicy += 30;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.buff += 100;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.bake > 0) {
            rule[i].baseRule.skillBuff.abilityBuff.bake += -50;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.fry > 0) {
            rule[i].addRule.buff += -100;
            break;
        }
    }
    return 23;
}

#endif