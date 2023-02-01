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
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
bool Chef::coinBuffOn = true;
void initChefRecipePairs(CRPairs &, std::map<int, Chef> &,
                         std::map<int, Recipe> &);
int run(CList &, RList &, CRPairs &, int, bool);
void calculator(CList &, RList &, CRPairs &);
int main(int argc, char *argv[]) {
    int opt;
    bool silent = false;
    int log = 0; // 0x0: 无输出 0x1: 正常输出 0x10: 详细输出
    const char *optstring = "svch";
    int seed = time(NULL);
    bool calculate = false;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        if (opt == 's') {
            silent = true;
        }
        if (opt == 'v') {
            log += 0x10;
        }
        if (opt == 'c') {
            calculate = true;
        }
        if (opt == 'h') {
            std::cout << "-s: 无进度条" << std::endl;
            std::cout << "-v: 输出详细信息" << std::endl;
            std::cout << "-c: 从文件中读取配置，计算分数" << std::endl;
        }
    }
    // seed = 1675055589;
    if (true)
        std::cout << "随机种子：" << seed << std::endl;
    srand(seed);
    std::map<int, Chef> chefList0, chefList;
    std::map<int, Recipe> recipeList;
    try {
        loadChef(chefList0);
        loadRecipe(recipeList);
        loadChefTools(chefList0, chefList);
    } catch (Json::RuntimeError &e) {
        std::cout << "json文件格式不正确。请确认：1. "
                     "已经上传了文件。2."
                     "如果文件内容是手动复制的，确认文件已经复制完整。\n";
        exit(1);
    } catch (Json::LogicError &e) {
        std::cout << "json文件格式不正确。请确认文件来自白菜菊花而非图鉴网。\n";
        exit(1);
    }

    CRPairs chefRecipePairs;
    initChefRecipePairs(chefRecipePairs, chefList, recipeList);
    for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
        chef->second.loadRecipeCapable(recipeList);
    }
    if (!calculate) {
        int s = 0;
        // do {
        s = run(chefList, recipeList, chefRecipePairs, log, silent);
        // } while (s < 660000);
    } else {
        calculator(chefList, recipeList, chefRecipePairs);
    }
}
int run(CList &chefList, RList &recipeList, CRPairs &chefRecipePairs, int log,
        bool silent) {
    SARunner saRunner(&chefList, &recipeList, &chefRecipePairs, ITER_CHEF,
                      T_MAX_CHEF, 0, e::getTotalPrice, r::randomChef,
                      f::t_dist_fast);
    // std::cout << log << std::endl;
    States s = saRunner.run(NULL, true, silent);

    std::cout << std::endl;
    log += 0x1;
    int score =
        e0::sumPrice(s, &chefList, &recipeList, &chefRecipePairs, log, true);
    std::cout << "**************\nScore: " << score << "\n***************"
              << std::endl;
    if (!silent) {
        SARunner saRunnerPrint(&chefList, &recipeList, &chefRecipePairs,
                               ITER_RECIPE, T_MAX_RECIPE, 0, e::getTotalPrice,
                               r::randomRecipe, f::t_dist_fast);
        saRunnerPrint.run(s.chef, false, silent, "../out/recipe");
    }
    return score;
}

void initChefRecipePairs(CRPairs &chefRecipePairs,
                         std::map<int, Chef> &chefList,
                         std::map<int, Recipe> &recipeList) {
    for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
        for (auto recipe = recipeList.begin(); recipe != recipeList.end();
             recipe++) {
            int price = getPrice(chef->second, recipe->second);
            if (price > 0) {
                // std::cout << &(chef->second) << std::endl;
                chefRecipePairs[&chef->second].push_back(&recipe->second);
            }
        }
        if (chefRecipePairs[&chef->second].size() == 0) {
            chefRecipePairs.erase(&chef->second);
        }
    }
    // std::cout << chefRecipePairs.size() << std::endl;
}
void calculator(CList &chefList, RList &recipeList, CRPairs &p) {
    std::ifstream f;
    f.open("../in/out.txt");
    States s;
    for (int i = 0; i < NUM_CHEFS; i++) {
        int c, t;
        f >> c >> t;
        s.chef[i] = &chefList[c * 6 + t];
    }
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        int r;
        f >> r;
        s.recipe[i] = &recipeList[r];
    }
    f.close();
    int score = e0::sumPrice(s, &chefList, &recipeList, &p, false, true);
    SARunner saRunner(&chefList, &recipeList, &p, ITER_CHEF, T_MAX_CHEF, 0);
    saRunner.print(s, true);
    std::cout << "\n\nScore: " << score << std::endl;
}