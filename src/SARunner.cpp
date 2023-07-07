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
SARunner::SARunner(CList *chefList, RList *recipeList, int stepMax, int tMax,
                   int tMin, e::GetEnergy getEnergyFunc,
                   r::RandomMove randomMoveFunc,
                   f::CoolingSchedule coolingScheduleFunc) {
    this->chefList = chefList;
    this->recipeList = recipeList;
    this->randomMoveFunc = randomMoveFunc;
    this->coolingScheduleFunc = coolingScheduleFunc;
    this->stepMax = stepMax;
    this->tMax = tMax;
    this->tMin = tMin;
    this->history = new History[stepMax];
    this->getEnergyFunc = getEnergyFunc;
#ifdef SEARCH_TARGET_SCORE
    // if (MODE != 2) {
    //     std::cout << "config.hpp中改了不该改的东西，请改回来" << std::endl;
    //     exit(1);
    // }
    this->targetScore = SEARCH_TARGET_SCORE;
#else
    this->targetScore = INT_MAX;
#endif
}
SARunner::~SARunner() { delete[] this->history; }
States SARunner::generateStates(CList *chefList, Chef *chefs[NUM_CHEFS]) {
    States s;

    // std::cout << chefs << std::endl;
    // std::cout << chefList->size() << " " << chefRecipePairs->size()
    // << std::endl;
    if (chefs == NULL) {
        if (chefList->size() == 0) {
            throw NoChefException();
        }
        for (int j = 0; j < NUM_CHEFS; j++) {
            int count = 0;
            do {
                s.chef[j] = &chefList->at(rand() % chefList->size());
                count++;
            } while (inArray(s.chef, j, s.chef[j]) &&
                     count < RANDOM_SEARCH_TIMEOUT);
            if (count >= RANDOM_SEARCH_TIMEOUT) {
                throw NoChefException();
            }
        }
    } else {
        for (int i = 0; i < NUM_CHEFS; i++) {
            s.chef[i] = chefs[i];
        }
    }
    int r = 0;
    for (int j = 0; j < NUM_CHEFS; j++) {
        auto recipeList = &s.chef[j]->recipeCapable;
        for (int i = 0; i < DISH_PER_CHEF; i++) {
            int count = 0;
            do {
                s.recipe[r] = recipeList->at(rand() % recipeList->size());
                count++;
            } while (inArray(s.recipe, r, s.recipe[r]) &&
                     count < RANDOM_SEARCH_TIMEOUT);
            if (count >= RANDOM_SEARCH_TIMEOUT) {
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
            s = generateStates(this->chefList, NULL);
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
    int energy = getEnergyFunc(s, this->chefList, this->recipeList, false);
    // std::cout << "Initial energy: " << energy << std::endl;
    this->bestState = s;
    this->bestEnergy = energy;
    int step = 0;
    double t = this->tMax;
    // srand(time(NULL));
    while (step < this->stepMax) {
        if (progress) {
            if (silent) {
                // if (step % 500 == 0) {
                //     std::cout << "\r" << step << "/" << this->stepMax
                //               << std::flush;
                // }
            } else {
                std::cout << "\r" << step << "/" << this->stepMax << " - "
                          << this->bestEnergy << std::flush;
            }
        }
        States newS;
        try {
            newS = randomMoveFunc(s, this->chefList, this->recipeList);

        } catch (NoRecipeException &e) {
            std::cout << e.what() << std::endl;
            break;
        } catch (NoChefException &e) {
            std::cout << e.what() << std::endl;
            break;
        }
        // std::cin >> step;
        // print(newS);
        int newEnergy =
            getEnergyFunc(newS, this->chefList, this->recipeList, false);
        double prob = 0;
        int delta = energy - newEnergy;
        if (delta / t < -30) {
            prob = 1.01;
        } else {
            // prob = 1.0 / (1 + std::exp(delta / (3 * t + 0.0)));
            prob = std::exp(-delta / t);
        }
        if (prob > (double)rand() / RAND_MAX) {
            s = newS;
            // print(s);
            // std::cout << newEnergy << std::endl;
            energy = newEnergy;
        }
        if (energy > this->bestEnergy) {
            this->bestEnergy = energy;
            for (int i = 0; i < NUM_CHEFS; i++) {
                s.toolCKPT[i] = s.chef[i]->getTool();
            }
            this->bestState = s;
            if (progress && !silent) {

                int tmp = e0::sumPrice(this->bestState);
                std::cout << " ";
            }
        }
        t = coolingScheduleFunc(this->stepMax, step, this->tMax, this->tMin);
        if (t <= this->tMin) {
            break;
        }
        this->history[step].energy = energy;
        this->history[step].t = t;
        this->history[step].states = s;
        if (energy >= this->targetScore) {
            break;
        }
        step++;
    }
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

    return this->bestState;
}

void SARunner::print(States s, bool verbose) {
    int r = 0;
    for (int i = 0; i < NUM_CHEFS; i++) {

        std::cout << "Chef: " << s.chef[i]->name << std::endl << "Recipe ";
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
        this->getEnergyFunc(s, this->chefList, this->recipeList, true);
    }
}
