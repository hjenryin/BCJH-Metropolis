#include "Types.hpp"
#include "Recipe.hpp"

int GradeBuffCondition::test(Skill *s, Recipe **r) {
    int count = 0;
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        count += (s->ability / r[i]->cookAbility >= grade);
    }
    return count;
}

int ThreeSameCookAbilityBuffCondition::test(Skill *s, Recipe **r) {
    Ability tmp(1,1,1,1,1,1);
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        tmp = tmp && r[i]->cookAbility;
    }
    return (tmp != 0);
}