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
#include "exception.hpp"
#include "loadToolEquipped.hpp"
bool Chef::coinBuffOn = true;
CookAbility Chef::globalAbilityBuff;
int Chef::globalAbilityMale = 0;
int Chef::globalAbilityFemale = 0;
ToolFileType Chef::toolFileType = NOT_LOADED;
std::map<int, Skill> Skill::skillList;
void initBuff(const Json::Value usrBuff) {
    Chef::setGlobalBuff(CookAbility(usrBuff));
    Chef::setGlobalAbilityMale(getInt(usrBuff["Male"]));
    Chef::setGlobalAbilityFemale(getInt(usrBuff["Female"]));
    Chef::setGlobalAbilityAll(getInt(usrBuff["All"]));
}
void splitUltimateSkill(std::map<int, int> &ultimateSkills,
                        const Json::Value &ids) {
    for (auto pair : ids) {
        auto str = pair.asString();
        int id = atoi(str.substr(0, str.find(",")).c_str());
        int skillId = atoi(str.substr(str.find(",") + 1).c_str());
        ultimateSkills[id] = skillId;
    }
}
void loadUltimateSkills(std::map<int, int> &ultimateSkills,
                        const Json::Value &usrBuff) {

    splitUltimateSkill(ultimateSkills, usrBuff["Partial"]["id"]);
    splitUltimateSkill(ultimateSkills, usrBuff["Self"]["id"]);
}
void Chef::loadAppendChef(CList &chefList, int chefRarity,
                          const Json::Value &gameData,
                          const Json::Value &usrData
#ifndef _WIN32
                          ,
                          bool allowTool
#endif
) {

    const Json::Value &chefs = gameData["chefs"];

    std::map<int, int> ultimateSkills;
    loadUltimateSkills(ultimateSkills, usrData["userUltimate"]);
    CList newChefList;
    auto chefGot = usrData["chefGot"];
    for (auto chef : chefs) {
        int id = chef["chefId"].asInt();
        if (chefGot[std::to_string(id)].asBool()) {
            if (chef["rarity"].asInt() != chefRarity) {
                continue;
            }

            if (ultimateSkills.find(id) != ultimateSkills.end()) {
                newChefList.push_back(Chef(chef, ultimateSkills[id]));
            } else {
                newChefList.push_back(Chef(chef, -1));
            }
        }
    }

#ifdef _WIN32
    if (toolFileType == NOT_LOADED) {
        toolFileType = loadToolFile();

        if (toolFileType == EMPTY_FILE__NOT_EQUIPPED) {
            std::cout << "toolEquipped.csv没有设定规则。允许所有厨师装备厨具。"
                      << std::endl;
        } else if (toolFileType == NO_FILE__NO_TOOL) {
            std::cout << "未找到toolEquipped.csv文件。不会装配任何厨具。"
                      << std::endl;
        } else {
            std::cout << "toolEquipped.csv文件已加载。" << std::endl;
        }
    }
    CSVWarning(w);
    for (auto &chef : newChefList) {

        w += loadToolFromFile(&chef, toolFileType);
    }
    if (w.missingRarity3) {
        std::cout
            << "提示：当前版本toolEquipped."
               "csv已支持“制作三火料理售价加成”，详见样例，但读取到的toolEquipp"
               "ed.csv中没有这一项。默认“制作三火料理售价加成”均为0。"
            << std::endl;
    }
#else
    if (allowTool) {
        for (auto &chef : newChefList) {
            chef.modifyTool(NOT_EQUIPPED);
        }
    } else {
        for (auto &chef : newChefList) {
            chef.modifyTool(NO_TOOL);
        }
    }
#endif
    chefList.insert(chefList.end(), newChefList.begin(), newChefList.end());
}
/**
 * Chef
 * @param ultimateSkillId: -1 means no ultimate skill
 */
Chef::Chef(Json::Value &chef, int ultimateSkillId) {
    if (chef.isMember("chefId") && chef.isMember("name") &&
        chef.isMember("skill")) {

        this->name = new std::string();
        this->skill = new Skill();
        this->companyBuff = new Skill();
        this->nextBuff = new Skill();
        this->id = chef["chefId"].asInt();
        *this->name = chef["name"].asString();
        this->skill->ability = CookAbility(chef);
        if (chef.isMember("tags") && chef["tags"].isArray()) {
            auto tags = chef["tags"];
            this->male = false;
            this->female = false;
            for (auto tag : tags) {
                if (tag.asInt() == 1) {
                    this->male = true;
                    this->skill->ability.add(globalAbilityMale);
                }
                if (tag.asInt() == 2) {
                    this->female = true;
                    this->skill->ability.add(globalAbilityFemale);
                }
            }
        }
    } else {
        std::cout << chef << std::endl;
        throw std::logic_error("Chef Json Error");
    }
    this->skill->ability.add(globalAbilityBuff);
    this->addSkill(chef["skill"].asInt());
    if (ultimateSkillId != -1) {
        this->addSkill(ultimateSkillId);
    }
    this->tool = NOT_EQUIPPED;
}

void Chef::print() const {
    std::cout << this->id << ": " << *this->name << "\t"
              << (this->male ? "男" : "") << (this->female ? "女" : "")
              << std::endl;
    this->skill->print();
}
CookAbility::CookAbility(const Json::Value &v) {

    if (v.isMember("stirfry") && v.isMember("bake") && v.isMember("boil") &&
        v.isMember("fry") && v.isMember("knife")) {

        this->stirfry = getInt(v["stirfry"]);
        this->bake = getInt(v["bake"]);
        this->boil = getInt(v["boil"]);
        this->steam = getInt(v["steam"]);
        this->fry = getInt(v["fry"]);
        this->knife = getInt(v["knife"]);

    } else if (v.isMember("Stirfry") && v.isMember("Bake") &&
               v.isMember("Boil") && v.isMember("Fry") && v.isMember("Knife")) {

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
void Skill::loadJson(const Json::Value &v) {
    for (auto skill : v) {
        int id = skill["skillId"].asInt();
        skillList[id] = Skill();
        for (auto effect : skill["effect"]) {

            Skill skill;
            std::string condition = effect["condition"].asString();
            if (condition != "Global") {
                if (condition == "Partial") {
                    skillList[id].type = PARTIAL;
                } else if (condition == "Self") {
                    skillList[id].type = SELF;
                } else if (condition == "Next") {
                    skillList[id].type = NEXT;
                }
                std::string type = effect["type"].asString();
                int value = effect["value"].asInt();
                if (type == "Gold_Gain") {
                    skill.pricePercentBuff = value;
                } else if (type == "Stirfry" || type == "Bake" ||
                           type == "Boil" || type == "Steam" || type == "Fry" ||
                           type == "Knife") {
                    std::string cal = effect["cal"].asString();
                    int *ptr = NULL;
                    if (cal == "Abs")
                        ptr = &skill.ability.stirfry;
                    else {
                        ptr = &skill.cookAbilityPercentBuff.stirfry;
                    }
                    int typeBias = getEnum(type) - ABILITY_ENUM_START;
                    ptr[typeBias] = value;
                } else if (type == "UseStirfry") {
                    skill.abilityBuff.stirfry = value;
                } else if (type == "UseBake") {
                    skill.abilityBuff.bake = value;
                } else if (type == "UseBoil") {
                    skill.abilityBuff.boil = value;
                } else if (type == "UseSteam") {
                    skill.abilityBuff.steam = value;
                } else if (type == "UseFry") {
                    skill.abilityBuff.fry = value;
                } else if (type == "UseKnife") {
                    skill.abilityBuff.knife = value;
                } else if (type == "UseSweet") {
                    skill.flavorBuff.sweet = value;
                } else if (type == "UseSour") {
                    skill.flavorBuff.sour = value;
                } else if (type == "UseSalty") {
                    skill.flavorBuff.salty = value;
                } else if (type == "UseBitter") {
                    skill.flavorBuff.bitter = value;
                } else if (type == "UseSpicy") {
                    skill.flavorBuff.spicy = value;
                } else if (type == "UseTasty") {
                    skill.flavorBuff.tasty = value;
                } else if (type == "UseVegetable") {
                    skill.materialBuff.vegetable = value;
                } else if (type == "UseMeat") {
                    skill.materialBuff.meat = value;
                } else if (type == "UseFish") {
                    skill.materialBuff.fish = value;
                } else if (type == "UseCreation") {
                    skill.materialBuff.creation = value;
                } else if (type == "BasicPrice") {
                    skill.baseAddBuff = value;
                }
                BuffCondition *condition = NULL;
                if (effect.isMember("conditionType")) {

                    auto conditionType = effect["conditionType"].asString();
                    int cvalue = 0;
                    if (effect.isMember("conditionValue"))
                        cvalue = getInt(effect["conditionValue"]);
                    if (conditionType == "CookbookRarity") {
                        for (auto &i : effect["conditionValueList"]) {
                            skill.rarityBuff[getInt(i)] = value;
                        }
                    } else if (conditionType == "PerRank") {
                        condition = new GradeBuffCondition(cvalue);
                    } else if (conditionType == "ExcessCookbookNum") {
                        DiscretizedBuff::Mask rarityUpperBound =
                            Recipe::moreThan(cvalue);
                        skill.rarityBuff.masked_add(rarityUpperBound, value);
                    } else if (conditionType == "FewerCookbookNum") {
                        DiscretizedBuff::Mask rarityLowerBound =
                            Recipe::lessThan(cvalue);
                        skill.rarityBuff.masked_add(rarityLowerBound, value);
                    } else if (conditionType == "SameSkill") {
                        condition = new ThreeSameCookAbilityBuffCondition();
                    } else if (conditionType == "Rank") {
                        skill.gradeBuff[cvalue] = value;
                    }
                }
                if (condition == NULL) {
                    skillList[id] += skill;
                } else {
                    skillList[id].conditionalEffects.push_back(
                        new ConditionalBuff(condition, skill));
                }
            }
        }
    }
}
void Chef::addSkill(int id) {
    auto skill = Skill::skillList[id];
    if (skill.type == Skill::SELF) {
        *this->skill += skill;
    } else if (skill.type == Skill::PARTIAL) {
        *this->companyBuff += skill;
    } else if (skill.type == Skill::NEXT) {
        *this->nextBuff += skill;
    }
}

// int CookAbility::operator/(const Ability &a) const {
//     int grade = 5;
//     const int *thisptr = &this->stirfry;
//     const int *aptr = &a.stirfry;
//     for (int i = 0; i < 6; i++) {
//         if (aptr[i] != 0) {
//             grade =
//                 grade < (thisptr[i] / aptr[i]) ? grade : (thisptr[i] /
//                 aptr[i]);
//         }
//     }
//     return grade;
// }
int CookAbility::operator*(const AbilityBuff &a) const {
    int buff = 0;
    const int *thisptr = &this->stirfry;
    const int *aptr = &a.stirfry;
    for (int i = 0; i < 6; i++) {
        if (thisptr[i] != 0) {
            buff += aptr[i];
        }
    }
    return buff;
}

// Chef Chef::addTool_modify_name(ToolEnum a) {
//     Chef newChef(*this);
//     newChef.tool = a;
//     switch (a) {
//     case STIRFRY:
//         newChef.skill.ability.stirfry += 100;
//         newChef.name += "-炒";
//         return newChef;
//     case BAKE:
//         newChef.skill.ability.bake += 100;
//         newChef.name += "-烤";
//         return newChef;
//     case STEAM:
//         newChef.skill.ability.steam += 100;
//         newChef.name += "-蒸";
//         return newChef;
//     case FRY:
//         newChef.skill.ability.fry += 100;
//         newChef.name += "-炸";
//         return newChef;
//     case BOIL:
//         newChef.skill.ability.boil += 100;
//         newChef.name += "-煮";
//         return newChef;
//     case KNIFE:
//         newChef.skill.ability.knife += 100;
//         newChef.name += "-切";
//         return newChef;
//     default:
//         std::cout << "Invalid Tool!" << std::endl;
//         throw 0;
//     }
// }
// bool Chef::isCapable(Recipe *recipe) {
//     if (this->skill.ability / recipe->cookAbility > 0) {
//         return true;
//     }
//     return false;
// }
// void Chef::loadRecipeCapable(std::vector<Recipe> &recipeList) {
//     if (this->tool == NO_TOOL) {
//         for (auto &recipe : recipeList) {
//             if (this->isCapable(&recipe)) {
//                 this->recipeCapable.push_back(&recipe);
//             }
//         }
//     } else if (this->tool == NOT_EQUIPPED) {
//         std::vector<Recipe *> recipeListCopy;
//         for (auto &recipe : recipeList) {
//             recipeListCopy.push_back(&recipe);
//         }
//         for (int i = 0; i < 6; i++) {
//             this->modifyTool((ToolEnum)i);
//             auto iter = recipeListCopy.begin();
//             while (iter != recipeListCopy.end()) {
//                 if (this->isCapable(*iter)) {
//                     this->recipeCapable.push_back(*iter);
//                     iter = recipeListCopy.erase(iter);
//                 } else {
//                     iter++;
//                 }
//             }
//         }
//         this->modifyTool(NOT_EQUIPPED);
//     }
// }
void Chef::modifyTool(ToolEnum a) {
    if (this->tool == NO_TOOL)
        return;
    this->tool = a;
}

std::string getToolName(ToolEnum tool) {
    std::string toolName;
    switch (tool) {
    case STIRFRY:
        toolName = "炒";
        break;
    case BOIL:
        toolName = "煮";
        break;
    case FRY:
        toolName = "炸";
        break;
    case STEAM:
        toolName = "蒸";
        break;
    case BAKE:
        toolName = "烤";
        break;
    case KNIFE:
        toolName = "切";
        break;
    case NO_TOOL:
        toolName = "厨具禁止";
        break;
    default:
        toolName = "无厨具";
    }
    return toolName;
}