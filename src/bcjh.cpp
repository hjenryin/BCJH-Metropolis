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
bool Chef::coinBuffOn = true;
void initChefRecipePairs(CRPairs &, std::map<int, Chef> &,
                         std::map<int, Recipe> &);
int main(int argc, char *argv[]) {
    int opt;
    bool silent = false;
    bool verbose = false;
    const char *optstring = "sv";
    int seed = time(NULL);
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        if (opt == 's') {
            silent = true;
        }
        if (opt == 'v') {
            verbose = true;
        }
    }
    // seed = 1674678021;
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
    return 0;
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