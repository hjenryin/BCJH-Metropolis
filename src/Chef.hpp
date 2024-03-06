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
#include <map>
class Recipe;
class CList;

class Chef {
  private:
    static CookAbility globalAbilityBuff;
    static int globalAbilityMale;
    static int globalAbilityFemale;
    void addSkill(int id);
    Tool tool;
    static ToolFileType toolFileType;
    std::string name;

  public:
    static bool coinBuffOn;
    bool male;
    bool female;
    int id;
    Skill *skill;
    Skill *companyBuff;
    Skill *nextBuff;
    Tool getTool() const { return this->tool; }
    ToolEnum getToolType() const { return this->tool.type; }
    bool allowsTool() const { return this->tool.type != NO_TOOL; }
    void setNoTool() { this->tool.type = NO_TOOL; }
    std::vector<Recipe *> *recipeLearned = NULL;

    static void setGlobalBuff(CookAbility buff) { globalAbilityBuff = buff; }
    static void setGlobalAbilityMale(int ability) {
        globalAbilityMale = ability;
    }
    static void setGlobalAbilityFemale(int ability) {
        globalAbilityFemale = ability;
        }
    static void setGlobalAbilityAll(int ability) {
        globalAbilityBuff.add(ability);
    }
    static void initBuff(const Json::Value &usrBuff) {

        coinBuffOn = true;

        setGlobalBuff(CookAbility(usrBuff));
        setGlobalAbilityMale(getInt(usrBuff["Male"]));
        setGlobalAbilityFemale(getInt(usrBuff["Female"]));
        setGlobalAbilityAll(getInt(usrBuff["All"]));
    }
    static void loadAppendChef(CList &chefList, int chefRarity,
                               const Json::Value &gameData,
                               const Json::Value &usrData
#ifndef _WIN32
                               ,
                               bool allowTool
#endif
    );

    std::string getName(bool wTool = true) const;
    Chef(Json::Value &v, int ultimateSkillId);
    Chef() { id = -1; }
    void print() const;
    void modifyTool(Tool);
    void modifyTool(ToolEnum);
    void deletePointers() {
        delete this->skill;
        delete this->companyBuff;
        delete this->nextBuff;
    }
    std::string getToolName() const;
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
class CList : public std::vector<Chef> {
    std::map<int, int> id2index;

  public:
    CList() : std::vector<Chef>() {}
    void initIDMapping() {
        int i = 0;
        for (auto &chef : *this) {
            id2index[chef.id] = i++;
        }
    }
    Chef *byId(int id) {
        int index = id2index[id];
        return &((*this)[index]);
    }
};

#endif