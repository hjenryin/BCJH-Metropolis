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
#include "exception.hpp"
#include "utils/math.hpp"
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
// Flavor Recipe::getFlavor(Json::Value &flavorJson) {
//     std::string flavorStr = flavorJson.asString();
//     Flavor flavor;
//     flavor.sweet = flavorStr.find("Sweet") != std::string::npos;
//     flavor.salty = flavorStr.find("Salty") != std::string::npos;
//     flavor.sour = flavorStr.find("Sour") != std::string::npos;
//     flavor.bitter = flavorStr.find("Bitter") != std::string::npos;
//     flavor.spicy = flavorStr.find("Spicy") != std::string::npos;
//     flavor.tasty = flavorStr.find("Tasty") != std::string::npos;
//     return flavor;
// }
FlavorEnum Recipe::getFlavor(Json::Value &flavorJson) {
    std::string flavorStr = flavorJson.asString();
    FlavorEnum f;
    if (flavorStr.find("Sweet") != std::string::npos)
        f = SWEET;
    else if (flavorStr.find("Salty") != std::string::npos)
        f = SALTY;
    else if (flavorStr.find("Sour") != std::string::npos)
        f = SOUR;
    else if (flavorStr.find("Bitter") != std::string::npos)
        f = BITTER;
    else if (flavorStr.find("Spicy") != std::string::npos)
        f = SPICY;
    else if (flavorStr.find("Tasty") != std::string::npos)
        f = TASTY;
    else
        std::abort();
    return f;
}
const struct MaterialList {
    int meat[16] = {1, 3, 4, 5, 7, 8, 9, 12, 26, 27, 28, 38, 39, 40, 43, 44};
    int vegetable[18] = {6,  10, 13, 14, 15, 16, 17, 18, 19,
                         22, 23, 25, 30, 31, 33, 36, 45, 46};
    int fish[7] = {2, 12, 24, 32, 37, 41, 42};
    int creation[6] = {11, 20, 21, 29, 34, 35};
} materialList;

void Recipe::print(const std::string &startLine, int priceDirectAdd,
                   int priceBuffAdd) const {
    auto rb = rarityBuff[this->rarity - 1];
    std::stringstream priceDirectAddStr, priceBuffAddStr, origPriceStr,
        finalPriceStr;
    priceBuffAddStr << (priceBuffAdd == 0 ? "" : " + ") << priceBuffAdd << "%";

    priceDirectAddStr << (priceDirectAdd == 0 ? "" : " + ") << priceDirectAdd;
    int finalPrice =
        int_ceil((this->price + priceDirectAdd) * (100 + priceBuffAdd) / 100);
    origPriceStr << (finalPrice == this->price ? "原价" : "") << this->price;
    finalPriceStr << (finalPrice == this->price ? "" : " = ") << finalPrice;

    std::cout << this->name << "（" << origPriceStr.str()
              << priceDirectAddStr.str() << priceBuffAddStr.str()
              << finalPriceStr.str() << "）"
              << ", " << this->rarity << "火 * " << rb.dishNum << std::endl;
    std::cout << startLine;
    this->cookAbility.print();
    this->materialCategories.print("；");
    this->printFlavor("；");
}
RList loadRecipe(const Json::Value &gameData, const Json::Value &usrData) {
    RList recipeList;
    auto recipes = gameData["recipes"];
    auto recipeGot = usrData["repGot"];
    for (auto recipe : recipes) {
        int id = recipe["recipeId"].asInt();
        if (recipeGot[std::to_string(id)].asBool()) {
            recipeList.push_back(Recipe(recipe));
        }
    }
    return recipeList;
}

// #define jsonStr2Int(v) atoi(v.asCString())
DishBuff Recipe::rarityBuff[5];
void Recipe::initRarityBuff(const Json::Value &usrBuff) {
    DishBuff r[5];
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

void testJsonUpdate(const Json::Value &gameData, const Json::Value &usrData) {
    int recipeNumUsr = usrData["repGot"].size();
    int recipeNumGame = gameData["recipes"].size();
    int chefNumUsr = usrData["chefGot"].size();
    int chefNumGame = gameData["chefs"].size();

    if (recipeNumUsr > recipeNumGame) {
        std::cout << "游戏数据更新了，当前文件仅收录了" << recipeNumGame
                  << "个菜谱，"
                  << "但是你的存档记录了" << recipeNumUsr
                  << "个菜谱。缺失的菜谱不会纳入计算。" << std::endl;
    }
    if (chefNumUsr > chefNumGame) {
        std::cout << "游戏数据更新了，当前文件仅收录了" << chefNumGame
                  << "个厨师，"
                  << "但是你的存档记录了" << chefNumUsr << "个厨师。"
                  << "缺失的厨师不会纳入计算。" << std::endl;
    }
}