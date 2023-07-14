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

#ifdef MEASURE_TIME
extern double randomRecipeTime;
extern double randomChefTime;
extern double banquetRuleTime;
extern double getStatesSkillsTime;
extern double generateBanquetRuleTime, generateBanquetRuleTimeOut;
extern double calculatePriceTime, calculatePriceTimeOut;
#endif

void initChefRecipePairs(CList &, RList &);
struct Result {
    int score;
    int seed;
    CList *chefList;
    RList recipeList;
    States state;
    std::string logs;
};
void calculator(CList &, RList &);

// // define a try-catch function wrapper
// template <typename F, typename... Args>
// auto try_catch(F f, std::string f_name, Args &&...args)
//     -> decltype(f(std::forward<Args>(args)...));

class ResultJsonSerializable {
  public:
    int score;
    int seed;
    const CList *chefList;
    RList recipeList;
    const States state;
    std::string logs;
    ResultJsonSerializable(int score, int seed, CList *chefList,
                           RList recipeList, States state, std::string &logs)
        : score(score), seed(seed), chefList(chefList), recipeList(recipeList),
          state(state), logs(logs) {}
    ResultJsonSerializable(Result result)
        : score(result.score), seed(result.seed), chefList(result.chefList),
          recipeList(result.recipeList), state(result.state),
          logs(result.logs) {}
    Json::String toJson() {
        Json::Value result;
        result["score"] = score;
        result["seed"] = seed;
        Json::Value chefsList(Json::arrayValue);
        for (int i = 0; i < NUM_CHEFS; i++) {
            chefsList[i] = *state[i]->name;
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
          int targetScore, int iterChef = ITER_CHEF,
          int iterRecipe = ITER_RECIPE, bool allowTool = true);
Result run(const RuleInfo &rl, CList &chefList, RList &recipeList, int log,
           bool silent, int seed, int8_t *progress);
#ifdef EMSCRIPTEN
EMSCRIPTEN_BINDINGS(module) { emscripten::function("run", &runjs); }
#endif
#endif