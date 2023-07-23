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

const int targetScore = 3170000;
const int T_MAX_CHEF = targetScore / 100;
const int T_MAX_RECIPE = targetScore / 400;

bool Chef::coinBuffOn = true;
void initChefRecipePairs(CList &, RList &);
struct Result {
    int score;
    int seed;
    CList *chefList;
    RList recipeList;
    States *state;
};
Result run(int, const RuleInfo &, const CList &, RList &, int, bool, int);
void calculator(CList &, RList &);
std::tuple<Json::Value, Json::Value, Json::Value> loadJsonFile();

void parseArgs(int argc, char *argv[], bool &silent, int &log, bool &calculate,
               bool &mp, int &seed) {
    int seed_orig = seed;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-s") {
            silent = true;
        } else if (arg == "-v") {
            log += 0x10;
        } else if (arg == "-h") {
            std::cout << "-s: 无进度条" << std::endl;
            std::cout << "-v: 输出详细信息" << std::endl;
            std::cout << "-c: 从文件中读取配置，计算分数" << std::endl;
        } else if (arg == "--no-mp") {
            mp = false;
        } else if (arg == "--seed") {
            seed = atoi(argv[++i]);
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
    SARunner::init(T_MAX_CHEF, T_MAX_RECIPE, ITER_CHEF, ITER_RECIPE,
                   targetScore);
    bool silent = false;
    int log = 0; // 0x0: 无输出 0x1: 正常输出 0x10: 详细输出
    int seed = (int)(time(NULL) * 100);
    bool calculate = false;
    bool mp = true;
    parseArgs(argc, argv, silent, log, calculate, mp, seed);
    auto [usrData, gameData, ruleData] = loadJsonFile();
    CList chefList;
    RList recipeList;
    RuleInfo rl;
    try {
        std::cout << "正在读取文件..." << std::endl;
        loadBanquetRule(rl, ruleData, true);
        loadRecipe(recipeList, usrData, gameData);
        loadChef(chefList, usrData, gameData);
        std::cout << "读取文件成功。" << std::endl;
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
    int totalScore = 0;
    int num_threads = std::thread::hardware_concurrency();

        Result result;

        if (!mp) {
            num_threads = 1;
        }
        // num_threads = 1;
        // seed = 1509184356;
        MultiThreadProgressBar::getInstance(num_threads);
        std::cout << "启用" << num_threads
                  << "线程，建议期间不要离开窗口，否则可能影响速度。"
                  << std::endl;

        std::vector<std::future<Result>> futures;

        for (int i = 0; i < num_threads; i++) {

            futures.push_back(std::async(
                std::launch::async, run, i, std::ref(rl), std::ref(chefList),
                std::ref(recipeList), 0, silent, seed++));
            silent = true;
        }
        // std::cout << "分数：";
        int max_score = 0;
        for (auto &future : futures) {
            Result tmp = future.get();
            totalScore += tmp.score;
            if (tmp.score > max_score) {
                if (max_score != 0) {
                    delete result.chefList;
                    delete result.state;
                }
                result = tmp;
                max_score = result.score;
            } else {
                delete tmp.chefList;
                delete tmp.state;
            }
            // std::cout << tmp.score << " ";
        }
        std::cout << "\n最佳结果：" << std::endl;

        log += 0x1;
        // log += 0x10;

        std::cout << "随机种子：" << result.seed << std::endl;
        exactChefTool(rl, *result.state);
        sumPrice(rl, *result.state, log);
        std::cout << "**************\n总分: " << result.score
                  << "\n***************" << std::endl;
        if (!silent) {
            SARunner saRunnerPrint(&rl, result.chefList, &result.recipeList,
                                   false, f::t_dist_fast);
            saRunnerPrint.run(result.state, false, silent, "../out/recipe");
        }
        delete result.chefList;
        delete result.state;

    end = clock();
    std::cout << "用时：" << (double)(end - start) / CLOCKS_PER_SEC << "秒"
              << std::endl;
#ifdef VIS_HISTORY
    std::cout << "均分：" << totalScore / num_threads << std::endl;
#endif
    for (auto &chef : chefList) {
        chef.deletePointers();
    }
    for (auto iter = Skill::skillList.begin(); iter != Skill::skillList.end();
         iter++) {
        auto &s = iter->second;
        for (auto &ce : s.conditionalEffects) {
            delete ce;
        }
    }
}
Result run(int threadId, const RuleInfo &rl, const CList &chefList,
           RList &recipeList, int log, bool silent, int seed) {
    CList *chefListPtr = new CList(chefList);
    *chefListPtr = chefList;
    for (auto &chef : *chefListPtr) {
        chef.recipeLearned = new std::vector<Recipe *>;
    }
    srand(seed);
    SARunner saRunner(&rl, chefListPtr, &recipeList, true, f::t_dist_slow,
                      threadId);
    // std::cout << log << std::endl;
    States *s = new States;
    *s = saRunner.run(NULL, true, false);
    // *s = perfectChef(*s, chefListPtr);
    int score = sumPrice(rl, *s, log);
    for (auto &chef : *chefListPtr) {
        delete chef.recipeLearned;
    }
    return Result{score, seed, chefListPtr, recipeList, s};
}

/**
 * @return userData, gameData
 */
std::tuple<Json::Value, Json::Value, Json::Value> loadJsonFile() {
    Json::Value usrData;
    Json::Value gameData;
    Json::Value ruleData;

    auto dirs = {"./", "../data/", "../../data/"};
    // std::ifstream gameDataF("../data/data.min.json", std::ifstream::binary);
    // std::ifstream usrDataF("../data/userData.json", std::ifstream::binary);

    std::ifstream gameDataF, usrDataF, ruleDataF;
    std::map<std::string, std::ifstream &> files = {{"data.min", gameDataF},
                                                    {"userData", usrDataF},
                                                    {"ruleData", ruleDataF}};
    for (auto &file : files) {
        for (const std::string &dir : dirs) {
            file.second.open(dir + file.first + ".json", std::ifstream::binary);
            if (file.second.good()) {
                break;
            }
            file.second.close();
        }
    }

    if (!usrDataF.good() || !gameDataF.good() || !ruleDataF.good()) {
        // get current working dir
#ifdef _WIN32
        char buf[256];
        _getcwd(buf, 256);
#else
        char buf[256];
        getcwd(buf, 256);
#endif
        std::cout << "当前工作目录：" << buf << std::endl;
        std::cout
            << "json文件有缺失。如果在网页端，请确认已经上传了文件；如果在"
               "本地，请确认已经data.min.json、userData.json和ruleData."
               "json三个文件已经在工作目录下。\n";
        exit(1);
    }

    try {
        usrDataF >> usrData;
        usrDataF.close();
        gameDataF >> gameData;
        gameDataF.close();
        ruleDataF >> ruleData;
        ruleDataF.close();

    } catch (Json::RuntimeError &e) {

        std::cout
            << "json文件格式不正确。如果文件内容是手动复制的，确认文件已"
               "经复制完整。如果文件是从powershell下载的，请确认编码是utf-8\n";
        exit(1);
    } catch (Json::LogicError &e) {
        std::cout << "json文件格式不正确。请确认文件来自白菜菊花而非图鉴网。\n";
        exit(1);
    }
    return {std::move(usrData), std::move(gameData), std::move(ruleData)};
}
