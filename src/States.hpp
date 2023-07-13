#ifndef STATES_HPP
#define STATES_HPP

#include "Chef.hpp"
#include "Recipe.hpp"
#include "../config.hpp"

class States {
    Skill skillsCache[NUM_CHEFS];
    bool cacheValid = false;
    Chef chefs[NUM_CHEFS];

  public:
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS] = {0};
    Skill *getSkills();
    const Chef getChef(int i) const { return chefs[i]; }
    const Chef *const operator[](int i) const { return &chefs[i]; }
    void setChef(int i, const Chef &chef) {
        chefs[i] = chef;
        cacheValid = false;
    }
    void modifyTool(int i, ToolEnum tool) {
        chefs[i].modifyTool(tool);
        cacheValid = false;
    }
    ToolEnum getTool(int i) { return chefs[i].getTool(); }
    void appendName(int i, const std::string &s) { *chefs[i].name += s; }
    bool repeatedChef(const Chef *const chef, int except) const {
        for (int i = 0; i < NUM_CHEFS; i++) {
            if (except != i && chef->id == chefs[i].id) {
                return true;
            }
        }
        return false;
    }
    bool repeatedRecipe(Recipe *recipe, int except) {
        for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
            if (except != i && recipe == this->recipe[i]) {
                return true;
            }
        }
        return false;
    }
    bool capable() {
        auto skills = this->getSkills();
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