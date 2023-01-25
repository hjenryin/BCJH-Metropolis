#include "functions.hpp"
#include "SARunner.hpp"
#include "../config.hpp"
#include "../rule.hpp"
#include "exception.hpp"

namespace r0 {
States randomRecipe(States &, CList *, RList *, CRPairs *);
States swapRecipe(States &, CList *, RList *, CRPairs *);
States randomChef(States &, CList *, RList *, CRPairs *);
States swapChefTool(States &, CList *, RList *, CRPairs *);
} // namespace r0

int e::getOptimalPrice(States s, CList *chefList, RList *recipeList,
                       CRPairs *chefRecipePairs, bool verbose) {
    // SARunner saRunner(chefList, recipeList, chefRecipePairs, 500, 100, 0,
    //                   e::sumPrice, r::randomRecipe, f::t_dist);
    // saRunner.run(s.chef);
    // return saRunner.bestEnergy;
    return e::sumPrice(s, chefList, recipeList, chefRecipePairs, verbose);
}
bool repeatChef(Chef *chef, Chef *chefs[NUM_CHEFS], int except) {
    for (int i = 0; i < NUM_CHEFS; i++) {
        if (except != i && chef->id == chefs[i]->id) {
            return true;
        }
    }
    return false;
}
States r0::randomChef(States &s, CList *chefList, RList *recipeList,
                      CRPairs *chefRecipePairs) {
    // std::cout << "Test Here" << std::endl;
    int chefNum = rand() % NUM_CHEFS;
    Chef *pChef;
    int count = 0;
    do {
        auto iter = chefList->begin();
        std::advance(iter, rand() % chefList->size());
        pChef = &(iter->second);
        count++;
    } while (repeatChef(pChef, s.chef, chefNum) &&
             count < RANDOM_SEARCH_TIMEOUT);
    if (count >= RANDOM_SEARCH_TIMEOUT) {
        throw NoChefException();
    }
    s.chef[chefNum] = pChef;
    SARunner saRunner(chefList, recipeList, chefRecipePairs, ITER_RECIPE,
                      T_MAX_RECIPE, 0, e::sumPrice, r::randomRecipe,
                      f::t_dist_fast);
    return saRunner.run(s.chef);
}
States r0::swapRecipe(States &s, CList *chefList, RList *r,
                      CRPairs *chefRecipePairs) {
    for (int i = 1; i < 10; i++) {
        int recipeNum1 = rand() % (NUM_CHEFS * DISH_PER_CHEF);
        int recipeNum2 = rand() % (NUM_CHEFS * DISH_PER_CHEF);

        Chef *chef1 = s.chef[recipeNum1 / DISH_PER_CHEF];
        Chef *chef2 = s.chef[recipeNum2 / DISH_PER_CHEF];

        if (chef1 == chef2) {
            swap(s.recipe[recipeNum1], s.recipe[recipeNum2]);
            return s;
        } else {
            bool chef1CanCook =
                chefCanCook(chefRecipePairs, chef1, s.recipe[recipeNum2]);
            bool chef2CanCook =
                chefCanCook(chefRecipePairs, chef2, s.recipe[recipeNum1]);
            if (chef1CanCook && chef2CanCook) {
                swap(s.recipe[recipeNum1], s.recipe[recipeNum2]);
                return s;
            }
        }
    }
    return s;
}

States r0::randomRecipe(States &s, CList *chefList, RList *r,
                        CRPairs *chefRecipePairs) {
    int recipeNum = rand() % (NUM_CHEFS * DISH_PER_CHEF);
    Chef *chef = s.chef[recipeNum / DISH_PER_CHEF];
    Recipe *recipe;
    auto recipes = &(*chefRecipePairs)[chef];
    r00::unrepeatedRandomRecipe(recipes, s.recipe, NUM_CHEFS * DISH_PER_CHEF,
                                recipeNum);
    return s;
}

States r0::swapChefTool(States &s, CList *chefList, RList *recipeList,
                        CRPairs *chefRecipePairs) {
    int chefNum = rand() % NUM_CHEFS;
    int tool = rand() % 6;
    int id = s.chef[chefNum]->id;
    s.chef[chefNum] = &chefList->at(6 * id + tool);
    for (int i = chefNum * DISH_PER_CHEF;
         i < chefNum * DISH_PER_CHEF + DISH_PER_CHEF; i++) {
        if (!s.chef[chefNum]->isCapable(s.recipe[i])) {
            auto rl = &s.chef[chefNum]->recipeCapable;
            r00::unrepeatedRandomRecipe(rl, s.recipe, NUM_CHEFS * DISH_PER_CHEF,
                                        i);
        }
    }

    return s;
}

int e::sumPrice(States s, CList *chefList, RList *recipeList,
                CRPairs *chefRecipePairs, bool verbose) {
    if (BANQUET) {
        BanquetRule rule[9];
        int bestFull = banquetRule(rule, s);
        BanquetInfo bi[9];
        int totalScore = 0;
        int totalFull = 0;
        for (int i = 0; i < 9; i++) {
            if (verbose && i % 3 == 0) {
                std::cout << "************" << std::endl;
                s.chef[i / 3]->print();
                std::cout << "************" << std::endl;
            }
            bi[i] = getPrice(s.chef[i / 3], s.recipe[i], rule[i], verbose);
            totalFull += bi[i].full;
            totalScore += bi[i].price;
        }

        switch (totalFull - bestFull) {
        case 0:
            return std::ceil(totalScore * 1.3);
        default:
            int delta = std::abs(totalFull - bestFull);
            return std::ceil(totalScore * (1 - 0.05 * delta));
        }
    } else {
        int energy = 0;
        int r = 0;

        for (int i = 0; i < NUM_CHEFS; i++) {
            if (verbose) {
                std::cout << "************" << std::endl;
                s.chef[i]->print();
                std::cout << "************" << std::endl;
            }
            for (int j = 0; j < DISH_PER_CHEF; j++) {

                energy += getPrice(*s.chef[i], *s.recipe[r++], verbose);
            }
        }
        return energy;
    }
}
States r::randomRecipe(States s, CList *chefList, RList *recipeList,
                       CRPairs *chefRecipePairs) {
    double r = (double)rand() / RAND_MAX;
    if (r < 0.1) {
        return r0::swapRecipe(s, chefList, recipeList, chefRecipePairs);
    } else {
        return r0::randomRecipe(s, chefList, recipeList, chefRecipePairs);
    }
}
States r::randomChef(States s, CList *chefList, RList *recipeList,
                     CRPairs *chefRecipePairs) {
    double r = (double)rand() / RAND_MAX;
    if (r < 0.1) {
        return r0::swapChefTool(s, chefList, recipeList, chefRecipePairs);
    } else {
        return r0::randomChef(s, chefList, recipeList, chefRecipePairs);
    }
}

void swap(Recipe *&a, Recipe *&b) {
    Recipe *temp = a;
    a = b;
    b = temp;
}
template <typename T> bool inArray(T **array, int size, T *value) {
    for (int i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}
bool chefCanCook(CRPairs *p, Chef *chef, Recipe *recipe) {
    return std::find((*p)[chef].begin(), (*p)[chef].end(), recipe) !=
           (*p)[chef].end();
}
double f::exponential_multiplicative(int stepMax, int step, double tMax,
                                     double tMin) {
    return tMax * std::exp(-step / 1000.0);
}
double f::linear(int stepMax, int step, double tMax, double tMin) {
    return (tMax - tMin) * (1 - step / (double)stepMax) + tMin;
}
double f::t_dist_fast(int stepMax, int step, double tMax, double tMin) {
    return tMax / (1 + step * step / 300000.0);
}
double f::t_dist_slow(int stepMax, int step, double tMax, double tMin) {
    return tMax / (1 + step * step / 1000000.0);
}
double f::linear_mul(int stepMax, int step, double tMax, double tMin) {
    return tMax / (1 + step / 100000.0);
}
double f::zipf(int stepMax, int step, double tMax, double tMin) {
    double t = tMax / std::pow(step + 1, 0.2);
    // std::cout << t << std::endl;
    return t;
}
double f::one_over_n(int stepMax, int step, double tMax, double tMin) {
    return tMax / std::pow(step + 1, 0.1);
}

void r00::unrepeatedRandomRecipe(std::vector<Recipe *> *rl, Recipe **rs,
                                 int size, int index) {
    int count = 0;
    Recipe *r;
    do {
        r = rl->at(rand() % rl->size());
        count++;
    } while (inArray(rs, size, r) && count < RANDOM_SEARCH_TIMEOUT);
    if (count >= RANDOM_SEARCH_TIMEOUT) {
        throw NoRecipeException();
    }
    rs[index] = r;
}