
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
#include "../data/userData.hpp"

#include <emscripten/bind.h>
using namespace emscripten;

bool Chef::coinBuffOn = true;
void initChefRecipePairs(CList &, RList &);
struct Result {
    int score;
    int seed;
    CList *chefList;
    RList recipeList;
    States *state;
};
Result run(const CList &, RList &, int, bool, int);
void calculator(CList &, RList &);

class ResultJsonSerializable {
  public:
    int score;
    int seed;
    CList *chefList;
    RList recipeList;
    States *state;
    ResultJsonSerializable(int score, int seed, CList *chefList,
                           RList recipeList, States *state)
        : score(score), seed(seed), chefList(chefList), recipeList(recipeList),
          state(state) {}
    ResultJsonSerializable(Result result)
        : score(result.score), seed(result.seed), chefList(result.chefList),
          recipeList(result.recipeList), state(result.state) {}
    Json::String toJson() {
        Json::Value result;
        result["score"] = score;
        result["seed"] = seed;
        Json::Value chefsList(Json::arrayValue);
        for (int i = 0; i < NUM_CHEFS; i++) {
            chefsList[i] = state->chef[i]->name;
        }
        result["chefs"] = chefsList;
        Json::Value recipesList(Json::arrayValue);
        for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
            recipesList[i] = state->recipe[i]->name;
        }
        result["recipes"] = recipesList;
        return result.toStyledString();
    }
};

std::string EMSCRIPTEN_KEEPALIVE runjs(const std::string &data) {

    bool silent = false;
    int log = 0; // 0x0: 无输出 0x1: 正常输出 0x10: 详细输出
    int seed = (int)time(NULL);

    std::stringstream dataStream1(data);
    std::stringstream dataStream2(data);

    CList chefList;
    RList recipeList;
    try {
        std::cout << "正在读取文件..." << std::endl;
        loadChef(chefList, &dataStream1);
        loadRecipe(recipeList, &dataStream2);
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

    for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
        chef->loadRecipeCapable(recipeList);
    }
    // Count time used
    clock_t start, end;
    start = clock();

    Result result = run(chefList, recipeList, 0, true, seed);
    // #ifdef EMSCRIPTEN
    //         num_threads -= 2;
    //         num_threads = 9;
    //         if (num_threads < 1) {
    //             num_threads = 1;
    //         }
    // #endif
    log += 0x1;
    std::cout << "随机种子：" << result.seed << std::endl;
    int score = e0::sumPrice(*result.state, result.chefList, &result.recipeList,
                             log, true);
    std::cout << "**************\n总分: " << result.score << "\n***************"
              << std::endl;
    if (!silent) {
        SARunner saRunnerPrint(result.chefList, &result.recipeList, ITER_RECIPE,
                               T_MAX_RECIPE, 0, e::getTotalPrice,
                               r::randomRecipe, f::t_dist_fast);
        saRunnerPrint.run(result.state, false, silent, "../out/recipe");
    }
    end = clock();
    std::cout << "用时：" << (double)((end - start) / CLOCKS_PER_SEC) << "秒"
              << std::endl;
    return ResultJsonSerializable(result).toJson();
}

Result run(const CList &chefList, RList &recipeList, int log, bool silent,
           int seed) {
    CList *chefListPtr = new CList(chefList);
    *chefListPtr = chefList;
    srand(seed);
    SARunner saRunner(chefListPtr, &recipeList, ITER_CHEF, T_MAX_CHEF,
                      T_MAX_CHEF / 10, e::getTotalPrice, r::randomChef,
                      f::t_dist_slow);
    // std::cout << log << std::endl;
    States *s = new States;
    *s = saRunner.run(NULL, true, silent);
    *s = perfectChef(*s, chefListPtr);
    int score = e0::sumPrice(*s, chefListPtr, &recipeList, log, false);
    return Result{score, seed, chefListPtr, recipeList, s};
}

EMSCRIPTEN_BINDINGS(module) { emscripten::function("run", &runjs); }
