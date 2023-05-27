#include "functions.hpp"
#include "SARunner.hpp"
#include "../config.hpp"
#include "banquetRule.hpp"
#include "exception.hpp"
#include "activityRule.hpp"

namespace r0 {
States randomRecipe(States &, CList *, RList *, CRPairs *);
States swapRecipe(States &, CList *, RList *, CRPairs *);
States randomChef(States &, CList *, RList *, CRPairs *);
States swapChefTool(States &, CList *, RList *, CRPairs *);
} // namespace r0

int e::getTotalPrice(States s, CList *chefList, RList *recipeList,
                     CRPairs *chefRecipePairs, bool verbose) {
    return e0::sumPrice(s, chefList, recipeList, chefRecipePairs, verbose,
                        false);
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
                      T_MAX_RECIPE, 0, e::getTotalPrice, r::randomRecipe,
                      f::t_dist_slow);
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
/**
 * @brief Warning: this function involves a large copy constructor.
 *
 * @return whether after deduction, the price is still the same
 */
bool deductTool(States s, CList *chefList, RList *recipeList,
                CRPairs *chefRecipePairs, int chefId, int deduction) {
    Chef chef(*s.chef[chefId]);
    int tool = chef.tool;
    int *cookAbility;
    switch (tool) {
    case STIRFRY:
        cookAbility = &chef.skill.ability.stirfry;
        break;
    case BOIL:
        cookAbility = &chef.skill.ability.boil;
        break;
    case FRY:
        cookAbility = &chef.skill.ability.fry;
        break;
    case STEAM:
        cookAbility = &chef.skill.ability.steam;
        break;
    case BAKE:
        cookAbility = &chef.skill.ability.bake;
        break;
    case KNIFE:
        cookAbility = &chef.skill.ability.knife;
        break;
    case NO_TOOL:
        return true;
    default:
        throw "Toolerror";
    }
    *cookAbility -= deduction;
    int bestPrice =
        e0::sumPrice(s, chefList, recipeList, chefRecipePairs, false, false);
    States newState = s;
    newState.chef[chefId] = &chef;
    int newPrice = e0::sumPrice(newState, chefList, recipeList, chefRecipePairs,
                                false, false);
    return newPrice == bestPrice;
}
/**
 * @brief
 * @param exactChefTool whether to use the exact tool deduction.Warning: it
 * should only be set true at the end of the function as it modifies the name of
 * the chefs.
 */
int e0::sumPrice(States s, CList *chefList, RList *recipeList,
                 CRPairs *chefRecipePairs, int log, bool exactChefTool) {
    if (exactChefTool) {
        // std::cout << "exactChefTool" << std::endl;
        for (int i = 0; i < NUM_CHEFS; i++) {
            if (deductTool(s, chefList, recipeList, chefRecipePairs, i, 40)) {
                if (deductTool(s, chefList, recipeList, chefRecipePairs, i,
                               70)) {
                    if (deductTool(s, chefList, recipeList, chefRecipePairs, i,
                                   100)) {
                        s.chef[i]->name += "(0)";
                    } else {
                        s.chef[i]->name += "(30)";
                    }
                } else {
                    s.chef[i]->name += "(60)";
                }
            } else {
                s.chef[i]->name += "(100)";
            }
        }
    }
    if (MODE == 1) {
        BanquetRule rule[NUM_CHEFS * DISH_PER_CHEF];
        int bestFull[NUM_GUESTS];
        banquetRule(rule, s, bestFull);
        BanquetInfo bi[NUM_CHEFS * DISH_PER_CHEF];
        int totalScore = 0;
        int totalFull = 0;
        int scoreCache = 0;
        int fullCache = 0;
        int ans = 0;
        int d = 0;
        int d2 = 0;
        for (int g = 0; g < NUM_GUESTS; g++) {
            d = DISH_PER_CHEF * CHEFS_PER_GUEST * g;
            d2 = CHEFS_PER_GUEST * g;
            totalScore = 0;
            totalFull = 0;
            scoreCache = 0;
            fullCache = 0;
            for (int i = 0; i < 9; i++) {
                if ((log & 0x10) && i % 3 == 0) {
                std::cout << "VERBOSE************" << std::endl;
                s.chef[d2 + i / 3]->print();
                std::cout << "************" << std::endl;
                }
                bi[d + i] = getPrice(s.chef[d2 + i / 3], s.recipe[d + i], rule[d + i], (log & 0x10));
                totalFull += bi[d + i].full;
                totalScore += bi[d + i].price;
                scoreCache += bi[d + i].price;
                fullCache += bi[d + i].full;
                if ((log & 0x1) && i % 3 == 2) {
                    std::cout << "厨师：" << s.chef[d2 + i / 3]->name << " -> "
                            << fullCache << " / " << scoreCache << std::endl;
                    scoreCache = 0;
                    fullCache = 0;
                    std::cout << "菜谱：" << s.recipe[d + i - 2]->name << "；"
                            << s.recipe[d + i - 1]->name << "；" << s.recipe[d + i]->name
                            << std::endl;
                }
            } 
            switch (totalFull - bestFull[g]) {
            case 0:
                ans += std::ceil(totalScore * 1.3);
                break;
            default:
                int delta = std::abs(totalFull - bestFull[g]);
                ans += std::ceil(totalScore * (1 - 0.05 * delta));
            }           
        }
        return ans;
    } else if (MODE == 2 || MODE == 0) {
        ActivityBuff activityBuff;
        auto p = &activityBuff;
        activityRule(p);
        if (MODE == 0)
            p = NULL;
        int energy = 0;
        int r = 0;
        for (int i = 0; i < NUM_CHEFS; i++) {
            if ((log & 0x10)) {
                std::cout << "VERBOSE************" << std::endl;
                s.chef[i]->print();
                std::cout << "************" << std::endl;
            }
            int scoreCache = 0;
            if (log & 0x1)
                std::cout << "厨师：" << s.chef[i]->name << std::endl
                          << "菜谱：";
            for (int j = 0; j < DISH_PER_CHEF; j++) {
                if (log & 0x1)
                    std::cout << s.recipe[r]->name << "；";
                scoreCache +=
                    getPrice(*s.chef[i], *s.recipe[r++], p, (log & 0x10));
            }
            energy += scoreCache;
            if (log & 0x1)
                std::cout << " -> " << scoreCache << std::endl;
        }

        return energy;
    } else {
        std::cout
            << "config.hpp中MODE设置错误。0为正常营业，1为宴会，2为限时任务"
            << std::endl;
        exit(1);
    }
}
States r::randomRecipe(States s, CList *chefList, RList *recipeList,
                       CRPairs *chefRecipePairs) {
    double r = (double)rand() / RAND_MAX;
    double p_randomRecipe = 1;
    if (MODE == 1)
        p_randomRecipe = 0.9;
    if (r > p_randomRecipe) {
        return r0::swapRecipe(s, chefList, recipeList, chefRecipePairs);
    } else {
        return r0::randomRecipe(s, chefList, recipeList, chefRecipePairs);
    }
}
States r::randomChef(States s, CList *chefList, RList *recipeList,
                     CRPairs *chefRecipePairs) {
    double r = (double)rand() / RAND_MAX;
    double p_randomChef = 0.9;
    if (MODE == 1)
        p_randomChef = 0.85;
    if (r < 0.1) {
        return r0::swapChefTool(s, chefList, recipeList, chefRecipePairs);
    } else if (r >= 1 - p_randomChef) {
        return r0::randomChef(s, chefList, recipeList, chefRecipePairs);
    } else { // swap Chef
        int chefIndex1 = rand() % NUM_CHEFS;
        int chefIndex2;
        do {
            chefIndex2 = rand() % NUM_CHEFS;
        } while (chefIndex1 == chefIndex2);
        States newS = s;
        newS.chef[chefIndex1] = s.chef[chefIndex2];
        newS.chef[chefIndex2] = s.chef[chefIndex1];
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            newS.recipe[chefIndex1 * DISH_PER_CHEF + i] =
                s.recipe[chefIndex2 * DISH_PER_CHEF + i];
            newS.recipe[chefIndex2 * DISH_PER_CHEF + i] =
                s.recipe[chefIndex1 * DISH_PER_CHEF + i];
        }
        return newS;
    }
}

void swap(Recipe *&a, Recipe *&b) {
    Recipe *temp = a;
    a = b;
    b = temp;
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