#include "States.hpp"
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
    }
#ifdef MEASURE_TIME
    clock_gettime(CLOCK_MONOTONIC, &end);
    getStatesSkillsTime +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return skillsCache;
};