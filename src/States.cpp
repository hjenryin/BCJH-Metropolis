#include "States.hpp"
#include <math.h>
double getStatesSkillsTime = 0;

Skill *States::getSkills() {
#ifdef MEASURE_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif

    if (!cacheValid) {
        cacheValid = true;
        for (int i = 0; i < NUM_CHEFS; i++) {
            skillsCache[i] = chefs[i]->skill;
        }
        //光环
        for (int g = 0; g < NUM_GUESTS; g++) {
            for (int i = g * CHEFS_PER_GUEST; i < (g + 1) * CHEFS_PER_GUEST;
                 i++) {
                for (int j = i; j < (g + 1) * CHEFS_PER_GUEST; j++) {
                    skillsCache[j] += chefs[i]->companyBuff;
                }
            }
            for (int i = g * CHEFS_PER_GUEST + 1; i < (g + 1) * CHEFS_PER_GUEST;
                 i++) {
                skillsCache[i] += chefs[i - 1]->nextBuff;
            }
        }
        //技法先加减后乘除
        for (int i = 0; i < NUM_CHEFS; i++) {
            CookAbility &a = skillsCache[i].ability;
            if (a.percent.bake)
                a.bake = int(ceil(a.bake * (a.percent.bake + 100) / 100.0));
            if (a.percent.boil)
                a.boil = int(ceil(a.boil * (a.percent.boil + 100) / 100.0));
            if (a.percent.steam)
                a.steam = int(ceil(a.steam * (a.percent.steam + 100) / 100.0));
            if (a.percent.stirfry)
                a.stirfry = int(ceil(a.stirfry * (a.percent.stirfry + 100) / 100.0));
            if (a.percent.knife)
                a.knife = int(ceil(a.knife * (a.percent.knife + 100) / 100.0));
            if (a.percent.fry)
                a.fry = int(ceil(a.fry * (a.percent.fry + 100) / 100.0));
        }        
        //条件技能
        for (int g = 0; g < NUM_GUESTS; g++) {
            for (int i = g * CHEFS_PER_GUEST; i < (g + 1) * CHEFS_PER_GUEST;
                 i++) {
                int effect = 0;
                switch (chefs[i]->conditionSkill.condition.type)
                {
                case 1://每制作一种x级料理
                    for (int j = i * DISH_PER_CHEF; j < (i + 1) * DISH_PER_CHEF; j++){
                        int grade = skillsCache[i].ability / recipe[j]->cookAbility;
                        if (grade >= chefs[i]->conditionSkill.condition.value)
                            effect++;
                    }                    
                    break;
                case 2://制作三种同技法料理
                    int *ptr1 = &recipe[i * DISH_PER_CHEF]->cookAbility.stirfry;
                    int *ptr2 = &recipe[i * DISH_PER_CHEF + 1]->cookAbility.stirfry;
                    int *ptr3 = &recipe[i * DISH_PER_CHEF + 2]->cookAbility.stirfry;
                    for (int i = 0; i < 6; i++) 
                        if (ptr1[i] > 0 && ptr2[i] > 0 && ptr3[i] > 0)
                            effect++;
                    break;                
                default:
                    break;
                }
                if (effect > 0)
                    switch (chefs[i]->conditionSkill.type)
                    {
                    case Skill::SELF:
                        for (int k = 0; k < effect; k++)
                            skillsCache[i] += chefs[i]->conditionSkill;
                        break;
                    case Skill::PARTIAL:
                        for (int j = i; j < (g + 1) * CHEFS_PER_GUEST; j++) 
                            for (int k = 0; k < effect; k++)
                                skillsCache[j] += chefs[i]->conditionSkill;
                        break;
                    case Skill::NEXT:
                        if ((i + 1) % CHEFS_PER_GUEST)                        
                            for (int k = 0; k < effect; k++)
                                skillsCache[i+1] += chefs[i]->conditionSkill;
                        break;                    
                    default:
                        break;
                    }
            }
        }
    }
#ifdef MEASURE_TIME
    clock_gettime(CLOCK_MONOTONIC, &end);
    getStatesSkillsTime +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return skillsCache;
};