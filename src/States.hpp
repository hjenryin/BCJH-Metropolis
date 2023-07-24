#ifndef STATES_HPP
#define STATES_HPP

#include "Chef.hpp"
#include "Recipe.hpp"
#include "../config.hpp"

class States {
    bool cookAbilitiesValid = false;
    uint32_t chefHasStrangeSkills = 0; // Each bit represents a chef
    Skill cookAbilitiesCache[NUM_CHEFS];
    // Skill skillsCache[NUM_CHEFS];
    Chef chefs[NUM_CHEFS];

  public:
    enum FLAG_getCookAbilities { FORCE_UPDATE = -2, DEFAULT = -1 };
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS] = {0};
    void getSkills(Skill *skills, int chefIdForTool = DEFAULT,
                   int toolValue = 100);
    const Skill *getCookAbilities(int chefIdForTool = DEFAULT,
                                  int toolValue = 100);
    Chef getChef(int i) const { return chefs[i]; }
    const Chef *const getChefPtr(int i) const { return &chefs[i]; }
    void setChef(int i, const Chef &chef) {
        chefs[i] = chef;
        cookAbilitiesValid = false;
        size_t numStrangeSkills = chef.skill->conditionalEffects.size() +
                                  chef.companyBuff->conditionalEffects.size() +
                                  chef.nextBuff->conditionalEffects.size();
        chefHasStrangeSkills =
            (chefHasStrangeSkills & ~(1 << i)) | ((numStrangeSkills > 0) << i);
    }
    void modifyTool(int i, ToolEnum tool) {
        chefs[i].modifyTool(tool);
        cookAbilitiesValid = false;
    }
    ToolEnum getTool(int i) { return chefs[i].getTool(); }
    void appendName(int i, const std::string &s) { *chefs[i].name += s; }
    bool repeatedChef(const Chef *const chef = NULL, int except = -1) const {
        if (chef != NULL) {
            for (int i = 0; i < NUM_CHEFS; i++) {
                if (except != i && chef->id == chefs[i].id) {
                    return true;
                }
            }
            return false;
        } else {
            for (int i = 0; i < NUM_CHEFS; i++) {
                for (int j = i + 1; j < NUM_CHEFS; j++) {
                    if (chefs[i].id == chefs[j].id) {
                        return true;
                    }
                }
            }
            return false;
        }
    }
    bool repeatedRecipe(Recipe *recipe = NULL, int except = -1) {
        if (recipe == NULL) {
            for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
                for (int j = i + 1; j < NUM_CHEFS * DISH_PER_CHEF; j++) {
                    if (this->recipe[i] == this->recipe[j]) {
                        return true;
                    }
                }
            }
            return false;
        } else {
            for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
                if (except != i && recipe == this->recipe[i]) {
                    return true;
                }
            }
            return false;
        }
    }
    bool capable() {
        auto skills = this->getCookAbilities();
        for (int i = 0; i < NUM_DISHES; i++) {
            if (skills[i / DISH_PER_CHEF].ability / this->recipe[i]->cookAbility ==
                0) {
                return false;
            }
        }
        return true;
    }
};

#endif