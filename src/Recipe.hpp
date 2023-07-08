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
    void print() {
        std::cout << "Materials: Vegetable: " << this->vegetable
                  << "; Meat: " << this->meat << "; Fish: " << this->fish
                  << "; Creation: " << this->creation << std::endl;
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
    void print() {
        std::cout << "Flavor: Sweet: " << this->sweet
                  << "; Salty: " << this->salty << "; Sour: " << this->sour
                  << "; Bitter: " << this->bitter << "; Spicy: " << this->spicy
                  << "; Tasty: " << this->tasty << std::endl;
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
    bool operator[](std::string flavor) {
        if (flavor == "Sweet") {
            return this->sweet;
        }
        if (flavor == "Salty") {
            return this->salty;
        }
        if (flavor == "Sour") {
            return this->sour;
        }
        if (flavor == "Bitter") {
            return this->bitter;
        }
        if (flavor == "Spicy") {
            return this->spicy;
        }
        if (flavor == "Tasty") {
            return this->tasty;
        }

        return false;
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
void loadRecipe(RList &recipeList, Json::Value &gameData, Json::Value &usrData);
#endif