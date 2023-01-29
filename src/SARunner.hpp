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
    CRPairs *chefRecipePairs;
    r::RandomMove randomMoveFunc;
    f::CoolingSchedule coolingScheduleFunc;
    int stepMax;
    int tMax;
    int tMin;
    std::map<int, Chef> *chefList;
    std::map<int, Recipe> *recipeList;
    e::GetEnergy getEnergyFunc;

    States bestState;
    History *history;

    States generateStates(CList *chefList, CRPairs *chefRecipePairs,
                          Chef *chefs[NUM_CHEFS]);

  public:
    int bestEnergy = 0;
    void print(States s, bool verbose = false);
    /**
     * @brief Construct a new SARunner object
     *
     * @param chefList
     * @param recipeList
     * @param chefRecipePairs
     * @param stepMax
     * @param tMax
     * @param tMin
     * @param randomMoveFunc
     * @param coolingScheduleFunc
     */
    SARunner(
        CList *chefList, RList *recipeList, CRPairs *chefRecipePairs,
        int stepMax, int tMax, int tMin,
        e::GetEnergy getEnergyFunc = e::getTotalPrice,
        r::RandomMove randomMoveFunc = r::randomRecipe,
        f::CoolingSchedule coolingScheduleFunc = f::exponential_multiplicative);
    States run(Chef *chefs[NUM_CHEFS] = NULL, bool progress = false,
               bool silent = false, const char *fn = NULL);
    ~SARunner();
};
#endif