#ifndef BCJH_JS_HPP
#define BCJH_JS_HPP
#include <iostream>
#include <string>
#include "Chef.hpp"
#include "Recipe.hpp"
#include <map>
#include "Calculator.hpp"
#include <vector>
#include "SARunner.hpp"
#include "functions.hpp"
#include "../config.hpp"
#include <stdio.h>
#include <fstream>
#include <time.h>
#include "exception.hpp"
#include <future>
#include <vector>
#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
using namespace emscripten;
#endif
#include "run.hpp"

#ifdef MEASURE_TIME
extern double randomRecipeTime;
extern double randomChefTime;
extern double banquetRuleTime;
extern double getStatesSkillsTime;
extern double generateBanquetRuleTime, generateBanquetRuleTimeOut;
extern double calculatePriceTime, calculatePriceTimeOut;
#endif

class ResultJsonSerializable {
  public:
    int score;
    int seed;
    const States state;
    std::string logs;
    ResultJsonSerializable(int score, int seed, States state, std::string &logs)
        : score(score), seed(seed), state(state), logs(logs) {}
    ResultJsonSerializable(Result &result)
        : score(result.score), seed(result.seed), state(result.state),
          logs(result.logs) {}
    Json::String toJson() {
        Json::Value result;
        result["score"] = score;
        result["seed"] = seed;
        Json::Value chefsList(Json::arrayValue);
        for (int i = 0; i < NUM_CHEFS; i++) {
            chefsList[i] = *state.getChefPtr(i)->name;
        }
        result["chefs"] = chefsList;
        Json::Value recipesList(Json::arrayValue);
        for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
            recipesList[i] = state.recipe[i]->name;
        }
        result["recipes"] = recipesList;
        result["logs"] = logs;
        return result.toStyledString();
    }
};
/**
 * @return  pair<Json::Value gameData, Json::Value userData>
 */
std::pair<Json::Value, Json::Value> loadJson(std::stringstream &userDataSs);
std::string
#ifdef EMSCRIPTEN
    EMSCRIPTEN_KEEPALIVE
#endif
    runjs(const std::string &userDataIn, const std::string &ruleDataIn,
          int targetScore, int iterChef = 5000, int iterRecipe = 1000,
          bool allowTool = true
#ifdef EMSCRIPTEN_PROGRESS
          ,
          emscripten::val postProgress = emscripten::val::null()
#endif
    );

#ifdef EMSCRIPTEN
EMSCRIPTEN_BINDINGS(module) { emscripten::function("run", &runjs); };
#endif
#endif