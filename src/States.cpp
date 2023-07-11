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
    }
#ifdef MEASURE_TIME
    clock_gettime(CLOCK_MONOTONIC, &end);
    getStatesSkillsTime +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return skillsCache;
};