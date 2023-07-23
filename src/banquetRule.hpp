#ifndef RULE_HPP
#define RULE_HPP
#include "Calculator.hpp"
#include "States.hpp"

int banquetRuleYU_GUI_REN(BanquetStrictRule **, BanquetLenientRule **, States &,
                          const Skill *, int);
int banquetRuleHU_XI_MEI(BanquetStrictRule **, BanquetLenientRule **, States &,
                         const Skill *, int);

/**
 * @brief
 *
 * @param rule
 * @param s
 * @param bestfull
 * @return void
 */
void banquetRule(const Skill *skills, BanquetRuleTogether *const &rule,
                 States &s, int *bestfull) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }

    // auto skills = s.getSkills();
    bestfull[0] = banquetRuleYU_GUI_REN(strictRule, lenientRule, s, skills, 0);
    bestfull[1] = banquetRuleHU_XI_MEI(strictRule, lenientRule, s, skills, 1);
    return;
}

int banquetRuleYU_GUI_REN(BanquetStrictRule **strictRule,
                          BanquetLenientRule **lenientRule, States &s,
                          const Skill *skill, int rank) {
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
        // 条件：鲜
        // 效果：本道料理饱腹感+2
        for (int i = d + 0; i < d + 3; i++) {
            if (s.recipe[i]->flavor == TASTY) {
                lenientRule[i]->addRule.full += 2;
                break;
            }
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
        if (s.recipe[i]->flavor == SOUR) {
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
        if (s.recipe[i]->flavor == SOUR) {
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
// 本程序由generateRule.py生成
// 风云宴 玉贵人2
int banquetRuleHU_XI_MEI(BanquetStrictRule **strictRule,
                         BanquetLenientRule **lenientRule, States &s,
                         const Skill *skill, int rank) {
    int d = rank * DISH_PER_CHEF * CHEFS_PER_GUEST;

    // 第1轮
    // 条件：三道炸
    // 效果：下两阶段煮技法料理饱腹感+1
    if (s.recipe[d + 0]->cookAbility.fry && s.recipe[d + 1]->cookAbility.fry &&
        s.recipe[d + 2]->cookAbility.fry) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.boil) {
                strictRule[i]->addRule.full += 1;
            }
        }
    }
    // 条件：三道烤
    // 效果：下两阶段蒸技法料理饱腹感+1
    if (s.recipe[d + 0]->cookAbility.bake &&
        s.recipe[d + 1]->cookAbility.bake &&
        s.recipe[d + 2]->cookAbility.bake) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.steam) {
                strictRule[i]->addRule.full += 1;
            }
        }
    }
    // 条件：三道炒
    // 效果：下两阶段切技法料理饱腹感+1
    if (s.recipe[d + 0]->cookAbility.stirfry &&
        s.recipe[d + 1]->cookAbility.stirfry &&
        s.recipe[d + 2]->cookAbility.stirfry) {
        for (int i = d + 3; i < d + 9; i++) {
            if (s.recipe[i]->cookAbility.knife) {
                strictRule[i]->addRule.full += 1;
            }
        }
    }
    // 条件：辣
    // 效果：本道料理售价+100%
    for (int i = d + 0; i < d + 3; i++) {
        if (s.recipe[i]->flavor == SPICY) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }

    // 第2轮
    // 条件：1火
    // 效果：本道料理售价+100%
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->rarity == 1) {
            lenientRule[i]->addRule.buff += 100;
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
    // 条件：第二道菜
    // 效果：第二道菜意图生效次数加一
    lenientRule[d + 4]->oneMore();
    // 条件：辣
    // 效果：本道料理基础售价+200
    for (int i = d + 3; i < d + 6; i++) {
        if (s.recipe[i]->flavor == SPICY) {
            lenientRule[i]->baseRule.directAdd += 200;
            break;
        }
    }

    // 第3轮
    // 条件：烤
    // 效果：下道料理为辣时饱腹感+3
    for (int i = d + 6; i < d + 8; i++) {
        if (s.recipe[i]->cookAbility.bake) {
            if (s.recipe[i + 1]->flavor == SPICY) {
                lenientRule[i + 1]->addRule.full += 3;
            }
            break;
        }
    }
    // 条件：3火
    // 效果：本道料理饱腹感+2
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->rarity == 3) {
            lenientRule[i]->addRule.full += 2;
            break;
        }
    }
    // 条件：炸
    // 效果：本道料理售价+100%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->cookAbility.fry) {
            lenientRule[i]->addRule.buff += 100;
            break;
        }
    }
    // 条件：辣
    // 效果：本道料理基础售价+50%
    for (int i = d + 6; i < d + 9; i++) {
        if (s.recipe[i]->flavor == SPICY) {
            lenientRule[i]->baseRule.buff += 50;
            break;
        }
    }
    return 44;
}

#endif