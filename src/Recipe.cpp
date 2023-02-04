#include "include/json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include "Chef.hpp"
#include "Recipe.hpp"
#include <vector>
#include "utils/json.hpp"
#include "../config.hpp"
Recipe::Recipe(Json::Value &recipe) {
    this->name = recipe["name"].asString();
    this->id = recipe["recipeId"].asInt();
    this->rarity = recipe["rarity"].asInt();
    this->price = recipe["price"].asInt();
    this->cookAbility.stirfry = recipe["stirfry"].asInt();
    this->cookAbility.bake = recipe["bake"].asInt();
    this->cookAbility.boil = recipe["boil"].asInt();
    this->cookAbility.steam = recipe["steam"].asInt();
    this->cookAbility.fry = recipe["fry"].asInt();
    this->cookAbility.knife = recipe["knife"].asInt();
    this->getMaterials(recipe["materials"]);
    this->flavor = this->getFlavor(recipe["condiment"]);
}
Flavor Recipe::getFlavor(Json::Value &flavorJson) {
    std::string flavorStr = flavorJson.asString();
    Flavor flavor;
    flavor.sweet = flavorStr.find("Sweet") != std::string::npos;
    flavor.salty = flavorStr.find("Salty") != std::string::npos;
    flavor.sour = flavorStr.find("Sour") != std::string::npos;
    flavor.bitter = flavorStr.find("Bitter") != std::string::npos;
    flavor.spicy = flavorStr.find("Spicy") != std::string::npos;
    flavor.tasty = flavorStr.find("Tasty") != std::string::npos;
    return flavor;
}
const struct MaterialList {
    int meat[16] = {1, 3, 4, 5, 7, 8, 9, 12, 26, 27, 28, 38, 39, 40, 43, 44};
    int vegetable[18] = {6,  10, 13, 14, 15, 16, 17, 18, 19,
                         22, 23, 25, 30, 31, 33, 36, 45, 46};
    int fish[7] = {2, 12, 24, 32, 37, 41, 42};
    int creation[6] = {11, 20, 21, 29, 34, 35};
} materialList;

void Recipe::print() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "ID: " << this->id << std::endl;
    std::cout << "Rarity: " << this->rarity;
    auto rb = rarityBuff[this->rarity - 1];
    std::cout << " (Num: " << rb.dishNum << ", +" << rb.dishBuff << "%)"
              << std::endl;
    std::cout << "Price: " << this->price << std::endl;
    this->materialCategories.print();
    this->cookAbility.print();
    this->flavor.print();
}
void loadRecipe(std::map<int, Recipe> &recipeList) {
    Json::Value usrData;
    std::ifstream gameDataF("../data/data.min.json", std::ifstream::binary);
    // std::cout << gameDataF.fail() << std::endl;
    Json::Value gameData;
    gameDataF >> gameData;
    gameDataF.close();
    // std::cout << "Game data loaded" << std::endl;
    std::ifstream usrDataF("../data/userData.json", std::ifstream::binary);
    usrDataF >> usrData;
    usrDataF.close();
    Recipe::initRarityBuff(usrData["userUltimate"]);
    auto recipes = gameData["recipes"];
    auto recipeGot = usrData["repGot"];
    for (auto recipe : recipes) {
        int id = recipe["recipeId"].asInt();
        if (recipeGot[std::to_string(id)].asBool()) {
            recipeList[id] = Recipe(recipe);
        }
    }
}

// #define jsonStr2Int(v) atoi(v.asCString())
dishBuff Recipe::rarityBuff[5];
void Recipe::initRarityBuff(Json::Value &usrBuff) {
    dishBuff r[5];
    r[0].dishNum = 40;
    r[1].dishNum = 30;
    r[2].dishNum = 25;
    r[3].dishNum = 20;
    r[4].dishNum = 15;
    r[0].dishNum += getInt(usrBuff["MaxLimit_1"]);
    r[1].dishNum += getInt(usrBuff["MaxLimit_2"]);
    r[2].dishNum += getInt(usrBuff["MaxLimit_3"]);
    r[3].dishNum += getInt(usrBuff["MaxLimit_4"]);
    r[4].dishNum += getInt(usrBuff["MaxLimit_5"]);
    r[0].dishBuff = getInt(usrBuff["PriceBuff_1"]);
    r[1].dishBuff = getInt(usrBuff["PriceBuff_2"]);
    r[2].dishBuff = getInt(usrBuff["PriceBuff_3"]);
    r[3].dishBuff = getInt(usrBuff["PriceBuff_4"]);
    r[4].dishBuff = getInt(usrBuff["PriceBuff_5"]);
    for (int i = 0; i < 5; i++) {
        rarityBuff[i] = r[i];
    }
    if (MODE == 2) {
        for (int i = 0; i < 5; i++) {
            rarityBuff[i].dishNum = 1;
        }
    }
}
void Recipe::getMaterials(Json::Value &materialsJson) {
    this->materials.clear();
    this->materialCategories = Materials();
    for (auto m : materialsJson) {
        int materialId = m["material"].asInt();
        int quantity = m["quantity"].asInt();
        this->materials[materialId] = quantity;

        for (auto meat : materialList.meat) {
            if (materialId == meat) {
                materialCategories.meat = true;
                break;
            }
        }
        for (auto vegetable : materialList.vegetable) {
            if (materialId == vegetable) {
                materialCategories.vegetable = true;
                break;
            }
        }
        for (auto fish : materialList.fish) {
            if (materialId == fish) {
                materialCategories.fish = true;
                break;
            }
        }
        for (auto creation : materialList.creation) {
            if (materialId == creation) {
                materialCategories.creation = true;
                break;
            }
        }
    }
}
