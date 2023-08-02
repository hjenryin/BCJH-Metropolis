#ifndef CHEF_HPP
#define CHEF_HPP

#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Recipe.hpp"
#include "Values.hpp"
#include "Types.hpp"
#include <vector>
#include "../config.hpp"
#include "utils/json.hpp"
class Recipe;

class Chef {
  private:
    static CookAbility globalAbilityBuff;
    static int globalAbilityMale;
    static int globalAbilityFemale;
    void addSkill(int id);
    ToolEnum tool = NOT_EQUIPPED;

  public:
    static bool coinBuffOn;
    bool male;
    bool female;
    int id;
    Skill *skill;
    Skill *companyBuff;
    Skill *nextBuff;
    ToolEnum getTool() const { return this->tool; }
    void NoTool() { this->tool = NO_TOOL; }
    std::string *name;
    std::vector<Recipe *> *recipeLearned;

    static void setGlobalBuff(CookAbility buff) { globalAbilityBuff = buff; }
    static void setGlobalAbilityMale(int ability) {
        globalAbilityMale = ability;
    }
    static void setGlobalAbilityFemale(int ability) {
        globalAbilityFemale = ability;
        // std::cout << "setGlobalAbilityFemale" << ability << std::endl;
    }
    static void setGlobalAbilityAll(int ability) {
        globalAbilityBuff.add(ability);
    }
    static void initBuff(const Json::Value &usrBuff) {
        // std::cout << usrBuff.toStyledString() << std::endl;
        if (MODE == 2) {
            coinBuffOn = false;
        } else {
            coinBuffOn = true;
        }
        setGlobalBuff(CookAbility(usrBuff));
        setGlobalAbilityMale(getInt(usrBuff["Male"]));
        setGlobalAbilityFemale(getInt(usrBuff["Female"]));
        setGlobalAbilityAll(getInt(usrBuff["All"]));
    }
    Chef(Json::Value &v, int ultimateSkillId);
    Chef() { id = -1; }
    void print() const;
    void modifyTool(ToolEnum);
    void deletePointers() {
        delete this->name;
        delete this->skill;
        delete this->companyBuff;
        delete this->nextBuff;
        delete this->recipeLearned;
    }
    // void updateLearnedRecipe(const RecipesInfo rI) const {
    //     auto &recipesLearned = *this->recipesLearned;
    //     auto worst = recipesLearned.end();
    //     int worstPrice = rI.score;
    //     int count = 0;
    //     for (auto iter = recipesLearned.begin(); iter < recipesLearned.end();
    //          iter++) {
    //         if ((iter->full == rI.full) && (iter->score < worstPrice)) {
    //             worst = iter;
    //             worstPrice = iter->score;
    //             count++;
    //         } else if ((iter->full == rI.full) && (iter->score == rI.score))
    //         {
    //             return;
    //         }
    //     }
    //     if (count < RECIPE_CACHE) {
    //         recipesLearned.push_back(rI);
    //     } else if (worst != recipesLearned.end()) {
    //         *worst = rI;
    //     }
    // }
};
typedef std::vector<Chef> CList;
void loadChef(CList &chefList, int chefRarity, const Json::Value &gameData,
              const Json::Value &usrData, bool allowTool);

// void loadChefTools(CList &chefList, CList &newChefList);
std::string getToolName(ToolEnum tool);

#endif