
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
Result run(const RuleInfo &, const CList &, RList &, int, bool, int);
void calculator(CList &, RList &);
#define FUNC_DEF(func) func, #func

// define a try-catch function wrapper
template <typename F, typename... Args>
auto try_catch(F f, std::string f_name, Args &&...args)
    -> decltype(f(std::forward<Args>(args)...)) {
    try {
        return f(std::forward<Args>(args)...);
    } catch (const FileNotExistException &e) {
        std::cout << "json文件缺失。如果在网页端，请确认已经上传了文件；如果在"
                     "本地，请确认已经下载了文件。\n";
    } catch (const Json::RuntimeError &e) {
        std::cout << "json文件格式不正确。如果文件内容是手动复制的，确认文件已"
                     "经复制完整。\n";
    } catch (const Json::LogicError &e) {
        std::cout << "json文件格式不正确。请确认文件来自白菜菊花而非图鉴网。\n";
    };
    std::cout << "Exception for function: " << f_name << std::endl;
    exit(1);
}
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
    // std::cout << gameDataF.fail() << std::endl;
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
          int targetScore, int iterChef, int iterRecipe, bool allowTool,
          bool verbose) {
    const int T_MAX_CHEF = targetScore / 100;
    const int T_MAX_RECIPE = targetScore / 400;
    SARunner::init(T_MAX_CHEF, T_MAX_RECIPE, iterChef, iterRecipe, targetScore);

    bool silent = false;
    int log = SILENT;
    if (verbose) {
        log += VERBOSE;
    }
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // std::cout << ts.tv_nsec << std::endl;
    // std::cout << ts.tv_sec << std::endl;
    int seed = ts.tv_nsec + ts.tv_sec + (int)time(NULL);
    // seed = 2115400760;
    std::stringstream userDataSs(userDataIn);
    std::stringstream ruleDataSs(ruleDataIn);
    Json::Value ruleDataJson;
    ruleDataSs >> ruleDataJson;
    CList chefList;
    RList recipeList;

    RuleInfo rl;
    std::cout << "正在读取..." << std::endl;
    int num_guests =
        try_catch(FUNC_DEF(loadBanquetRuleFromInput), rl, ruleDataJson, true);
    // std::cout << num_guests << std::endl;
    if (num_guests == -1) {
        std::cout << "读取规则失败。" << std::endl;
        exit(1);
    } else if (num_guests != NUM_GUESTS) {
        return "期望" + std::to_string(NUM_GUESTS) +
               "位客人，但是读取到的客人数为" + std::to_string(num_guests) +
               "。";
    }
    auto [gameData, userData] = try_catch(FUNC_DEF(loadJson), userDataSs);
    try_catch(FUNC_DEF(Skill::loadJson), gameData["skills"]);
    try_catch(FUNC_DEF(Recipe::initRarityBuff), userData["userUltimate"]);
    try_catch(FUNC_DEF(Chef::initBuff), userData["userUltimate"]);
    try_catch(FUNC_DEF(loadChef), chefList, gameData, userData, allowTool);
    try_catch(FUNC_DEF(loadRecipe), recipeList, gameData, userData);

    std::cout << "读取文件成功。" << std::endl;

    try {

        // Skill::loadJson(gameData["skills"]);
        // Recipe::initRarityBuff(userData["userUltimate"]);
        // Chef::initBuff(userData["userUltimate"]);

        // loadChef(chefList, gameData, userData);
        // // for (auto chef = chefList.begin(); chef != chefList.end(); chef++)
        // {
        // //     chef->print();
        // // }
        // loadRecipe(recipeList, gameData, userData);
        // loadBanquetRuleFromInput(rl, ruleDataJson, true);
        std::cout << "读取文件成功。" << std::endl;
    } catch (FileNotExistException &e) {
        std::cout << "json文件缺失。如果在网页端，请确认已经上传了文件；如果在"
                     "本地，请确认已经下载了文件。\n";
        exit(1);
    } catch (Json::RuntimeError &e) {
        std::cout << "json文件格式不正确。如果文件内容是手动复制的，确认文件已"
                     "经复制完整。\n";
        exit(1);
    } catch (Json::LogicError &e) {
        std::cout << "json文件格式不正确。请确认文件来自白菜菊花而非图鉴网。\n";
        exit(1);
    }

    // for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
    //     chef->loadRecipeCapable(recipeList);
    // }
    // Count time used
    clock_t start, end;
    start = clock();

    Result result = run(rl, chefList, recipeList, 0, true, seed);
    std::cout << "run finished" << std::endl;
    log += ORDINARY;
    std::cout << "随机种子：" << result.seed << std::endl;
    debugIntegrity(result.state);
    int score = sumPrice(rl, result.state, log, true);
    std::cout << "**************\n总分: " << result.score << "\n***************"
              << std::endl;
    if (!silent) {
        SARunner saRunnerPrint(&rl, result.chefList, &result.recipeList, false,
                               f::t_dist_fast);
        saRunnerPrint.run(&result.state, false, silent);
    }
    end = clock();
    std::cout << "用时：" << (double)((end - start) / CLOCKS_PER_SEC) << "秒"
              << std::endl;
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
        delete chef.recipeLearned;
    }
    return resultStr;
}

Result run(const RuleInfo &rl, CList &chefList, RList &recipeList, int log,
           bool silent, int seed) {
    auto *chefListPtr = &chefList;
    seed = -1676265360;
    srand(seed);
    // throw std::runtime_error("Not implemented");
    SARunner saRunner(&rl, chefListPtr, &recipeList, true, f::t_dist_slow);
    // std::cout << log << std::endl;
    auto s = saRunner.run(NULL, true, silent);
    // *s = perfectChef(rl, *s, chefListPtr);
    int score = sumPrice(rl, s, log, false);
    debugIntegrity(s);
    return Result{score, seed, chefListPtr, recipeList, s};
}
#ifdef EMSCRIPTEN
EMSCRIPTEN_BINDINGS(module) { emscripten::function("run", &runjs); }
#endif
