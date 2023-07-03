#ifndef RECIPE_HPP
#define RECIPE_HPP

#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Chef.hpp"
#include "Values.hpp"
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
    void print(std::string end = "\n") {
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
class Flavor {
  public:
    bool sweet;
    bool salty;
    bool sour;
    bool bitter;
    bool spicy;
    bool tasty;
    Flavor()
        : sweet(false), salty(false), sour(false), bitter(false), spicy(false),
          tasty(false) {}
    void print(std::string end = "\n") {
        if (this->sweet) {
            std::cout << "甜";
        }
        if (this->salty) {
            std::cout << "咸";
        }
        if (this->sour) {
            std::cout << "酸";
        }
        if (this->bitter) {
            std::cout << "苦";
        }
        if (this->spicy) {
            std::cout << "辣";
        }
        if (this->tasty) {
            std::cout << "鲜";
        }
        std::cout << end;
    }
    FlavorEnum get_flavor() {
        if (this-> sweet){
            return FlavorEnum::SWEET;
        }
        if (this-> salty){
            return FlavorEnum::SALTY;
        }
        if (this-> sour){
            return FlavorEnum::SOUR;
        }
        if (this-> bitter){
            return FlavorEnum::BITTER;
        }
        if (this-> spicy){
            return FlavorEnum::SPICY;
        }
        if (this-> tasty){
            return FlavorEnum::TASTY;
        }
        return FlavorEnum::UNIDENTIFIED;
    }
    int operator*(FlavorBuff &buff) {
        int sum = 0;
        if (this->sweet) {
            sum += buff.sweet;
        }
        if (this->salty) {
            sum += buff.salty;
        }
        if (this->sour) {
            sum += buff.sour;
        }
        if (this->bitter) {
            sum += buff.bitter;
        }
        if (this->spicy) {
            sum += buff.spicy;
        }
        if (this->tasty) {
            sum += buff.tasty;
        }
        return sum;
    }
};

class Recipe {
  private:
    Flavor getFlavor(Json::Value &flavorJson);
    void getMaterials(Json::Value &materialsJson);

  public:
    static DishBuff rarityBuff[5];
    std::string name;
    int id;
    int rarity;
    int price;
    std::map<int, int> materials;
    Materials materialCategories;
    CookAbility cookAbility;
    Flavor flavor;
    Recipe(Json::Value &recipe);
    Recipe() {}
    void print();
    static void initRarityBuff(Json::Value &usrBuff);
};
typedef std::vector<Recipe> RList;
void loadRecipe(RList &recipeList);
#endif