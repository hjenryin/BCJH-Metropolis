#include <iostream>
#include "functions.hpp"
#include "SARunner.hpp"
#include <map>
#include <vector>
#include "Chef.hpp"
#include "Recipe.hpp"
#include <cmath>
#include <random>
#include <fstream>
#include "exception.hpp"
// #include "activityRule.hpp"
#include <limits.h>
#include "Randomizer.hpp"
#include "banquetRuleGen.hpp"
#include "utils/ProgressBar.hpp"

int SARunner::T_MAX_CHEF, SARunner::T_MAX_RECIPE, SARunner::iterChef,
    SARunner::iterRecipe, SARunner::targetScore;
SARunner::SARunner(const RuleInfo *rl, CList *chefList, RList *recipeList,
                   bool randomizeChef, f::CoolingSchedule coolingScheduleFunc,
                   int threadId)
    : threadId(threadId), rl(rl), coolingScheduleFunc(coolingScheduleFunc) {
    if (randomizeChef) {
        this->randomMoveFunc =
            new ChefRandomizer(chefList, recipeList, rl, targetScore);
        this->tMax = T_MAX_CHEF;
        this->tMin = T_MAX_CHEF / 10;
        this->stepMax = iterChef;
    } else {
        this->randomMoveFunc = new RecipeRandomizer(chefList, recipeList, rl);
        this->tMax = T_MAX_RECIPE;
        this->tMin = T_MAX_RECIPE / 10;
        this->stepMax = iterRecipe;
    }
#ifdef VIS_HISTORY
    this->history = new History[stepMax];
#endif
}

States SARunner::generateStates(Chef *chefs[NUM_CHEFS]) {
    States s;
    CList *chefList = this->randomMoveFunc->c;

    // std::cout << chefs << std::endl;
    // std::cout << chefList->size() << " " << chefRecipePairs->size()
    // << std::endl;
    if (chefs == NULL) {
        if (chefList->size() == 0) {
            throw NoChefException();
        }
        for (int j = 0; j < NUM_CHEFS; j++) {
            int count = 0;
            Chef *randomChef;
            do {
                randomChef = &chefList->at(rand() % chefList->size());
                s.setChef(j, *randomChef);
                count++;
            } while (s.repeatedChef(randomChef, j) &&
                     count < RANDOM_SEARCH_TIMEOUT);
            if (count >= RANDOM_SEARCH_TIMEOUT) {

                throw NoChefException();
            }
        }

    } else {
        for (int i = 0; i < NUM_CHEFS; i++) {
            s.setChef(i, *chefs[i]);
        }
    }

    int r = 0;
    RList *recipeList = this->randomMoveFunc->r;

    for (int j = 0; j < NUM_CHEFS; j++) {
        auto &skill = s.getCookAbilities()[j];
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            int count = 0;
            Recipe *newRecipe;
            do {
                newRecipe = &recipeList->at(rand() % recipeList->size());
                count++;
            } while (((skill.ability / newRecipe->cookAbility == 0) ||
                      inArray(s.recipe, r, newRecipe)) &&
                     count < RANDOM_SEARCH_TIMEOUT * RANDOM_SEARCH_TIMEOUT);
            s.recipe[r] = newRecipe;
            if (count >= RANDOM_SEARCH_TIMEOUT * RANDOM_SEARCH_TIMEOUT) {
                // std::cout << "generate State";
                throw NoRecipeException();
            }
            r++;
        }
    }

    return s;
}
States SARunner::run(States *s0, bool progress, bool silent,
                     const char *filename) {
    States s;
    if (s0 == NULL) {
        try {
            s = generateStates(NULL);
        } catch (NoChefException &e) {
            std::cout << e.what() << std::endl;
            exit(1);
        } catch (NoRecipeException &e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }
    } else {
        s = *s0;
    }
    int energy = sumPrice(*rl, s, false);
    // std::cout << "Initial energy: " << energy << std::endl;
    this->bestState = s;
    this->bestEnergy = energy;
    int step = 0;
    double t = this->tMax;
    // srand(time(NULL));
    int progressSoFar = 0;
    while (step < this->stepMax) {
        if (progress) {
            if (silent) {
                // if (step % 500 == 0) {
                //     std::cout << "\r" << step << "/" << this->stepMax
                //               << std::flush;
                // }
            } else {
                if (step * 10 / stepMax > progressSoFar) {
                    progressSoFar = step * 10 / stepMax;
                    MultiThreadProgressBar::getInstance(0)->print(
                        threadId, progressSoFar * 10,
                        "当前最高分数：" + std::to_string(this->bestEnergy));
                }
            }
        }
        States newS=s;
        try {
            (*randomMoveFunc)(newS);

        } catch (NoRecipeException &e) {
            std::cout << e.what() << std::endl;
            break;
        } catch (NoChefException &e) {
            std::cout << e.what() << std::endl;
            break;
        }
        // std::cin >> step;
        // print(newS);
        int newEnergy = sumPrice(*rl, newS, false);
        double prob = 0;
        int delta = energy - newEnergy;
        if (delta / t < 0) {
            prob = 1.01;
        } else {
            prob = std::exp(-delta / t);
        }
        if (prob > (double)rand() / RAND_MAX) {
            s = newS;
            energy = newEnergy;
        }
        if (energy > this->bestEnergy) {
            this->bestEnergy = energy;
            this->bestState = s;
            if (progress && !silent) {
                sumPrice(*rl, this->bestState);
                std::cout << " ";
            }
        }
        t = coolingScheduleFunc(this->stepMax, step, this->tMax, this->tMin);
        if (t <= this->tMin) {
            break;
        }
#ifdef VIS_HISTORY
        this->history[step].energy = energy;
        this->history[step].t = t;
        this->history[step].states = s;
#endif
        // if (energy >= this->targetScore) {
        //     break;
        // }
        step++;
    }
    if (progress && !silent) {
        MultiThreadProgressBar::getInstance(0)->print(
            threadId, 100, "最高分数：" + std::to_string(this->bestEnergy));
    }
#ifdef VIS_HISTORY
    if (progress && !silent) {
        std::fstream file;
        file.open("../out/history.csv", std::ios::out);
        for (int i = 0; i < step; i++) {
            file << this->history[i].energy << "," << this->history[i].t
                 << std::endl;
        }
        file.close();
        // std::cout << "Saved to ../out/history.csv!" <<std::endl;
        system("python ../src/plot.py &");
    }

    if (filename) {

        std::fstream file;
        std::string fn(filename);
        // std::cout << "Saving to file: " << fn + ".csv" << std::endl;
        file.open(fn + ".csv", std::ios::out);
        for (int i = 0; i < step; i++) {
            file << this->history[i].energy << "," << this->history[i].t
                 << std::endl;
        }
        file.close();
        std::string cmd = "python ../src/plot.py -f " + fn + " &";
        system(cmd.c_str());
    }
#endif
    return this->bestState;
}

void SARunner::print(States s, bool verbose) const {
    int r = 0;
    for (int i = 0; i < NUM_CHEFS; i++) {

        std::cout << "Chef: " << *s.getChefPtr(i)->name << "\nRecipe ";
        for (int j = 0; j < DISH_PER_CHEF; j++) {
            std::cout << j << ": " << s.recipe[r++]->name;
        }
        std::cout << std::endl;
    }
    if (verbose) {
        std::cout << "------verbose------" << std::endl;

        // r = 0;
        // for (int i = 0; i < NUM_CHEFS; i++) {
        //     for (int j = 0; j < DISH_PER_CHEF; i++) {
        //         getPrice(*s.chef[i], *s.recipe[r++], true);
        //     }
        // }
        sumPrice(*rl, s, true);
    }
}
