#include "functions.hpp"
#include "SARunner.hpp"
#include "../config.hpp"
#include "exception.hpp"
#include "activityRule.hpp"
#include <cassert>
#include "banquetRule.hpp"
const double bestToolProb = 0.9;
extern double generateBanquetRuleTime, generateBanquetRuleTimeOut;
extern double calculatePriceTime, calculatePriceTimeOut;

ToolEnum toolHeuristic(States &s, int chefId) {
    auto chef = s.getChef(chefId);
    Recipe **recipes = &s.recipe[chefId * DISH_PER_CHEF];
    if (chef->getTool() == NO_TOOL)
        return NO_TOOL;
    ToolEnum best = NOT_EQUIPPED;
    chef->modifyTool(NOT_EQUIPPED);
    int max = 0;
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        max += chef->skill.ability / recipes[i]->cookAbility;
    }
    for (int i = ABILITY_ENUM_START; i < ABILITY_ENUM_END; i++) {
        auto tool = (ToolEnum)i;
        chef->modifyTool(tool);
        int value = 0;
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            value += chef->skill.ability / recipes[i]->cookAbility;
        }
        if (value > max) {
            max = value;
            best = tool;
        }
    }
    return best;
}

int e::getTotalPrice(States s, CList *chefList, RList *recipeList,
                     bool verbose) {
    return e0::sumPrice(s, chefList, recipeList, verbose, false);
}

bool ChefRandomizer::randomChef(States &s) const {
    auto &chefList = this->c;
    int chefNum = rand() % NUM_CHEFS;
    Chef *pChef = s.getChef(chefNum);
    int count = 0;
    auto learned = &(pChef->recipeLearned);
    learned->clear();
    int dishNum = chefNum * DISH_PER_CHEF;
    int totalDishNum = NUM_CHEFS * DISH_PER_CHEF;
    for (int i = 0; i < DISH_PER_CHEF; i++) {
        learned->push_back(s.recipe[dishNum + i]);
    }
    do {
        pChef = &chefList->at(rand() % chefList->size());
        count++;
    } while (s.repeatChef(pChef, -1) && count < RANDOM_SEARCH_TIMEOUT);
    if (count >= RANDOM_SEARCH_TIMEOUT) {
        throw NoChefException();
    }
    s.setChef(chefNum, pChef);
    if (pChef->recipeLearned.size() == DISH_PER_CHEF) {
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            s.recipe[dishNum + i] = pChef->recipeLearned[i];
        }
    }
    bool changed = true;
    auto oldS = s;
    oldS.saveChefTool();
    Skill &skill = s.getSkills()[chefNum];
    for (int i = dishNum; i < dishNum + DISH_PER_CHEF; i++) {
        if ((skill.ability / s.recipe[i]->cookAbility == 0) ||
            inArray(s.recipe, NUM_CHEFS * DISH_PER_CHEF, s.recipe[i])) {
            bool thisChanged = this->unrepeatedRandomRecipe(
                skill, s.recipe, totalDishNum, i,
                RANDOM_SEARCH_TIMEOUT * RANDOM_SEARCH_TIMEOUT);
            changed = changed && thisChanged;
        }
    }
    // std::cout << changed << std::endl;
    if (changed) {
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "randomChef T" << std::endl;
            exit(1);
        }

        return true;
    } else {
        s = oldS;
        s.loadChefTool();
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "randomChef F" << std::endl;
            exit(1);
        }
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
        const Chef *chef1 = s.getConstChef(chefNum1);
        const Chef *chef2 = s.getConstChef(chefNum2);
        if (!toolChanged && random < bestToolProb) {
            toolChanged = true;
            Chef *chef = s.getChef(chefNum1);
            chef->modifyTool(toolHeuristic(s, chefNum1));
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
    s.saveChefTool();
    States saveS = s;
    if (hasRepeatedRecipe(s.recipe)) {
        std::cout << "swap orig" << std::endl;
        // exit(1);
    }
    int i;
    for (i = 0; i < RANDOM_SEARCH_TIMEOUT; i++) {
        s = saveS;
        s.loadChefTool();
        int chefNum = rand() % NUM_CHEFS;
        auto chef = s.getChef(chefNum);
        int orig_tool = chef->getTool();

        int tool;
        do {
            tool = rand() % 6 + ABILITY_ENUM_START;
        } while (tool == orig_tool);
        chef->modifyTool((ToolEnum)tool);
        auto &skill = s.getSkills()[chefNum];
        auto &ability = skill.ability;
        for (int i = chefNum * DISH_PER_CHEF;
             i < chefNum * DISH_PER_CHEF + DISH_PER_CHEF; i++) {
            if (ability / s.recipe[i]->cookAbility == 0) {
                bool thisChanged = this->unrepeatedRandomRecipe(
                    skill, s.recipe, NUM_CHEFS * DISH_PER_CHEF, i);
                if (hasRepeatedRecipe(s.recipe)) {
                    std::cout << "swap in" << std::endl;
                    // exit(1);
                }
                if (!thisChanged) {
                    break;
                }
            }
        }
    }
    if (i >= RANDOM_SEARCH_TIMEOUT) {
        s = saveS;
        s.loadChefTool();
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "swap false" << std::endl;
            // exit(1);
        }
        return false;
    } else {
        s.saveChefTool();
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "swap true" << std::endl;
            // exit(1);
        }
        return true;
    }
}
/**
 * @warning this function involves a large copy constructor.
 * @return whether after deduction, the price is still the same
 */
bool deductTool(States s, CList *chefList, RList *recipeList, int chefId,
                int deduction) {
    Chef chef(*s.getConstChef(chefId));
    int tool = chef.getTool();
    int *cookAbility;
    switch (tool) {
    case STIRFRY:
        cookAbility = &chef.skill.ability.stirfry;
        break;
    case BOIL:
        cookAbility = &chef.skill.ability.boil;
        break;
    case FRY:
        cookAbility = &chef.skill.ability.fry;
        break;
    case STEAM:
        cookAbility = &chef.skill.ability.steam;
        break;
    case BAKE:
        cookAbility = &chef.skill.ability.bake;
        break;
    case KNIFE:
        cookAbility = &chef.skill.ability.knife;
        break;
    case NO_TOOL:
        return true;
    default:
        // std::cout << "Not using a tool" << std::endl;
        return true;
    }
    *cookAbility -= deduction;
    int bestPrice = e0::sumPrice(s, chefList, recipeList, false, false);
    States newState = s;
    newState.setChef(chefId, &chef);
    int newPrice = e0::sumPrice(newState, chefList, recipeList, false, false);
    return newPrice == bestPrice;
}
/**
 * @brief
 * @param exactChefTool whether to use the exact tool deduction.Warning: it
 * should only be set true at the end of the function as it modifies the name of
 * the chefs.
 */
int e0::sumPrice(States s, CList *chefList, RList *recipeList, int log,
                 bool exactChefTool) {
    if (exactChefTool) {

        s.saveChefTool();
        assert(chefList != NULL && recipeList != NULL);
        // std::cout << "exactChefTool" << std::endl;
        for (int i = 0; i < NUM_CHEFS; i++) {
            ToolEnum tool = s.getChef(i)->getTool();
            std::string toolName = getToolName(tool);
            toolName = "-" + toolName;
            if (deductTool(s, chefList, recipeList, i, 40)) {
                if (deductTool(s, chefList, recipeList, i, 70)) {
                    if (deductTool(s, chefList, recipeList, i, 100)) {
                        s.getChef(i)->name += toolName + "(0)";
                    } else {
                        s.getChef(i)->name += toolName + "(30)";
                    }
                } else {
                    s.getChef(i)->name += toolName + "(60)";
                }
            } else {
                s.getChef(i)->name += toolName + "(100)";
            }
        }
    }
    if (MODE == 1) {

        BanquetRuleTogether rule[NUM_CHEFS * DISH_PER_CHEF];
        int bestFull[NUM_GUESTS];
        banquetRule(rule, s, bestFull);
#ifdef MEASURE_TIME
        struct timespec start, finish;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
#endif
        BanquetInfo biCache;
        int totalScore = 0;
        int totalFull = 0;
        int scoreCache = 0;
        int fullCache = 0;
        int ans = 0;
        int dishStart = 0;
        int chefStart = 0;
        for (int g = 0; g < NUM_GUESTS; g++) {
            dishStart = DISH_PER_CHEF * CHEFS_PER_GUEST * g;
            chefStart = CHEFS_PER_GUEST * g;
            totalScore = 0;
            totalFull = 0;
            scoreCache = 0;
            fullCache = 0;
            auto skills = s.getSkills();
            for (int i = 0; i < DISH_PER_CHEF * CHEFS_PER_GUEST; i++) {
                if ((log & 0x10) && i % 3 == 0) {
                    std::cout << "VERBOSE************" << std::endl;
                    s.getConstChef(chefStart + i / 3)->print();
                    std::cout << "************" << std::endl;
                }
                biCache =
                    getPrice(skills[chefStart + i / 3], s.recipe[dishStart + i],
                             rule[dishStart + i], (log & 0x10));
                totalFull += biCache.full;
                totalScore += biCache.price;
                scoreCache += biCache.price;
                fullCache += biCache.full;
                if ((log & 0x1) && i % 3 == 2) {
                    std::cout << "  厨师："
                              << s.getConstChef(chefStart + i / 3)->name
                              << " -> " << fullCache << " / " << scoreCache
                              << std::endl;
                    scoreCache = 0;
                    fullCache = 0;
                    std::cout << "  菜谱：" << s.recipe[dishStart + i - 2]->name
                              << "；" << s.recipe[dishStart + i - 1]->name
                              << "；" << s.recipe[dishStart + i]->name
                              << std::endl;
                }
            }
            int guestScore;
            switch (totalFull - bestFull[g]) {
            case 0:
                guestScore = (int)std::ceil(totalScore * 1.3);
                break;
            default:
                int delta = std::abs(totalFull - bestFull[g]);
                guestScore = (int)std::ceil(totalScore * (1 - 0.05 * delta));
            }
            ans += guestScore;
            if (log & 0x1)
                std::cout << "第" << g + 1 << "位客人：" << totalFull << " / "
                          << bestFull[g] << " -> " << guestScore << ""
                          << std::endl;
        }
#ifdef MEASURE_TIME
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
        banquetRuleTime += finish.tv_sec - start.tv_sec +
                           (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
        return ans;
    } else if (MODE == 2 || MODE == 0) {
        ActivityBuff activityBuff;
        auto p = &activityBuff;
        activityRule(p);
        if (MODE == 0)
            p = NULL;
        int energy = 0;
        int r = 0;
        for (int i = 0; i < NUM_CHEFS; i++) {
            if ((log & 0x10)) {
                std::cout << "VERBOSE************" << std::endl;
                s.getConstChef(i)->print();
                std::cout << "************" << std::endl;
            }
            int scoreCache = 0;
            if (log & 0x1)
                std::cout << "厨师：" << s.getConstChef(i)->name << std::endl
                          << "菜谱：";
            auto skills = s.getSkills();

            for (int j = 0; j < DISH_PER_CHEF; j++) {
                if (log & 0x1)
                    std::cout << s.recipe[r]->name << "；";
                scoreCache +=
                    getPrice(skills[i], *s.recipe[r++], p, (log & 0x10));
            }
            energy += scoreCache;
            if (log & 0x1)
                std::cout << " -> " << scoreCache << std::endl;
        }
        return energy;
    } else {
        std::cout
            << "config.hpp中MODE设置错误。0为正常营业，1为宴会，2为限时任务"
            << std::endl;
        exit(1);
    }
}
States RecipeRandomizer::operator()(States s) {
#ifdef MEASURE_TIME
    struct timespec start, finish;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
#endif
    double r = (double)rand() / RAND_MAX;
    double p_randomRecipe = 1;
    if (MODE == 1)
        p_randomRecipe = 0.9;
    if (r > p_randomRecipe) {

        success += swapRecipe(s);
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "Swap recipe" << std::endl;
            exit(1);
        }
    } else {
        success += randomRecipe(s);
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "Random recipe" << std::endl;
            exit(1);
        }
    }
    calls++;

#ifdef MEASURE_TIME
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    randomRecipeTime +=
        finish.tv_sec - start.tv_sec + (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
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
    double random = (double)rand() / RAND_MAX;
    double p_randomChef = 0.9;
    int path;
    calls++;
    if (MODE == 1)
        p_randomChef = 0.85;
    if (random < 0.1) {
        path = 0;
        success += swapChefTool(s);
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "SwapTool" << std::endl;
            exit(1);
        }
    } else if (random >= 1 - p_randomChef) {
        path = 1;
        success += randomChef(s);
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "RandomChef" << std::endl;
            exit(1);
        }

    } else { // swap Chef
        success++;
        path = 2;
        int chefIndex1 = rand() % NUM_CHEFS;
        int chefIndex2;
        do {
            chefIndex2 = rand() % NUM_CHEFS;
        } while (chefIndex1 == chefIndex2);
        auto chef1 = s.getChef(chefIndex1);
        auto chef2 = s.getChef(chefIndex2);
        s.setChef(chefIndex1, chef1);
        s.setChef(chefIndex2, chef2);
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            swap(s.recipe[chefIndex1 * DISH_PER_CHEF + i],
                 s.recipe[chefIndex2 * DISH_PER_CHEF + i]);
        }
        if (hasRepeatedRecipe(s.recipe)) {
            std::cout << "SwapChef" << std::endl;
            exit(1);
        }
    }
    // for (int i = 0; i < NUM_CHEFS; i++) {
    //     for (int j = 0; j < DISH_PER_CHEF; j++) {
    //         if (i == j)
    //             continue;
    //         if (s.getConstChef(i) == s.getConstChef(j)) {
    //             std::cout << path << std::endl;
    //             exit(1);
    //         }
    //     }
    // }

    SARunner saRunner(c, r, false, e::getTotalPrice, f::t_dist_slow);
#ifdef MEASURE_TIME
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    randomChefTime +=
        finish.tv_sec - start.tv_sec + (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
    return saRunner.run(&s);
}

void swap(Recipe *&a, Recipe *&b) {
    Recipe *temp = a;
    a = b;
    b = temp;
}

double f::exponential_multiplicative(int stepMax, int step, double tMax,
                                     double tMin) {
    return tMax * std::exp(-step / 1000.0);
}
double f::linear(int stepMax, int step, double tMax, double tMin) {
    return (tMax - tMin) * (1 - step / (double)stepMax) + tMin;
}
double f::t_dist_fast(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step * step / 300000.0);
}
double f::t_dist_slow(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step * step / 3000000.0);
}
double f::linear_mul(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step / 100000.0);
}
double f::zipf(int stepMax, int step, double tMax, double tMin) {
    double t = tMin + (tMax - tMin) / std::pow(step + 1, 0.2);
    // std::cout << t << std::endl;
    return t;
}
double f::one_over_n(int stepMax, int step, double tMax, double tMin) {
    return tMax / std::pow(step + 1, 0.1);
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
        if (hasRepeatedRecipe(recipes)) {
            std::cout << "Unrepeated false" << std::endl;
            // exit(1);
        }
        return false;
    }
    recipes[index] = r;
    if (hasRepeatedRecipe(recipes)) {
        std::cout << "Unrepeated true" << std::endl;
        // exit(1);
    }
    return true;
}
States perfectTool(States &s) {
    for (int i = 0; i < NUM_CHEFS; i++) {
        auto chef = s.getChef(i);
        if (chef->getTool() == NO_TOOL)
            continue;
        chef->modifyTool(NOT_EQUIPPED);
        int max = e0::sumPrice(s);
        ToolEnum bestTool = NOT_EQUIPPED;
        for (int j = ABILITY_ENUM_START; j < ABILITY_ENUM_END; j++) {
            chef->modifyTool(ToolEnum(j));
            int temp = e0::sumPrice(s);
            if (temp > max) {
                max = temp;
                bestTool = ToolEnum(j);
            }
        }
        chef->modifyTool(bestTool);
    }
    s.saveChefTool();
    return s;
}
States perfectChef(States &s, CList *c) {
    // perform a one-shot deviation from current state
    States newS = s;
    States bestS = s;
    int bestSscore = e0::sumPrice(bestS);
    bestS.saveChefTool();
    for (int i = 0; i < NUM_CHEFS; i++) {
        for (auto &chef : *c) {
            if (newS.repeatChef(&chef, i)) {
                continue;
            }

            newS.setChef(i, &chef);
            States pS = perfectTool(newS);
            int pSs = e0::sumPrice(pS);

            if (pSs > bestSscore) {
                bestS = pS;
                bestSscore = pSs;
                bestS.saveChefTool();
            }
        }
    }
    bestS.loadChefTool();
    return bestS;
}