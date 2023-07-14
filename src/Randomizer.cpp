#include "Randomizer.hpp"
#include "functions.hpp"
#include "SARunner.hpp"
const double bestToolProb = 0.9;
inline bool debugIntegrity(States &s) {
#ifndef DEBUG_INTEGRITY
    return true;
#else
    bool result = true;
    if (!s.capable()) {
        std::cout << "NOT CAPABLE" << std::endl;
        result = false;
    }
    if (s.repeatedRecipe()) {
        std::cout << "REPEATED RECIPE" << std::endl;
        result = false;
    }
    if (s.repeatedChef()) {
        std::cout << "REPEATED CHEF" << std::endl;
        result = false;
    }
    if (result == false) {
        return result;
    }
    return result;
#endif
}
ToolEnum toolHeuristic(States &s, int chefId) {
    auto chef = s[chefId];
    Recipe **recipes = &s.recipe[chefId * DISH_PER_CHEF];
    if (chef->getTool() == NO_TOOL)
        return NO_TOOL;
    ToolEnum best = s.getTool(chefId);
    s.modifyTool(chefId, NOT_EQUIPPED);
    int max = 0;
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        max += (chef->skill->ability + best) / recipes[i]->cookAbility;
    }
    for (int t = ABILITY_ENUM_START; t < ABILITY_ENUM_END; t++) {
        auto tool = (ToolEnum)t;
        s.modifyTool(chefId, tool);
        if (!s.capable()) {
            continue;
        }
        int value = 0;
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            auto skill = (chef->skill->ability + tool);
            auto thisgrade = skill / recipes[i]->cookAbility;
            value += thisgrade;
        }
        if (value > max) {
            max = value;
            best = tool;
        }
    }
    return best;
}

bool ChefRandomizer::randomChef(States &s) const {
    debugIntegrity(s);
    auto &chefList = this->c;
    int chefNum = rand() % NUM_CHEFS;
    Chef pChef = s.getChef(chefNum);
    int count = 0;
    auto learned = pChef.recipeLearned;
    learned->clear();
    int dishNum = chefNum * DISH_PER_CHEF;
    int totalDishNum = NUM_CHEFS * DISH_PER_CHEF;
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        learned->push_back(s.recipe[dishNum + i]);
    }
    do {
        pChef = chefList->at(rand() % chefList->size());
        count++;
    } while (s.repeatedChef(&pChef, -1) && count < RANDOM_SEARCH_TIMEOUT);
    if (count >= RANDOM_SEARCH_TIMEOUT) {
        throw NoChefException();
    }
    s.setChef(chefNum, pChef);
    if (pChef.recipeLearned->size() == DISH_PER_CHEF) {
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            s.recipe[dishNum + i] = (*pChef.recipeLearned)[i];
        }
    }
    bool changed = true;
    auto oldS = s;
    Skill *skills = s.getSkills();
    int i = dishNum;
    do {
        auto skill = skills[i / DISH_PER_CHEF];
        if ((skill.ability / s.recipe[i]->cookAbility == 0) ||
            inArray(s.recipe, NUM_CHEFS * DISH_PER_CHEF, s.recipe[i])) {
            bool thisChanged = this->unrepeatedRandomRecipe(
                skill, s.recipe, totalDishNum, i,
                RANDOM_SEARCH_TIMEOUT * RANDOM_SEARCH_TIMEOUT);
            changed = changed && thisChanged;
        }
        i++;
    } while (i % (DISH_PER_CHEF * CHEFS_PER_GUEST) != 0);
    if (changed) {
        debugIntegrity(s);
        return true;
    } else {
        s = oldS;
        debugIntegrity(s);
        return false;
    }
}
bool Randomizer::swapRecipe(States &s) const {
    double random = rand() / RAND_MAX;
    bool toolChanged = false;
    for (int i = 1; i < RANDOM_SEARCH_TIMEOUT; i++) {
        int recipeNum1 = rand() % (NUM_CHEFS * DISH_PER_CHEF);
        int recipeNum2 = rand() % (NUM_CHEFS * DISH_PER_CHEF);
        int chefNum1 = recipeNum1 / DISH_PER_CHEF;
        int chefNum2 = recipeNum2 / DISH_PER_CHEF;
        const Chef *chef1 = s[chefNum1];
        const Chef *chef2 = s[chefNum2];
        if (!toolChanged && random < bestToolProb) {
            toolChanged = true;
            s.modifyTool(chefNum1, toolHeuristic(s, chefNum1));
        }
        if (chef1 == chef2) {
            swap(s.recipe[recipeNum1], s.recipe[recipeNum2]);
            return true;
        } else {
            bool chef1CanCook = s.getSkills()[chefNum1].ability /
                                    s.recipe[recipeNum2]->cookAbility >
                                0;
            bool chef2CanCook = s.getSkills()[chefNum2].ability /
                                    s.recipe[recipeNum1]->cookAbility >
                                0;
            if (chef1CanCook && chef2CanCook) {
                swap(s.recipe[recipeNum1], s.recipe[recipeNum2]);
                return true;
            }
        }
    }
    return false;
}

bool RecipeRandomizer::randomRecipe(States &s) const {
    for (int tries = 0; tries < RANDOM_SEARCH_TIMEOUT; tries++) {
        int recipeNum = rand() % (NUM_CHEFS * DISH_PER_CHEF);
        Skill &skill = s.getSkills()[recipeNum / DISH_PER_CHEF];
        bool changed = this->unrepeatedRandomRecipe(
            skill, s.recipe, NUM_CHEFS * DISH_PER_CHEF, recipeNum);
        if (changed) {
            return true;
        }
    }
    return false;
}

bool ChefRandomizer::swapChefTool(States &s) const {
    States saveS = s;
    int i;
    for (i = 0; i < RANDOM_SEARCH_TIMEOUT; i++) {
        s = saveS;
        int chefNum = rand() % NUM_CHEFS;
        int orig_tool = s.getTool(chefNum);
        int tool;
        do {
            tool = rand() % 6 + ABILITY_ENUM_START;
        } while (tool == orig_tool);
        s.modifyTool(chefNum, (ToolEnum)tool);
        auto &skill = s.getSkills()[chefNum];
        auto &ability = skill.ability;
        for (int i = chefNum * DISH_PER_CHEF;
             i < chefNum * DISH_PER_CHEF + DISH_PER_CHEF; i++) {
            if (ability / s.recipe[i]->cookAbility == 0) {
                bool thisChanged = this->unrepeatedRandomRecipe(
                    skill, s.recipe, NUM_CHEFS * DISH_PER_CHEF, i);
                if (!thisChanged) {
                    break;
                }
            }
        }
    }
    if (i >= RANDOM_SEARCH_TIMEOUT) {
        s = saveS;
        return false;
    } else {
        return true;
    }
}
States RecipeRandomizer::operator()(States s) {
#ifdef MEASURE_TIME
    struct timespec start, finish;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
#endif
    debugIntegrity(s);
    double r = (double)rand() / RAND_MAX;
    double p_randomRecipe = 1;
    if (MODE == 1)
        p_randomRecipe = 0.9;
    if (r > p_randomRecipe) {
        success += swapRecipe(s);
    } else {
        success += randomRecipe(s);
    }
    calls++;

#ifdef MEASURE_TIME
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    randomRecipeTime +=
        finish.tv_sec - start.tv_sec + (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    debugIntegrity(s);
    return s;
}
/**
 * @todo swap chef may cause grade=0.
 */
States ChefRandomizer::operator()(States s) {
#ifdef MEASURE_TIME
    struct timespec start, finish;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
#endif
    debugIntegrity(s);
    double random = (double)rand() / RAND_MAX;
    double p_randomChef = 0.9;
    int path;
    calls++;
    if (MODE == 1)
        p_randomChef = 0.85;
    if (random < 1 - p_randomChef) {
        path = 0;
        success += swapChefTool(s);

    } else if (random >= 1 - p_randomChef) {
        path = 1;
        success += randomChef(s);
    }
    // for (int i = 0; i < NUM_CHEFS; i++) {
    //     for (int j = 0; j < DISH_PER_CHEF; j++) {
    //         if (i == j)
    //             continue;
    //         if (s[i] == s[j]) {
    //             std::cout << path << std::endl;
    //             exit(1);
    //         }
    //     }
    // }
    debugIntegrity(s);
    SARunner saRunner(c, r, false, f::t_dist_slow);
#ifdef MEASURE_TIME
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    randomChefTime +=
        finish.tv_sec - start.tv_sec + (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return saRunner.run(&s);
}
bool Randomizer::unrepeatedRandomRecipe(Skill &skill, Recipe **recipes,
                                        int size, int index,
                                        int repeats) const {
    int count = 0;
    Recipe *r;
    auto &rl = this->r;
    do {
        r = &rl->at(rand() % rl->size());
        count++;
    } while (
        ((skill.ability / r->cookAbility == 0) || inArray(recipes, size, r)) &&
        count < repeats);
    if (count >= repeats) {
        return false;
    }
    recipes[index] = r;

    return true;
}
