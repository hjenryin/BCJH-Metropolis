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
struct States;
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
    int targetScore;

    States bestState;
    History *history;
    const RuleInfo *rl;
    States generateStates(CList *chefList, Chef *chefs[NUM_CHEFS]);

  public:
    int bestEnergy = 0;
    void print(States s, bool verbose = false);
    /**
     * @brief Construct a new SARunner object
     * @param chefList
     * @param recipeList
     * @param stepMax
     * @param tMax
     * @param tMin
     * @param randomizeChef If false, randomize recipe.
     * @param getEnergyFunc
     * @param coolingScheduleFunc
     */
    SARunner(
        const RuleInfo *rl, CList *chefList, RList *recipeList, int stepMax,
        int tMax, int tMin, bool randomizeChef,
        e::GetEnergy getEnergyFunc = e::getTotalPrice,
        f::CoolingSchedule coolingScheduleFunc = f::exponential_multiplicative);
    States run(States *s = NULL, bool progress = false, bool silent = false,
               const char *fn = NULL);
    ~SARunner();
};
#endif