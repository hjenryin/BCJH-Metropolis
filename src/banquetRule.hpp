#ifndef RULE_HPP
#define RULE_HPP
#include "Calculator.hpp"
#include "States.hpp"
int banquetRuleDA_GENG_REN(BanquetStrictRule **, BanquetLenientRule **,
                           States &, Skill *, int);
int banquetRuleYU_GUI_REN(BanquetStrictRule **, BanquetLenientRule **, States &,
                          Skill *, int);

/**
 * @brief
 *
 * @param rule
 * @param s
 * @param bestfull
 * @return void
 */
void banquetRule(BanquetRuleTogether *const &rule, States &s, int *bestfull) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }

    auto skill = s.getSkills();
    bestfull[0] = banquetRuleDA_GENG_REN(strictRule, lenientRule, s, skill, 0);
    bestfull[1] = banquetRuleYU_GUI_REN(strictRule, lenientRule, s, skill, 1);
    return;
}

// 本程序由generateRule.py生成
// 风云宴 打更人
int banquetRuleDA_GENG_REN(BanquetStrictRule **strictRule,
                           BanquetLenientRule **lenientRule, States &s,
                           Skill *skill, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;

    // 第1轮
    // 条件：传
    // 效果：本道料理意图生效次数加一
    for (int i = d + 0; i < d + 3; i++) {
        if (skill[i / 3].ability / s.recipe[i]->cookAbility >= 5) {
            lenientRule[i]->oneMore();
            break;
        }
    }
    // 条件：蒸
    // 效果：本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->cookAbility.steam) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：鲜
    // 效果：本道料理饱腹感+2
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor == TASTY) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    // 条件：三道炒
    // 效果：下阶段烤技法料理售价+100%
    if (s.recipe[d + 0]->cookAbility.stirfry &&
        s.recipe[d + 1]->cookAbility.stirfry &&
        s.recipe[d + 2]->cookAbility.stirfry) {
        for (int i = d + 3; i < d + 6; i++) {
            if (s.recipe[i]->cookAbility.bake) {
                strictRule[i]->addRule.buff += 100;
            }
        }
    }

    // 第2轮
    // 条件：酸
    // 效果：本道料理基础售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor == SOUR) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：1火
    // 效果：本道料理基础售价+300
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->baseRule.directAdd += 300;
            break;
        }
    }
    // 条件：三道烤
    // 效果：下阶段炸技法料理售价+50%
    if (s.recipe[d + 3]->cookAbility.bake &&
        s.recipe[d + 4]->cookAbility.bake &&
        s.recipe[d + 5]->cookAbility.bake) {
        for (int i = d + 6; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.fry) {
                strictRule[i]->addRule.buff += 50;
            }
        }
    }
    // 条件：煮
    // 效果：本道料理饱腹感+2
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->cookAbility.boil) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }

    // 第3轮
    // 条件：炒
    // 效果：本道料理售价-150%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.stirfry) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    // 条件：煮
    // 效果：本道料理售价-150%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.boil) {
            lenientRule[i]->addRule.buff += -150;
            break;
        }
    }
    // 条件：神
    // 效果：本道料理基础售价+300
    for (int i = d + 6; i < d + 9; i++) {
        if (skill[i / 3].ability / s.recipe[i]->cookAbility >= 4) {
            lenientRule[i]->baseRule.directAdd += 300;
            break;
        }
    }
    // 条件：4火
    // 效果：本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 4) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    return 20;
}
// 本程序由generateRule.py生成
// 风云宴 玉贵人2
int banquetRuleYU_GUI_REN(BanquetStrictRule **strictRule,
                          BanquetLenientRule **lenientRule, States &s,
                          Skill *skill, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;

    // 第1轮
    // 条件：三道切
    // 效果：下两阶段煮技法料理饱腹感-2
    if (s.recipe[d + 0]->cookAbility.knife &&
        s.recipe[d + 1]->cookAbility.knife &&
        s.recipe[d + 2]->cookAbility.knife) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.boil) {
                strictRule[i]->addRule.full += -2;
            }
        }
    }
    // 条件：三道炸
    // 效果：下两阶段炒技法料理饱腹感-2
    if (s.recipe[d + 0]->cookAbility.fry && s.recipe[d + 1]->cookAbility.fry &&
        s.recipe[d + 2]->cookAbility.fry) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.stirfry) {
                strictRule[i]->addRule.full += -2;
            }
        }
    }
    // 条件：5火
    // 效果：本道料理饱腹感-3
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->rarity == 5) {
            lenientRule[i]->addRule.full += -3;
            break;
        }
    }
    // 条件：三道蒸
    // 效果：下两阶段切技法料理售价+50%
    if (s.recipe[d + 0]->cookAbility.steam &&
        s.recipe[d + 1]->cookAbility.steam &&
        s.recipe[d + 2]->cookAbility.steam) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.knife) {
                strictRule[i]->addRule.buff += 50;
            }
        }
    }

    // 第2轮
    // 条件：传
    // 效果：本道料理基础售价+50%
    for (int i = d + 3; i < d + 6; i++) {
        if (skill[i / 3].ability / s.recipe[i]->cookAbility >= 5) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：1火
    // 效果：本道料理饱腹感+5
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->addRule.full += 5;
            break;
        }
    }
    // 条件：鲜
    // 效果：下道料理为酸时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->flavor == TASTY) {
            if (s.recipe[i + 1]->flavor == SOUR) {
                lenientRule[i + 1]->addRule.buff += 100;
            }
            break;
        }
    }
    // 条件：辣
    // 效果：下道料理为甜时售价+100%
    for (int i = d + 3; i < d + 5; i++) {
        if (s.recipe[i]->flavor == SPICY) {
            if (s.recipe[i + 1]->flavor == SWEET) {
                lenientRule[i + 1]->addRule.buff += 100;
            }
            break;
        }
    }

    // 第3轮
    // 条件：4火
    // 效果：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 4) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：3火
    // 效果：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    // 条件：咸
    // 效果：本道料理饱腹感+3
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor == SALTY) {
            lenientRule[i]->addRule.full += 3;
            break;
        }
    }
    // 条件：烤
    // 效果：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.bake) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    return 13;
}
#endif