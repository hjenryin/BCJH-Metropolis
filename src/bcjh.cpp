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
bool Chef::coinBuffOn = true;
void initChefRecipePairs(CList &, RList &);
int run(CList &, RList &, int, bool);
void calculator(CList &, RList &);

void parseArgs(int argc, char *argv[], bool &silent, int &log,
               bool &calculate) {
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
        }
    }
}

int main(int argc, char *argv[]) {

    bool silent = false;
    int log = 0; // 0x0: 无输出 0x1: 正常输出 0x10: 详细输出
    int seed = (int)time(NULL);
    bool calculate = false;
    parseArgs(argc, argv, silent, log, calculate);
    // seed = 1687004120;
    if (true)
        std::cout << "随机种子：" << seed << std::endl;
    srand(seed);
    CList chefList;
    RList recipeList;
    try {
        std::cout << "正在读取文件..." << std::endl;
        loadChef(chefList);
        loadRecipe(recipeList);
        std::cout << "读取文件成功。" << std::endl;
    } catch (FileNotExistException &e) {
        std::cout << "json文件确实。如果在网页端，请确认已经上传了文件；如果在"
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
        int s = 0;
        // for (int i = 0; i < 9; i++) {
        // s = run(chefList, recipeList, log, silent);
        // }
        // do{
        s = run(chefList, recipeList, log, silent);
        // } while (s < 1210000);
    } else {
        calculator(chefList, recipeList);
    }
    end = clock();
    std::cout << "用时：" << (double)(end - start) / CLOCKS_PER_SEC << "秒"
              << std::endl;
}
int run(CList &chefList, RList &recipeList, int log, bool silent) {
    SARunner saRunner(&chefList, &recipeList, ITER_CHEF, T_MAX_CHEF, 0,
                      e::getTotalPrice, r::randomChef, f::t_dist_slow);
    // std::cout << log << std::endl;
    States s = saRunner.run(NULL, true, silent);
    s = perfectTool(s);
    std::cout << std::endl;
    log += 0x1;
    int score = e0::sumPrice(s, &chefList, &recipeList, log, true);
    std::cout << "**************\nScore: " << score << "\n***************"
              << std::endl;
    if (!silent) {
        SARunner saRunnerPrint(&chefList, &recipeList, ITER_RECIPE,
                               T_MAX_RECIPE, 0, e::getTotalPrice,
                               r::randomRecipe, f::t_dist_fast);
        saRunnerPrint.run(&s, false, silent, "../out/recipe");
    }
    return score;
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
