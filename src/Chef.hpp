#ifndef CHEF_HPP
#define CHEF_HPP

#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Recipe.hpp"
#include "Values.hpp"
#include <vector>
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
    ToolEnum getTool() { return this->tool; }
    void NoTool() { this->tool = NO_TOOL; }
    std::string name;
    Skill skill;
    Skill companyBuff;
    Skill nextBuff;
    std::vector<Recipe *> recipeCapable;
    std::vector<Recipe *> recipeLearned;

    void loadRecipeCapable(std::vector<Recipe> &recipeList);
    bool isCapable(Recipe *r);

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

    Chef(Json::Value &v, int ultimateSkillId);
    Chef() {}
    void print();
    void modifyTool(ToolEnum);
    Chef addTool_modify_name(ToolEnum);
};
typedef std::vector<Chef> CList;
void loadChef(CList &chefList);

// void loadChefTools(CList &chefList, CList &newChefList);
std::string getToolName(ToolEnum tool);

#endif