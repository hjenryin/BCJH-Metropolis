#ifndef STATES_HPP
#define STATES_HPP

#include "Chef.hpp"
#include "Recipe.hpp"
#include "../config.hpp"

class States {
    bool cacheValid = false;
    Skill skillsCache[NUM_CHEFS];
    Chef *chefs[NUM_CHEFS];
    ToolEnum toolCKPT[NUM_CHEFS];

  public:
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS] = {0};
    Skill *getSkills();
    const Chef *const getConstChef(int i) { return chefs[i]; }
    Chef *getChef(int i) {
        cacheValid = false;
        return chefs[i];
    }
    Chef **getChefs() {
        cacheValid = false;
        return chefs;
    }
    void setChef(int i, Chef *c) {
        cacheValid = false;
        chefs[i] = c;
    }

    bool repeatChef(Chef *chef, int except) {
        for (int i = 0; i < NUM_CHEFS; i++) {
            if (except != i && chef->id == chefs[i]->id) {
                return true;
            }
        }
        return false;
    }
    /**
     * @brief When you keep some states and want to explore other states and
     * decide later which state to use, you need to save a snapshot of the
     * states.
     */
    void saveChefTool() {
        cacheValid = false;
        for (int i = 0; i < NUM_CHEFS; i++) {
            toolCKPT[i] = chefs[i]->getTool();
        }
    }
    void loadChefTool() {
        cacheValid = false;
        for (int i = 0; i < NUM_CHEFS; i++) {
            chefs[i]->modifyTool(toolCKPT[i]);
        }
    }
};

#endif