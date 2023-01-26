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
    //20230126年兽大赛
    //Stage1 
    
    //炸*3 下阶段烤技法料理饱腹感+1
    if (s.recipe[0]->cookAbility.fry > 0 &&
        s.recipe[1]->cookAbility.fry > 0 &&
        s.recipe[2]->cookAbility.fry > 0) {
        for (int i = 3; i < 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                rule[i].addRule.full += 1;
            }
        }
    }
    //切*3 下阶段炒技法料理售价+100%
    if (s.recipe[0]->cookAbility.knife > 0 &&
        s.recipe[1]->cookAbility.knife > 0 &&
        s.recipe[2]->cookAbility.knife > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.stirfry += 100; 
        }
    }
    //辣本道料理售价+100%
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->flavor.spicy == true) {
            rule[i].addRule.buff += 100;
            break;
        }
    }
    //第三道菜意图生效次数加一
    rule[2].add(rule[2]); 


//Stage2
//2火本道料理基础售价+50%
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 2) {
            rule[i].addRule.buff += 50;
            break;
        }
    }
//3火本道料理饱腹感+3
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.full += 3;
            break;
        }
    }
//4火下道料理意图生效次数加一
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 4 && i < 5) {
            rule[i+1].add(rule[i+1]);
            break;
        }
    }
//切本道料理基础售价+500
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            rule[i].baseRule.directAdd = 500;
            break;
        }
    }
//Stage3
//蒸下道料理为鲜时饱腹感+8
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.steam > 0 && i < 8 && s.recipe[i+1]->flavor.tasty == true) {
                rule[i+1].addRule.full += 8;
            break;
        }
    }
//炒本道料理售价+150%
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0 ) {
            rule[i].addRule.buff += 150;
            break;
        }
    }
//4火本道料理售价+50%
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 4) {
            rule[i].addRule.buff += 50;
            break;
        }
    }
//1火本道料理基础售价+1000
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 1) {
            rule[i].baseRule.directAdd = 1000;
            break;
        }
    }

    return 46;
}
/*
int banquetRule(BanquetRule *const &rule, States &s) {
    if (s.recipe[0]->cookAbility.stirfry > 0 &&
        s.recipe[1]->cookAbility.stirfry > 0 &&
        s.recipe[2]->cookAbility.stirfry > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.knife += 100;
        }
    }
    if (s.recipe[0]->cookAbility.fry > 0 &&
        s.recipe[1]->cookAbility.fry > 0 &&
        s.recipe[2]->cookAbility.fry > 0) {
        for (int i = 3; i < 6; i++) {
            rule[i].addRule.skillBuff.abilityBuff.steam += 100;
        }
    }
    rule[2].addRule.buff = -30;
    for (int i = 0; i < 3; i++) {
        if (s.recipe[i]->flavor.salty == true) {
            rule[i].addRule.full -= 3;
            break;
        }
    }

    //////

    rule[3].addRule.fullAdd = false;
    rule[3].addRule.full = 6;

    rule[4].baseRule.directAdd = 500;

    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.full -= 5;
            break;
        }
    }
    for (int i = 3; i < 6; i++) {
        if (s.recipe[i]->flavor.bitter == true) {
            rule[i].addRule.buff -= 50;
            break;
        }
    }

    //////

    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            rule[i].addRule.full += 3;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            rule[i].baseRule.directAdd += 500;
            break;
        }
    }
    for (int i = 6; i < 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            rule[i].baseRule.buff -= 15;
            break;
        }
    }

    rule[8].add(rule[8]);
    return 46;
}
*/
#endif