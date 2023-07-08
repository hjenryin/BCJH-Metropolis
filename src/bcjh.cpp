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

bool Chef::coinBuffOn = true;
void initChefRecipePairs(CList &, RList &);
struct Result {
    int score;
    int seed;
    CList *chefList;
    RList recipeList;
    States *state;
};
Result run(const RuleInfo &, const CList &, RList &, int, bool, int);
void calculator(RuleInfo &, CList &, RList &);
void loadJson(Json::Value &gameData, Json::Value &userData);
void parseArgs(int argc, char *argv[], bool &silent, int &log, bool &calculate,
               bool &mp, int &seed, int &bcjhId) {
    int seed_orig = seed;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-s") {
            silent = true;
        } else if (arg == "-v") {
            log += 0x10;
        } else if (arg == "-c") {
            calculate = true;
        } else if (arg == "-h") {
            std::cout << "-s: 无进度条" << std::endl;
            std::cout << "-v: 输出详细信息" << std::endl;
            std::cout << "-c: 从文件中读取配置，计算分数" << std::endl;
        } else if (arg == "--no-mp") {
            mp = false;
        } else if (arg == "--seed") {
            seed = atoi(argv[++i]);
#ifdef EMSCRIPTEN
        } else if (arg == "--bcjh-id") {
            bcjhId = atoi(argv[++i]);
#endif
        } else {
            std::cout << "未知参数：" << arg << std::endl;
            exit(1);
        }
        // set seed
    }
    if (seed_orig != seed) {
        if (mp) {
            mp = false;
            std::cout << "Seed set to " << seed << ", mp disabled" << std::endl;
        } else {
            std::cout << "Seed set to " << seed << std::endl;
        }
    }

    if (mp) {
        silent = true;
    }
}
int main(int argc, char *argv[]) {

    bool silent = false;
    int log = 0; // 0x0: 无输出 0x1: 正常输出 0x10: 详细输出
    int seed = (int)time(NULL);
    bool calculate = false;
    bool mp = true;
    int bcjhId = 0;
    std::cout << "BCJH-Metropolis " << std::endl;
    parseArgs(argc, argv, silent, log, calculate, mp, seed, bcjhId);
    std::cout << "Seed: " << seed << std::endl;

    CList chefList;
    RList recipeList;
    RuleInfo rl;
    Json::Value gameData;
    Json::Value userData;

    try {
        loadJson(gameData, userData);
        std::cout << "正在读取文件..." << std::endl;
        loadChef(chefList, gameData, userData);
        loadRecipe(recipeList, gameData, userData);
        loadBanquetRule(rl, gameData, 680020);
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
    time_t start, end;
    start = time(NULL);

    if (!calculate) {
        Result result;
        // result = run(rl, chefList, recipeList, log, silent, seed);
        // return 1;
        int num_threads = std::thread::hardware_concurrency();
        // #ifdef EMSCRIPTEN
        //         num_threads -= 2;
        //         num_threads = 9;
        //         if (num_threads < 1) {
        //             num_threads = 1;
        //         }
        // #endif

        if (!mp) {
            num_threads = 1;
        }
        num_threads = 1;
        seed = 625410139;
        log += 0x10;
        std::cout << "启用" << num_threads
                  << "线程，建议期间不要离开窗口，否则可能影响速度。"
                  << std::endl;

        std::vector<std::future<Result>> futures;

        for (int i = 0; i < num_threads; i++) {

            futures.push_back(std::async(
                std::launch::async, run, std::ref(rl), std::ref(chefList),
                std::ref(recipeList), 0, silent, seed++));
            silent = true;
        }
        std::cout << "分数：";
        int max_score = 0;
        for (auto &future : futures) {
            Result tmp = future.get();
            if (tmp.score > max_score) {
                result = tmp;
                max_score = result.score;

            } else {
                delete tmp.chefList;
                delete tmp.state;
            }
            std::cout << tmp.score << " ";
        }

        std::cout << "\n最佳结果：" << std::endl;

        log += 0x1;
        std::cout << "随机种子：" << result.seed << std::endl;
        int score = e0::sumPrice(rl, *result.state, result.chefList,
                                 &result.recipeList, log, true);
        std::cout << "**************\n总分: " << result.score
                  << "\n***************" << std::endl;
        if (!silent) {
            SARunner saRunnerPrint(&rl, result.chefList, &result.recipeList,
                                   ITER_RECIPE, T_MAX_RECIPE, 0, false,
                                   e::getTotalPrice, f::t_dist_fast);
            saRunnerPrint.run(result.state, false, silent, "../out/recipe");
        }
        delete result.chefList;
        delete result.state;
    } else {
        calculator(rl, chefList, recipeList);
    }
    end = time(NULL);
    std::cout << "用时：" << (end - start) << "秒" << std::endl;
}
Result run(const RuleInfo &rl, const CList &chefList, RList &recipeList,
           int log, bool silent, int seed) {
    CList *chefListPtr = new CList(chefList);
    *chefListPtr = chefList;
    srand(seed);
    SARunner saRunner(&rl, chefListPtr, &recipeList, ITER_CHEF, T_MAX_CHEF,
                      T_MAX_CHEF / 10, true, e::getTotalPrice, f::t_dist_slow);
    // std::cout << log << std::endl;
    States *s = new States;
    *s = saRunner.run(NULL, true, silent);
    *s = perfectChef(rl, *s, chefListPtr);
    int score = e0::sumPrice(rl, *s, chefListPtr, &recipeList, log, false);
    return Result{score, seed, chefListPtr, recipeList, s};
}

void calculator(RuleInfo &rl, CList &chefList, RList &recipeList) {
    std::ifstream f;
    f.open("../in/out.txt");
    States s;
    for (int i = 0; i < NUM_CHEFS; i++) {
        int c, t;
        f >> c >> t;
        if (t == NO_TOOL) {
            t = 0;
        }
        s.chef[i] = &chefList[c * 6 + t];
        s.chef[i]->print();
    }
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        int r;
        f >> r;
        s.recipe[i] = &recipeList[r];
    }
    f.close();
    int score = e0::sumPrice(rl, s, &chefList, &recipeList, false, true);
    SARunner saRunner(&rl, &chefList, &recipeList, ITER_CHEF, T_MAX_CHEF, 0,
                      true);
    saRunner.print(s, true);
    std::cout << "\n\nScore: " << score << std::endl;
}
void loadJson(Json::Value &gameData, Json::Value &userData) {
    std::ifstream gameDataF("data.min.json", std::ifstream::binary);
    if (!gameDataF.good()) {
        gameDataF.open("../data/data.min.json", std::ifstream::binary);
        if (!gameDataF.good()) {
            throw FileNotExistException();
        }
    }
    // std::cout << gameDataF.fail() << std::endl;
    gameDataF >> gameData;
    gameDataF.close();

    std::ifstream usrDataF("userData.json", std::ifstream::binary);
    if (!usrDataF.good()) {
        usrDataF.open("../data/userData.json", std::ifstream::binary);
        if (!usrDataF.good()) {
            throw FileNotExistException();
        }
    }
    // std::cout << "Game data loaded" << std::endl;
    usrDataF >> userData;
    usrDataF.close();
}