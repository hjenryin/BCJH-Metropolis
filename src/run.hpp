#ifndef SHARED_RUN_HPP
#define SHARED_RUN_HPP

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
#include "banquetRuleGen.hpp"
// #ifdef _WIN32
// #include <direct.h>
// #else
// #include <unistd.h>
// #endif
#include "utils/ProgressBar.hpp"
struct Result {
    int score;
    int seed;
    States state;
    std::string logs;
};
Result run(const RuleInfo &rl, const CList &chefList, RList &recipeList,
           int log, bool silent, int seed
#ifdef EMSCRIPTEN_PROGRESS
           ,
           emscripten::val postProgress
#endif
           ,
           int threadId = -1, States *stateResumed = nullptr);

std::tuple<RList, CList> loadJson(const Json::Value &gameData,
                                  const Json::Value &userData
#ifndef _WIN32
                                  ,
                                  bool allowTool
#endif
);

#endif
