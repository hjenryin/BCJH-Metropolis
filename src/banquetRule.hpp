#ifndef RULE_HPP
#define RULE_HPP
#include "Calculator.hpp"
#include "functions.hpp"

//风云宴 蓝采和
int banquetRule0(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 吕洞宾
int banquetRule1(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 韩湘子
int banquetRule2(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 玉贵人
int banquetRule3(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 苏妲己
int banquetRule4(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 何仙姑
int banquetRule5(BanquetStrictRule **, BanquetLenientRule **, States &, int);
//风云宴 铁拐李
int banquetRule6(BanquetStrictRule **, BanquetLenientRule **, States &, int);


/**
 * @brief
 *
 * @param rule
 * @param s
 * @param bestfull
 * @return void
 */
void banquetRule(BanquetRule *const &rule, States &s, int *bestfull) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS* DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }

    
    bestfull[0] = banquetRule0(strictRule, lenientRule, s, 0);
    bestfull[1] = banquetRule5(strictRule, lenientRule, s, 1);
    bestfull[2] = banquetRule6(strictRule, lenientRule, s, 2);
    return;
}

//风云宴 蓝采和
int banquetRule0(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道切：下阶段炒技法料理基础售价+50%
    if (s.recipe[d + 0]->cookAbility.knife > 0 &&
        s.recipe[d + 1]->cookAbility.knife > 0 &&
        s.recipe[d + 2]->cookAbility.knife > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.stirfry > 0) {
                strictRule[i]->baseRule.buff += 50;
            }
        }
    }
    // 条件：三道蒸：下阶段烤技法料理基础售价+50%
    if (s.recipe[d + 0]->cookAbility.steam > 0 &&
        s.recipe[d + 1]->cookAbility.steam > 0 &&
        s.recipe[d + 2]->cookAbility.steam > 0) {
        for (int i = d + 3; i < d + 6 ; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->baseRule.buff += 50;
            }
        }
    }
    // 条件：三道煮：下阶段炸技法料理基础售价+50%
    if (s.recipe[d+0]->cookAbility.boil > 0 &&
        s.recipe[d+1]->cookAbility.boil > 0 &&
        s.recipe[d+2]->cookAbility.boil > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.fry > 0) {
                strictRule[i]->baseRule.buff += 50;
            }
        }
    }
    // 条件：甜：本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.sweet) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }


    // 第2轮
    // 条件：传：本道料理基础售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 5) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：第二道菜：第二道菜饱食度-3
    lenientRule[d + 4]->addRule.full += -3;
    // 条件：优：下道料理意图生效次数+1
    for (int i = d + 3; i < d + 5; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 2) {
            lenientRule[i + 1]->oneMore();
            break;
        }
    }
    // 条件：神：本道料理售价+100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 4) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }


    // 第3轮
    // 条件：苦：本道料理饱食度-4
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor.bitter) {
            lenientRule[i]->addRule.full += -4;
            break;
        }
    }
    // 条件：1火：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：甜：本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor.sweet) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：传：本道料理意图生效+1
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 5) {
            lenientRule[i]->oneMore();
            break;
        }
    }
    return 12;
}

//风云宴 吕洞宾
int banquetRule1(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道炸：下阶段烤技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.fry > 0 &&
        s.recipe[d + 1]->cookAbility.fry > 0 &&
        s.recipe[d + 2]->cookAbility.fry > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：2火：本道料理基础售价+50%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->rarity == 2) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：第三道菜：第三道菜意图生效次数加一
    lenientRule[d+2]->oneMore();
    // 条件：3火：本道料理售价+100%
    for (int i = d+0; i < d+3; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }


    // 第2轮
    // 条件：3火：下道料理为辣时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->rarity == 3) {
            if (s.recipe[i+1]->flavor.spicy){
                lenientRule[i+1]->addRule.buff += 100;
            }            
            break;
        }
    }
    // 条件：辣：下道料理为酸时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->flavor.spicy) {
            if (s.recipe[i+1]->flavor.sour){
                lenientRule[i+1]->addRule.buff += 100;
            }            
            break;
        }
    }
    // 条件：酸：本道料理基础售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.sour) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：切：本道料理饱腹感-2
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            lenientRule[i]->addRule.full += -2;
            break;
        }
    }


    // 第3轮
    // 条件：第一道菜：第一道菜饱腹感为2
    lenientRule[d + 6]->addRule.full += 2;
    // 条件：5火：本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：特：本道料理意图生效次数加一
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 3) {
            lenientRule[i]->oneMore();
            break;
        }
    }
    // 条件：神：本道料理售价-150%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 4) {
            lenientRule[i]->baseRule.buff += -150;
            break;
        }
    }
    return 33;
}

//风云宴 韩湘子
int banquetRule2(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道炒：下两阶段切技法料理饱腹感-1
    if (s.recipe[d + 0]->cookAbility.stirfry > 0 &&
        s.recipe[d + 1]->cookAbility.stirfry > 0 &&
        s.recipe[d + 2]->cookAbility.stirfry > 0) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.knife > 0) {
                strictRule[i]->addRule.full += -1;
            }
        }
    }
    // 条件：三道蒸：下两阶段烤技法料理饱腹感-1
    if (s.recipe[d + 0]->cookAbility.steam > 0 &&
        s.recipe[d + 1]->cookAbility.steam > 0 &&
        s.recipe[d + 2]->cookAbility.steam > 0) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.bake > 0) {
                strictRule[i]->addRule.full += -1;
            }
        }
    }
    // 条件：三道煮：下两阶段炸技法料理饱腹感-1
    if (s.recipe[d + 0]->cookAbility.boil > 0 &&
        s.recipe[d + 1]->cookAbility.boil > 0 &&
        s.recipe[d + 2]->cookAbility.boil > 0) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.fry > 0) {
                strictRule[i]->addRule.full += -1;
            }
        }
    }
    // 条件：3火：本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 第2轮
    // 条件：苦：本道料理售价+100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.bitter) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：辣：本道料理意图生效次数加一
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.spicy) {
            lenientRule[i]->oneMore();
            break;
        }
    }
    // 条件：第二道菜：第二道菜基础售价+200
    lenientRule[d + 4]->baseRule.directAdd += 200;
    // 条件：2火：本道料理售价+100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 2) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 第3轮
    // 条件：优：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 2) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：特：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 3) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    // 条件：神：本道料理饱腹感+3
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 4) {
            lenientRule[i]->addRule.full += 3;
            break;
        }
    }
    // 条件：传：本道料理饱腹感+4
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 5) {
            lenientRule[i]->addRule.full += 4;
            break;
        }
    }
    return 15;
}

//风云宴 玉贵人
int banquetRule3(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道炒：下阶段切技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.stirfry > 0 &&
        s.recipe[d + 1]->cookAbility.stirfry > 0 &&
        s.recipe[d + 2]->cookAbility.stirfry > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.knife > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道烤：下阶段煮技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.bake > 0 &&
        s.recipe[d + 1]->cookAbility.bake > 0 &&
        s.recipe[d + 2]->cookAbility.bake > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.boil > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道炸：下阶段蒸技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.fry > 0 &&
        s.recipe[d + 1]->cookAbility.fry > 0 &&
        s.recipe[d + 2]->cookAbility.fry > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.steam > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：酸味: 本道料理基础售价+50%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.sour) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }   


    // 第2轮
    // 条件：五火：本道料理售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->addRule.buff += 50;
            break;
        }
    }
    // 条件：酸味: 饱腹感-4
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.sour) {
            lenientRule[i]->addRule.full += -4;
            break;
        }
    }
    // 条件：五火：本道料理意图+1
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->oneMore();
            break;
        }
    }
    // 条件：四火：下道料理意图+1
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->rarity == 4) {
            lenientRule[i + 1]->oneMore();
            break;
        }
    }

    
    // 第3轮
    // 条件：蒸：本道料理售价-150%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.steam > 0) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    // 条件：煮：本道料理售价-150%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.boil > 0) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    // 条件：切：本道料理饱腹感-3
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.knife > 0) {
            lenientRule[i]->addRule.full += -3;
            break;
        }
    }
    // 条件：一火：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    return 19;
}

//风云宴 苏妲己
int banquetRule4(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道切：下阶段切技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.knife > 0 &&
        s.recipe[d + 1]->cookAbility.knife > 0 &&
        s.recipe[d + 2]->cookAbility.knife > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.knife > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道煮：下阶段煮技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.boil > 0 &&
        s.recipe[d + 1]->cookAbility.boil > 0 &&
        s.recipe[d + 2]->cookAbility.boil > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.boil > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道蒸：下阶段蒸技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.steam > 0 &&
        s.recipe[d + 1]->cookAbility.steam > 0 &&
        s.recipe[d + 2]->cookAbility.steam > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.steam > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：苦味料理: 本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.bitter) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    
    // 第2轮
    // 条件：甜味料理: 饱腹感+3
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.sweet) {
            lenientRule[i]->addRule.full += 3;
            break;
        }
    }
    // 条件：苦味料理：本道料理基础售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.bitter) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：第三道菜：第三道菜基础售价+200
    lenientRule[d + 5]->baseRule.directAdd += 200;
    // 条件：五火：本道料理售价-100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->addRule.buff += -100;
            break;
        }
    }
    // 第3轮
    // 条件：苦味料理: 本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor.bitter) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：三火：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    // 条件：炒：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry > 0) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：第二道菜：第二道菜意图生效次数+1
    lenientRule[d + 7]->oneMore();
    return 32;
}

//风云宴 何仙姑
int banquetRule5(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道炒：下阶段蒸技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.stirfry > 0 &&
        s.recipe[d + 1]->cookAbility.stirfry > 0 &&
        s.recipe[d + 2]->cookAbility.stirfry > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.steam > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：酸: 本道料理饱腹感-2
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.sour) {
            lenientRule[i]->addRule.full += -2;
            break;
        }
    }
    // 条件：咸: 本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.salty) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：鲜: 本道料理基础售价+50%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.tasty) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    
    // 第2轮
    // 条件：二火：下道料理为蒸时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->rarity == 2) {
            if (s.recipe[i + 1]->cookAbility.steam) {
                lenientRule[i + 1]->addRule.buff += 100;
            }
            break;
        }
    }
    // 条件：切：下道料理为蒸时基础售价+200
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->cookAbility.knife) {
            if (s.recipe[i + 1]->cookAbility.steam) {
                lenientRule[i + 1]->baseRule.directAdd += 200;
            }
            break;
        }
    }
    // 条件：五火：本道料理饱腹感-2
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i + 1]->addRule.full += -2;
            break;
        }
    }
    // 条件：第三道菜：第三道菜售价+100%
    lenientRule[d + 5]->addRule.buff += 100;
    
    // 第3轮
    // 条件：第二道菜：第二道菜意图生效次数+1
    lenientRule[d + 7]->oneMore();
    // 条件：五火：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：优：下道料理意图生效次数+1
    for (int i = d + 6; i < d + 8; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 2) {
            lenientRule[i + 1]->oneMore();
            break;
        }
    }
    // 条件：神：本道料理饱腹感-5
    for (int i = d + 6; i < d + 9; i++) {
        if (s.chef[i/3]->skill.ability / s.recipe[i]->cookAbility >= 4) {
            lenientRule[i]->addRule.full += -5;
            break;
        }
    }
    return 22;
}

//风云宴 铁拐李
int banquetRule6(BanquetStrictRule **strictRule, BanquetLenientRule **lenientRule, States &s, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;
    // 第1轮
    // 条件：三道烤：下阶段切技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.bake > 0 &&
        s.recipe[d + 1]->cookAbility.bake > 0 &&
        s.recipe[d + 2]->cookAbility.bake > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.knife > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：三道炸：下阶段煮技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.fry > 0 &&
        s.recipe[d + 1]->cookAbility.fry > 0 &&
        s.recipe[d + 2]->cookAbility.fry > 0) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.boil > 0) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }
    // 条件：咸: 本道料理售价-150%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.salty) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    // 条件：鲜: 本道料理售价-150%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor.tasty) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    
    // 第2轮
    // 条件：切：下道料理为辣时基础售价+50%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->cookAbility.knife) {
            if (s.recipe[i + 1]->flavor.spicy) {
                lenientRule[i + 1]->baseRule.buff += 50;
            }
            break;
        }
    }
    // 条件：煮：下道料理为酸时基础售价+200
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->cookAbility.boil) {
            if (s.recipe[i + 1]->flavor.sour) {
                lenientRule[i + 1]->baseRule.directAdd += 200;
            }
            break;
        }
    }
    // 条件：辣：下道料理为煮时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->flavor.spicy) {
            if (s.recipe[i + 1]->cookAbility.boil) {
                lenientRule[i + 1]->addRule.buff += 100;
            }
            break;
        }
    }
    // 条件：酸：本道料理售价+100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor.sour) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    
    // 第3轮    
    // 条件：鲜：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor.tasty) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    } 
    // 条件：咸：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor.salty) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    // 条件：烤：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.bake) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：炸：本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.fry) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    return 30;
}

#endif