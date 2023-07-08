#ifndef TYPES_HPP
#define TYPES_HPP
#include "Chef.hpp"
#include "Recipe.hpp"
#include "Values.hpp"
#include "../config.hpp"
struct States {
    Chef *chef[NUM_CHEFS];
    ToolEnum toolCKPT[NUM_CHEFS];
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS];
    void getSkills(Skill *skills) {
        for (int i = 0; i < NUM_CHEFS; i++) {
            skills[i] = chef[i]->skill;
        }
        for (int g = 0; g < NUM_GUESTS; g++) {
            for (int i = g * CHEFS_PER_GUEST; i < (g + 1) * CHEFS_PER_GUEST;
                 i++) {
                for (int j = i; j < (g + 1) * CHEFS_PER_GUEST; j++) {
                    skills[j].add(chef[i]->companyBuff);
                }
            }
            for (int i = g * CHEFS_PER_GUEST + 1; i < (g + 1) * CHEFS_PER_GUEST;
                 i++) {
                skills[i].add(chef[i - 1]->nextBuff);
            }
        }
    };
};

#endif