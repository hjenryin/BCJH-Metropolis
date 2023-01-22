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
SARunner::SARunner(CList *chefList, RList *recipeList, CRPairs *chefRecipePairs,
                   int stepMax, int tMax, int tMin, e::GetEnergy getEnergyFunc,
                   r::RandomMove randomMoveFunc,
                   f::CoolingSchedule coolingScheduleFunc) {
    this->chefList = chefList;
    this->recipeList = recipeList;
    this->chefRecipePairs = chefRecipePairs;
    this->randomMoveFunc = randomMoveFunc;
    this->coolingScheduleFunc = coolingScheduleFunc;
    this->stepMax = stepMax;
    this->tMax = tMax;
    this->tMin = tMin;
    this->history = new History[stepMax];
    this->getEnergyFunc = getEnergyFunc;
}
States SARunner::generateStates(CList *chefList, CRPairs *chefRecipePairs,
                                Chef *chefs[NUM_CHEFS]) {
    States s;
    int r = 0;
    // std::cout << chefs << std::endl;
    // std::cout << chefList->size() << " " << chefRecipePairs->size()
    // << std::endl;
    if (chefs == NULL) {
        for (int j = 0; j < NUM_CHEFS; j++) {
            auto iter = chefList->begin();
            std::advance(iter, rand() % chefList->size());
            s.chef[j] = &iter->second;
        }
    } else {
        for (int i = 0; i < NUM_CHEFS; i++) {
            Chef *chef = chefs[i];
            // if (chefList->find(chef->id) == chefList->end()) {

            //     std::cout << "Chef not found:" << chef->id << "in"
            //               << chefList->size() << std::endl;
            //     auto iter = chefList->begin();

            //     exit(1);
            // }
            s.chef[i] = chef;
        }

        // s.chef = chefId;
    }

    for (int j = 0; j < NUM_CHEFS; j++) {
        auto recipeList = &(*chefRecipePairs)[s.chef[j]];
        // std::cout << j << " " << recipeList->size() << std::endl;
        for (int i = 0; i < DISH_PER_CHEF; i++) {

            s.recipe[r++] = (*recipeList)[rand() % recipeList->size()];
        }
    }

    // std::cout << "******\nInitial State: \n";
    // print(s);
    // std::cout << "************" << std::endl;
    return s;
}

States SARunner::run(Chef *chefs[NUM_CHEFS], bool verbose, bool progress) {
    // std::cout << "Here" << std::endl;
    States s = generateStates(this->chefList, this->chefRecipePairs, chefs);
    int energy = getEnergyFunc(s, this->chefList, this->recipeList,
                               this->chefRecipePairs, false);
    // std::cout << "Initial energy: " << energy << std::endl;
    this->bestState = s;
    this->bestEnergy = energy;
    int step = 0;
    double t = this->tMax;
    // srand(time(NULL));
    while (step < this->stepMax) {
        if (progress) {
            std::cout << "\r" << step << "/" << this->stepMax << std::flush;
        }
        States newS = randomMoveFunc(s, this->chefList, this->recipeList,
                                     this->chefRecipePairs);
        // print(newS);
        int newEnergy = getEnergyFunc(newS, this->chefList, this->recipeList,
                                      this->chefRecipePairs, false);
        double prob = 0;
        int delta = energy - newEnergy;
        if (delta / t > 30) {
            prob = 0;
        } else {
            prob = 1.0 / (1 + std::exp(delta / (3 * t + 0.0)));
            //  prob = std::exp((newEnergy - energy) / (t + 0.0));
        }
        if (prob > (double)rand() / RAND_MAX) {
            s = newS;
            // print(s);
            // std::cout << newEnergy << std::endl;
            energy = newEnergy;
        }
        if (energy > this->bestEnergy) {
            this->bestEnergy = energy;
            this->bestState = s;
        }
        t = coolingScheduleFunc(this->stepMax, step, this->tMax, this->tMin);
        if (t <= this->tMin) {
            break;
        }
        this->history[step].energy = energy;
        this->history[step].t = t;
        this->history[step].states = s;
        step++;
    }
    // std::cout << " * Best energy: " << this->bestEnergy << std::endl;
    // print(this->bestState, verbose);
    if (progress) {
        std::fstream file;
        file.open("../out/history.csv", std::ios::out);
        for (int i = 0; i < step; i++) {
            file << this->history[i].energy << "," << this->history[i].t
                 << std::endl;
        }
        file.close();
        // std::cout <<
        system("python3 ../src/plot.py");
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
        this->getEnergyFunc(s, this->chefList, this->recipeList,
                            this->chefRecipePairs, true);
    }
}
