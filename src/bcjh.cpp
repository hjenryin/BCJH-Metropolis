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
bool Chef::coinBuffOn = true;
void initChefRecipePairs(CRPairs &, std::map<int, Chef> &,
                         std::map<int, Recipe> &);
int main() {
    char buffer[256];
    char *val = getcwd(buffer, sizeof(buffer));
    if (val) {
        std::cout << buffer << std::endl;
    }
    srand(time(NULL));
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
    States s = saRunner.run(NULL, false, true);
    std::cout << std::endl;
    saRunner.print(s);
    std::cout << "Score: "
              << e::sumPrice(s, &chefList, &recipeList, &chefRecipePairs)
              << std::endl;

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
    std::cout << chefRecipePairs.size() << std::endl;
}