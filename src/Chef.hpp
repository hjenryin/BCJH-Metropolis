#ifndef CHEF_HPP
#define CHEF_HPP

#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Recipe.hpp"
#include "Values.hpp"
class Recipe;

class Chef {
  private:
    static CookAbility globalAbilityBuff;
    static int globalAbilityMale;
    static int globalAbilityFemale;
    void addSkill(int id);

  public:
    static bool coinBuffOn;
    bool male;
    bool female;
    int id;
    int tool = NOT_EQUIPPED;
    std::string name;
    Skill skill;
    std::vector<Recipe *> recipeCapable;

    void loadRecipeCapable(std::map<int, Recipe> &recipeList);
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
    Chef addTool(AbilityEnum);
};

void loadChef(std::map<int, Chef> &chefList);

void loadChefTools(const std::map<int, Chef> &chefList,
                   std::map<int, Chef> &newChefList);

#endif