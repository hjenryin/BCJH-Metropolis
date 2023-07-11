#ifndef RECIPE_HPP
#define RECIPE_HPP

#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Chef.hpp"
#include "Values.hpp"
#include "Types.hpp"
#include <vector>

struct DishBuff {
    int dishNum;
    int dishBuff = 0;
};
class Materials {
  public:
    bool vegetable;
    bool meat;
    bool fish;
    bool creation;
    Materials() : vegetable(false), meat(false), fish(false), creation(false) {}
    void print(std::string end = "\n") const {
        std::cout << (this->vegetable ? "菜 " : "") << (this->meat ? "肉 " : "")
                  << (this->fish ? "鱼 " : "") << (this->creation ? "面 " : "")
                  << end;
    }
    int operator*(MaterialCategoryBuff &buff) {
        int sum = 0;
        if (this->vegetable) {
            sum += buff.vegetable;
        }
        if (this->meat) {
            sum += buff.meat;
        }
        if (this->fish) {
            sum += buff.fish;
        }
        if (this->creation) {
            sum += buff.creation;
        }
        return sum;
    }
};

class Recipe {
  private:
    FlavorEnum getFlavor(Json::Value &flavorJson);
    void getMaterials(Json::Value &materialsJson);
    void printFlavor(std::string end = "\n") const {
        if (this->flavor == SWEET) {
            std::cout << "甜";
        } else if (this->flavor == SALTY) {
            std::cout << "咸";
        } else if (this->flavor == SOUR) {
            std::cout << "酸";
        } else if (this->flavor == BITTER) {
            std::cout << "苦";
        } else if (this->flavor == SPICY) {
            std::cout << "辣";
        } else if (this->flavor == TASTY) {
            std::cout << "鲜";
        } else {
            std::cout << "未知";
        }
        std::cout << end;
    }

  public:
    static DishBuff rarityBuff[5];
    std::string name;
    int id;
    int rarity;
    int price;
    std::map<int, int> materials;
    Materials materialCategories;
    CookAbility cookAbility;
    FlavorEnum flavor;
    Recipe(Json::Value &recipe);
    Recipe() : flavor(UNIDENTIFIED_FLAVOR) {}
    void print(const std::string &startLine = "") const;
    static void initRarityBuff(const Json::Value &usrBuff);
};
typedef std::vector<Recipe> RList;
void loadRecipe(RList &recipeList, const Json::Value &userData,
                const Json::Value &gameData);
#endif