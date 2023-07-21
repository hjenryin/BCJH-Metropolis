#include "States.hpp"
#include <math.h>
double getStatesSkillsTime = 0;

template <typename T> inline void copy(T *dst, const T *src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

void applyCookAbilities(Skill *skillsResult, Skill *selfSkills,
                        Skill *companyBuffs, Skill *nextBuffs, Chef *chefs) {
    for (int i = 0; i < NUM_CHEFS; i++) {
        skillsResult[i] = selfSkills[i];
        skillsResult[i].ability.add(Ability(chefs[i].getTool()));
    }
    // 光环
    for (int g = 0; g < NUM_GUESTS; g++) {
        for (int i = g * CHEFS_PER_GUEST; i < (g + 1) * CHEFS_PER_GUEST; i++) {
            for (int j = i; j < (g + 1) * CHEFS_PER_GUEST; j++) {
                skillsResult[j] += companyBuffs[i];
            }
        }
        for (int i = g * CHEFS_PER_GUEST + 1; i < (g + 1) * CHEFS_PER_GUEST;
             i++) {
            skillsResult[i] += nextBuffs[i - 1];
        }
    }
    // 技法先加减后乘除
    for (int i = 0; i < NUM_CHEFS; i++) {
        CookAbility &a = skillsResult[i].cookAbilityPercentBuff;
        a.bake = int(ceil(a.bake * (a.bake + 100) / 100.0));
        a.boil = int(ceil(a.boil * (a.boil + 100) / 100.0));
        a.steam = int(ceil(a.steam * (a.steam + 100) / 100.0));
        a.stirfry = int(ceil(a.stirfry * (a.stirfry + 100) / 100.0));
        a.knife = int(ceil(a.knife * (a.knife + 100) / 100.0));
        a.fry = int(ceil(a.fry * (a.fry + 100) / 100.0));
    }
}
void applyConditionBuff(Skill *cookAbilitySkill,
                        const std::vector<ConditionalBuff *> &conditionalEffects,
                        Skill *skillTarget, Recipe **recipe) {
    // 条件技能

    for (auto &ce : conditionalEffects) {
        for (int i = ce->conditionFunc->test(cookAbilitySkill, recipe); i > 0;
             i--) {
            *skillTarget += ce->skill;
        }
    }
}
Skill *States::getCookAbilities() {
    Skill selfSkills[NUM_CHEFS];
    Skill companySkills[NUM_CHEFS];
    Skill nextSkills[NUM_CHEFS];
    for (int i = 0; i < NUM_CHEFS; i++) {
        selfSkills[i] = *chefs[i].skill;
        companySkills[i] = *chefs[i].companyBuff;
        nextSkills[i] = *chefs[i].nextBuff;
    }
    cacheValid = true;

    applyCookAbilities(skillsCache, selfSkills, companySkills, nextSkills,
                       chefs);
    return skillsCache;
}

Skill *States::getSkills() {
#ifdef MEASURE_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif
    Skill selfSkills[NUM_CHEFS];
    Skill companySkills[NUM_CHEFS];
    Skill nextSkills[NUM_CHEFS];
    for (int i = 0; i < NUM_CHEFS; i++) {
        selfSkills[i] = *chefs[i].skill;
        companySkills[i] = *chefs[i].companyBuff;
        nextSkills[i] = *chefs[i].nextBuff;
    }
    auto skillsPreview = getCookAbilities();
    for (int i = 0; i < NUM_CHEFS; i++) {
        applyConditionBuff(skillsPreview+i, chefs[i].skill->conditionalEffects,
                           selfSkills + i, recipe + i * DISH_PER_CHEF);
        applyConditionBuff(skillsPreview+i,
                           chefs[i].companyBuff->conditionalEffects,
                           companySkills + i, recipe + i * DISH_PER_CHEF);
        applyConditionBuff(skillsPreview+i, chefs[i].nextBuff->conditionalEffects,
                           nextSkills + i, recipe + i * DISH_PER_CHEF);
    }
    applyCookAbilities(skillsCache, selfSkills, companySkills, nextSkills,
                           chefs);

#ifdef MEASURE_TIME
    clock_gettime(CLOCK_MONOTONIC, &end);
    getStatesSkillsTime +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return skillsCache;
};