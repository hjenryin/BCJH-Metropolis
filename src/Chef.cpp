#include "include/json/json.h"
#include "Chef.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "../config.hpp"
#include "Calculator.hpp"
#include "utils/json.hpp"
#include "../toolEquipped.hpp"
// #define jsonStr2Int(v) atoi(v.asCString())
CookAbility Chef::globalAbilityBuff;
int Chef::globalAbilityMale = 0;
int Chef::globalAbilityFemale = 0;
std::map<int, Skill> Skill::skillList;
void initBuff(Json::Value usrBuff) {
    Chef::setGlobalBuff(CookAbility(usrBuff));
    Chef::setGlobalAbilityMale(getInt(usrBuff["Male"]));
    Chef::setGlobalAbilityFemale(getInt(usrBuff["Female"]));
    Chef::setGlobalAbilityAll(getInt(usrBuff["All"]));
}
void splitUltimateSkill(std::map<int, int> &ultimateSkills, Json::Value &ids) {
    for (auto pair : ids) {
        auto str = pair.asString();
        int id = atoi(str.substr(0, str.find(",")).c_str());
        int skillId = atoi(str.substr(str.find(",") + 1).c_str());
        ultimateSkills[id] = skillId;
    }
}
void loadUltimateSkills(std::map<int, int> &ultimateSkills,
                        Json::Value &usrBuff) {
    splitUltimateSkill(ultimateSkills, usrBuff["Partial"]["id"]);
    splitUltimateSkill(ultimateSkills, usrBuff["Self"]["id"]);
}
void loadChef(std::map<int, Chef> &chefList) {

    Json::Value usrData;
    Json::Value gameData;
    std::ifstream gameDataF("../data/data.min.json", std::ifstream::binary);
    std::ifstream usrDataF("../data/userData.json", std::ifstream::binary);
    usrDataF >> usrData;
    usrDataF.close();
    gameDataF >> gameData;
    gameDataF.close();
    initBuff(usrData["userUltimate"]);
    const Json::Value chefs = gameData["chefs"];
    Skill::loadJson(gameData["skills"]);
    std::map<int, int> ultimateSkills;
    loadUltimateSkills(ultimateSkills, usrData["userUltimate"]);

    auto chefGot = usrData["chefGot"];
    for (auto chef : chefs) {
        int id = chef["chefId"].asInt();
        if (chefGot[std::to_string(id)].asBool()) {
            if (AVOID_CHEF_1 && chef["rarity"].asInt() == 1) {
                continue;
            }
            if (AVOID_CHEF_2 && chef["rarity"].asInt() == 2) {
                continue;
            }
            if (AVOID_CHEF_3 && chef["rarity"].asInt() == 3) {
                continue;
            }

            if (ultimateSkills.find(id) != ultimateSkills.end()) {
                chefList[id] = Chef(chef, ultimateSkills[id]);
            } else {
                chefList[id] = Chef(chef, -1);
            }
        }
    }
    auto chefP = chefList.begin();
    while (chefP != chefList.end()) {
        auto chef = &chefP->second;
        toolEquipped(chef);
        chefP++;
    }
}
/**
 * Chef
 * @param ultimateSkillId: -1 means no ultimate skill
 */
Chef::Chef(Json::Value &chef, int ultimateSkillId) {
    if (chef.isMember("chefId") && chef.isMember("name") &&
        chef.isMember("skill")) {
        // std::cout << "333" << std::endl;
        this->id = chef["chefId"].asInt();
        this->name = chef["name"].asString();
        this->skill.ability = CookAbility(chef);
        if (chef.isMember("tags") && chef["tags"].isArray()) {
            auto tags = chef["tags"];
            this->male = false;
            this->female = false;
            for (auto tag : tags) {
                if (tag.asInt() == 1) {
                    this->male = true;
                    this->skill.ability.add(globalAbilityMale);
                }
                if (tag.asInt() == 2) {
                    this->female = true;
                    this->skill.ability.add(globalAbilityFemale);
                }
            }
        }
    } else {
        std::cout << chef << std::endl;
        throw std::logic_error("Chef Json Error");
    }
    this->skill.ability.add(globalAbilityBuff);
    this->addSkill(chef["skill"].asInt());
    if (ultimateSkillId != -1) {
        this->addSkill(ultimateSkillId);
    }
    this->tool = -1;
}

void Chef::print() {
    std::cout << "ID: " << this->id << std::endl;
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Male: " << this->male << "; Female: " << this->female << "\n";
    this->skill.print();
}
CookAbility::CookAbility(Json::Value &v) {
    // std::cout << "Here" << std::endl;
    if (v.isMember("stirfry") && v.isMember("bake") && v.isMember("boil") &&
        v.isMember("fry") && v.isMember("knife")) {
        // std::cout << "yes1" << std::endl;
        this->stirfry = getInt(v["stirfry"]);
        this->bake = getInt(v["bake"]);
        this->boil = getInt(v["boil"]);
        this->steam = getInt(v["steam"]);
        this->fry = getInt(v["fry"]);
        this->knife = getInt(v["knife"]);

    } else if (v.isMember("Stirfry") && v.isMember("Bake") &&
               v.isMember("Boil") && v.isMember("Fry") && v.isMember("Knife")) {
        // std::cout << v << std::endl;
        this->stirfry = getInt(v["Stirfry"]);
        this->bake = getInt(v["Bake"]);
        this->boil = getInt(v["Boil"]);
        this->steam = getInt(v["Steam"]);
        this->fry = getInt(v["Fry"]);
        this->knife = getInt(v["Knife"]);
    } else {
        std::cout << "no" << std::endl;
        throw std::logic_error("CookAbility: Invalid Json");
    }
}
void Skill::loadJson(Json::Value &v) {
    for (auto skill : v) {
        int id = skill["skillId"].asInt();
        skillList[id] = Skill();
        for (auto effect : skill["effect"]) {
            auto skill = &skillList[id];
            if (effect["condition"].asString() != "Global") {
                std::string type = effect["type"].asString();
                int value = effect["value"].asInt();
                if (type == "Gold_Gain") {
                    skill->coinBuff = value;
                } else if (type == "Stirfry") {
                    skill->ability.stirfry = value;
                } else if (type == "Bake") {
                    skill->ability.bake = value;
                } else if (type == "Boil") {
                    skill->ability.boil = value;
                } else if (type == "Steam") {
                    skill->ability.steam = value;
                } else if (type == "Fry") {
                    skill->ability.fry = value;
                } else if (type == "Knife") {
                    skill->ability.knife = value;
                } else if (type == "UseStirfry") {
                    skill->abilityBuff.stirfry = value;
                } else if (type == "UseBake") {
                    skill->abilityBuff.bake = value;
                } else if (type == "UseBoil") {
                    skill->abilityBuff.boil = value;
                } else if (type == "UseSteam") {
                    skill->abilityBuff.steam = value;
                } else if (type == "UseFry") {
                    skill->abilityBuff.fry = value;
                } else if (type == "UseKnife") {
                    skill->abilityBuff.knife = value;
                } else if (type == "UseSweet") {
                    skill->flavorBuff.sweet = value;
                } else if (type == "UseSour") {
                    skill->flavorBuff.sour = value;
                } else if (type == "UseSalty") {
                    skill->flavorBuff.salty = value;
                } else if (type == "UseBitter") {
                    skill->flavorBuff.bitter = value;
                } else if (type == "UseSpicy") {
                    skill->flavorBuff.spicy = value;
                } else if (type == "UseTasty") {
                    skill->flavorBuff.tasty = value;
                } else if (type == "UseVegetable") {
                    skill->materialBuff.vegetable = value;
                } else if (type == "UseMeat") {
                    skill->materialBuff.meat = value;
                } else if (type == "UseFish") {
                    skill->materialBuff.fish = value;
                } else if (type == "UseCreation") {
                    skill->materialBuff.creation = value;
                }
            }
        }
    }
}
void Chef::addSkill(int id) { this->skill.add(Skill::skillList[id]); }

int CookAbility::operator/(const Ability &a) {
    int grade = 5;
    if (a.stirfry != 0) {
        grade = grade < (this->stirfry / a.stirfry)
                    ? grade
                    : (this->stirfry / a.stirfry);
        // std::cout << grade << std::endl;
    }
    if (a.bake != 0) {
        grade = grade < (this->bake / a.bake) ? grade : (this->bake / a.bake);
        // std::cout << grade << std::endl;
    }
    if (a.boil != 0) {
        grade = grade < (this->boil / a.boil) ? grade : (this->boil / a.boil);
        // std::cout << grade << std::endl;
    }
    if (a.steam != 0) {
        grade =
            grade < (this->steam / a.steam) ? grade : (this->steam / a.steam);
        // std::cout << grade << std::endl;
    }
    if (a.fry != 0) {
        grade = grade < (this->fry / a.fry) ? grade : (this->fry / a.fry);
        // std::cout << grade << std::endl;
    }
    if (a.knife != 0) {
        grade =
            grade < (this->knife / a.knife) ? grade : (this->knife / a.knife);
        // std::cout << grade << std::endl;
    }
    return grade;
}
int CookAbility::operator*(const AbilityBuff &a) {
    int grade = 0;
    if (this->stirfry != 0) {
        grade += a.stirfry;
    }
    if (this->bake != 0) {
        grade += a.bake;
    }
    if (this->boil != 0) {
        grade += a.boil;
    }
    if (this->steam != 0) {
        grade += a.steam;
    }
    if (this->fry != 0) {
        grade += a.fry;
    }
    if (this->knife != 0) {
        grade += a.knife;
    }

    return grade;
}

void loadChefTools(const std::map<int, Chef> &chefList,
                   std::map<int, Chef> &newChefList) {
    for (auto iter : chefList) {
        int id = iter.first;
        Chef chef = iter.second;
        if (chef.tool != NO_TOOL) {
            for (int i = 0; i < 6; i++) {
                newChefList[id * 6 + i] = chef.addTool((AbilityEnum)i);
            }
        } else {
            for (int i = 0; i < 6; i++) {
                newChefList[id * 6 + i] = chef;
            }
        }
    }
}
Chef Chef::addTool(AbilityEnum a) {
    Chef newChef(*this);
    newChef.tool = a;
    switch (a) {
    case STIRFRY:
        newChef.skill.ability.stirfry += 100;
        newChef.name += "-炒";
        return newChef;
    case BAKE:
        newChef.skill.ability.bake += 100;
        newChef.name += "-烤";
        return newChef;
    case STEAM:
        newChef.skill.ability.steam += 100;
        newChef.name += "-蒸";
        return newChef;
    case FRY:
        newChef.skill.ability.fry += 100;
        newChef.name += "-炸";
        return newChef;
    case BOIL:
        newChef.skill.ability.boil += 100;
        newChef.name += "-煮";
        return newChef;
    case KNIFE:
        newChef.skill.ability.knife += 100;
        newChef.name += "-切";
        return newChef;
    default:
        std::cout << "Invalid Tool!" << std::endl;
        throw 0;
    }
}
bool Chef::isCapable(Recipe *recipe) {
    if (this->skill.ability / recipe->cookAbility > 0) {
        return true;
    }
    return false;
}
void Chef::loadRecipeCapable(std::map<int, Recipe> &recipeList) {
    for (auto recipe = recipeList.begin(); recipe != recipeList.end();
         recipe++) {
        if (this->isCapable(&recipe->second)) {
            // std::cout << &(chef->second) << std::endl;
            this->recipeCapable.push_back(&recipe->second);
        }
    }
}
