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
bool Chef::coinBuffOn = true;
void initChefRecipePairs(CRPairs &, std::map<int, Chef> &,
                         std::map<int, Recipe> &);
void run(CList &, RList &, CRPairs &, bool, bool);
void calculator(CList &, RList &, CRPairs &);
int main(int argc, char *argv[]) {
    int opt;
    bool silent = false;
    bool verbose = false;
    const char *optstring = "svch";
    int seed = time(NULL);
    bool calculate = false;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        if (opt == 's') {
            silent = true;
        }
        if (opt == 'v') {
            verbose = true;
        }
        if (opt == 'c') {
            calculate = true;
        }
        if (opt = 'h') {
            std::cout << "-s: 进度条逢500更新" << std::endl;
            std::cout << "-v: 输出详细信息" << std::endl;
            std::cout << "-c: 从文件中读取配置，计算分数" << std::endl;
        }
    }
    // seed = 1674729618;
    if (true)
        std::cout << "随机种子：" << seed << std::endl;
    srand(seed);
    std::map<int, Chef> chefList0, chefList;
    std::map<int, Recipe> recipeList;
    loadChef(chefList0);
    loadRecipe(recipeList);
    loadChefTools(chefList0, chefList);
    CRPairs chefRecipePairs;
    initChefRecipePairs(chefRecipePairs, chefList, recipeList);
    for (auto chef = chefList.begin(); chef != chefList.end(); chef++) {
        chef->second.loadRecipeCapable(recipeList);
    }
    if (!calculate) {
        run(chefList, recipeList, chefRecipePairs, verbose, silent);
    } else {
        calculator(chefList, recipeList, chefRecipePairs);
    }
}
void run(CList &chefList, RList &recipeList, CRPairs &chefRecipePairs,
         bool verbose, bool silent) {
    SARunner saRunner(&chefList, &recipeList, &chefRecipePairs, ITER_CHEF,
                      T_MAX_CHEF, 0, e::getTotalPrice, r::randomChef,
                      f::t_dist_fast);
    States s = saRunner.run(NULL, verbose, true, silent);
    std::cout << std::endl;
    int score =
        e0::sumPrice(s, &chefList, &recipeList, &chefRecipePairs, false, true);

    saRunner.print(s, verbose);
    std::cout << "**************\nScore: " << score << "\n***************"
              << std::endl;
    if (!silent) {
        SARunner saRunnerPrint(&chefList, &recipeList, &chefRecipePairs,
                               ITER_RECIPE, T_MAX_RECIPE, 0, e::getTotalPrice,
                               r::randomRecipe, f::t_dist_fast);
        saRunnerPrint.run(s.chef, false, false, true, "../out/recipe");
    }
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