
#include <iostream>
#include <string>
#include "Chef.hpp"
#include "Recipe.hpp"
#include <map>
#include "Calculator.hpp"
#include <vector>
#include "SARunner.hpp"
#include "functions.hpp"
#include "Randomizer.hpp"
#include "../config.hpp"
#include <stdio.h>
#include <fstream>
#include <time.h>
#include "exception.hpp"
#include <future>
#include <vector>
#include "bcjh_js.hpp"
#include "run.hpp"
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

/**
 * @return  pair<Json::Value gameData, Json::Value userData>
 */
std::pair<Json::Value, Json::Value> loadJson(std::stringstream &userDataSs) {
    Json::Value gameData;
    Json::Value userData;
    std::ifstream gameDataF("data.min.json", std::ifstream::binary);
    if (!gameDataF.good()) {
        gameDataF =
            std::ifstream("../data/data.min.json", std::ifstream::binary);
        if (!gameDataF.good())
            throw FileNotExistException();
    }
    gameDataF >> gameData;
    gameDataF.close();

    userDataSs >> userData;

    return {std::move(gameData), std::move(userData)};
}

std::string
#ifdef EMSCRIPTEN
    EMSCRIPTEN_KEEPALIVE
#endif
    runjs(const std::string &userDataIn, const std::string &ruleDataIn,
          int targetScore, int iterChef, int iterRecipe, bool allowTool
#ifdef EMSCRIPTEN_PROGRESS
          ,
          emscripten::val postProgress
#endif
    ) {
    const int T_MAX_CHEF = targetScore / 100;
    const int T_MAX_RECIPE = targetScore / 400;
    SARunner::init(T_MAX_CHEF, T_MAX_RECIPE, iterChef, iterRecipe, targetScore);
    bool silent = false;
    int log = SILENT | VERBOSE;
    struct timespec ts;
    int seed = (int)time(NULL);
#ifdef EMSCRIPTEN
    clock_gettime(CLOCK_MONOTONIC, &ts);
    seed += ts.tv_nsec + ts.tv_sec;
#endif
    // seed = 2115400760;
    std::stringstream userDataSs(userDataIn);
    std::stringstream ruleDataSs(ruleDataIn);
    Json::Value ruleDataJson;
    ruleDataSs >> ruleDataJson;

    RuleInfo rl;
    std::cout << "正在读取..." << std::endl;
    int num_guests = loadBanquetRuleFromInput(rl, ruleDataJson, true);
    if (num_guests == -1) {
        std::cout << "读取规则失败。" << std::endl;
        exit(1);
    } else if (num_guests != NUM_GUESTS) {
        return "期望" + std::to_string(NUM_GUESTS) +
               "位客人，但是读取到的客人数为" + std::to_string(num_guests) +
               "。";
    }
    auto [gameData, userData] = loadJson(userDataSs);
    auto [recipeList, chefList] = loadJson(gameData, userData, allowTool);

    // for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
    //     chef->loadRecipeCapable(recipeList);
    // }
    // Count time used
    clock_t start, end;
    start = clock();

    Result result = run(rl, chefList, recipeList, 0, true, seed
#ifdef EMSCRIPTEN_PROGRESS
                        ,
                        postProgress
#endif

    );
    std::cout << "run finished" << std::endl;
    log += ORDINARY;
    // Redirects std::cout
    std::stringstream ss;
    std::streambuf *oldCoutStreamBuf = std::cout.rdbuf(ss.rdbuf());

    std::cout << "随机种子：" << result.seed << std::endl;
    debugIntegrity(result.state);
    exactChefTool(rl, result.state);

    int score = sumPrice(rl, result.state, log, true);
    std::cout << "**************\n总分: " << result.score << "\n***************"
              << std::endl;
    end = clock();
    std::cout << "用时：" << (double)((end - start) / CLOCKS_PER_SEC) << "秒"
              << std::endl;
    // Restore std::cout
    std::cout.rdbuf(oldCoutStreamBuf);
    result.logs = ss.str();
#ifdef MEASURE_TIME
    std::cout << "randomRecipeTime: " << randomRecipeTime << std::endl;
    std::cout << "randomChefTime: " << randomChefTime << std::endl;
    std::cout << "banquetRuleTime: " << banquetRuleTime << std::endl;
    std::cout << "getStatesSkillsTime: " << getStatesSkillsTime << std::endl;
    std::cout << "generateBanquetRuleTime: " << generateBanquetRuleTime << " / "
              << generateBanquetRuleTimeOut << std::endl;
    std::cout << "calculatePriceTime: " << calculatePriceTime << " / "
              << calculatePriceTimeOut << std::endl;
#endif

    auto resultStr = ResultJsonSerializable(result).toJson();
    for (auto &chef : chefList) {
        chef.deletePointers();
    }
    return resultStr;
}
