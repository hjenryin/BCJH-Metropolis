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
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include "utils/ProgressBar.hpp"
struct Result {
    int score;
    int seed;
    const CList *chefList;
    RList recipeList;
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
           int threadId = -1) {
    auto *chefListPtr = &chefList;
    srand(seed);
    SARunner saRunner(&rl, chefListPtr, &recipeList, true, f::t_dist_slow,
                      threadId);

    auto s = saRunner.run(NULL,
#ifdef EMSCRIPTEN_PROGRESS
                          postProgress,
#endif
                          silent);
    // *s = perfectChef(rl, *s, chefListPtr);
    int score = sumPrice(rl, s, log, false);
    debugIntegrity(s);
    return Result{score, seed, chefListPtr, recipeList, s, ""};
}
#endif