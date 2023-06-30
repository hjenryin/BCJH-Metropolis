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

void parseArgs(int argc, char *argv[], bool &silent, int &log, bool &calculate,
               bool &mp, int &seed) {
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
    parseArgs(argc, argv, silent, log, calculate, mp, seed);

    CList chefList;
    RList recipeList;
    try {
        std::cout << "正在读取文件..." << std::endl;
        loadChef(chefList);
        loadRecipe(recipeList);
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

    if (!calculate) {
        Result result;

        int num_threads = std::thread::hardware_concurrency();
        if (!mp) {
            num_threads = 1;
        }
        seed *= num_threads;
        std::cout << "启用" << num_threads
                  << "线程，建议期间不要离开窗口，否则可能影响速度。"
                  << std::endl;

        std::vector<std::future<Result>> futures;

        for (int i = 0; i < num_threads; i++) {

            futures.push_back(
                std::async(std::launch::async, run, std::ref(chefList),
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
        int score = e0::sumPrice(*result.state, result.chefList,
                                 &result.recipeList, log, true);
        std::cout << "**************\n总分: " << result.score
                  << "\n***************" << std::endl;
        if (!silent) {
            SARunner saRunnerPrint(
                result.chefList, &result.recipeList, ITER_RECIPE, T_MAX_RECIPE,
                0, e::getTotalPrice, r::randomRecipe, f::t_dist_fast);
            saRunnerPrint.run(result.state, false, silent, "../out/recipe");
        }
        delete result.chefList;
        delete result.state;
    } else {
        calculator(chefList, recipeList);
    }
    end = clock();
    std::cout << "用时：" << (double)(end - start) / CLOCKS_PER_SEC << "秒"
              << std::endl;
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

void calculator(CList &chefList, RList &recipeList) {
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
    int score = e0::sumPrice(s, &chefList, &recipeList, false, true);
    SARunner saRunner(&chefList, &recipeList, ITER_CHEF, T_MAX_CHEF, 0);
    saRunner.print(s, true);
    std::cout << "\n\nScore: " << score << std::endl;
}
