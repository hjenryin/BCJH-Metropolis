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
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        if (opt == 's') {
            silent = true;
        }
        if (opt == 'v') {
            verbose = true;
        }
    }
    int seed = time(NULL);

    if (verbose)
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
                      T_MAX_CHEF, 0, e::getOptimalPrice, r::randomChef,
                      f::t_dist_slow);
    States s = saRunner.run(NULL, verbose, true, silent);
    std::cout << std::endl;
    saRunner.print(s, verbose);
    std::cout << "Score: "
              << e::sumPrice(s, &chefList, &recipeList, &chefRecipePairs)
              << "\n***************" << std::endl;

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
    }
    // std::cout << chefRecipePairs.size() << std::endl;
}