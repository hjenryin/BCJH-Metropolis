#ifndef SARUNNER_HPP
#define SARUNNER_HPP
#include <iostream>
#include "functions.hpp"
#include <map>
#include <vector>
#include "Chef.hpp"
#include "Recipe.hpp"
#include <cmath>
#include <random>
#include <fstream>

struct History {
    States states;
    int energy;
    double t;
};

class SARunner {
    Randomizer *randomMoveFunc;
    f::CoolingSchedule coolingScheduleFunc;
    int stepMax;
    int tMax;
    int tMin;
    CList *chefList;
    RList *recipeList;
    e::GetEnergy getEnergyFunc;
    static int targetScore;

    States bestState;
#ifdef VIS_HISTORY
    History *history;
#endif
    States generateStates(CList *chefList, Chef *chefs[NUM_CHEFS]);
    static int T_MAX_CHEF;
    static int T_MAX_RECIPE;
    static int iterChef;
    static int iterRecipe;

  public:
    static void init(int T_MAX_CHEF, int T_MAX_RECIPE, int iterChef,
                     int iterRecipe, int targetScore) {
        SARunner::T_MAX_CHEF = T_MAX_CHEF;
        SARunner::T_MAX_RECIPE = T_MAX_RECIPE;
        SARunner::iterChef = iterChef;
        SARunner::iterRecipe = iterRecipe;
        SARunner::targetScore = targetScore;
    }
    int bestEnergy = 0;
    void print(States s, bool verbose = false) const;
    /**
     * @brief Construct a new SARunner object
     * @param chefList
     * @param recipeList
     * @param randomizeChef If false, randomize recipe.
     * @param getEnergyFunc
     * @param coolingScheduleFunc
     */
    SARunner(
        CList *chefList, RList *recipeList, bool randomizeChef,
        e::GetEnergy getEnergyFunc = e::getTotalPrice,
        f::CoolingSchedule coolingScheduleFunc = f::exponential_multiplicative);
    States run(States *s = NULL, bool progress = false, bool silent = false,
               const char *fn = NULL);
#ifdef VIS_HISTORY
    ~SARunner() { delete[] this->history; }
#endif
};
#endif