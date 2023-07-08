#include "Calculator.hpp"
#include "Chef.hpp"
#include <cmath>

int getPrice(Skill &skill, Recipe &recipe, ActivityBuff *activityBuff,
             bool verbose) {
    // if (verbose)
    //     std::cout << chef.name << " " << recipe.name << std::endl;
    int grade = skill.ability / recipe.cookAbility;
    // std::cout << grade << std::endl;
    int gradebuff = 0;
    switch (grade) {
    case 0:
        return 0;
    case 1:
        gradebuff = 0;
        break;
    case 2:
        gradebuff = 10;
        break;
    case 3:
        gradebuff = 30;
        break;
    case 4:
        gradebuff = 50;
        break;
    default:
        gradebuff = 100;
    }
    gradebuff += skill.rarityBuff[recipe.rarity];
    int activityBuffValue = 0;
    // std::cout << "activityBuff: " << activityBuff << std::endl;
    if (activityBuff != nullptr) {
        for (auto &i : activityBuff->materialBuff) {
            if (recipe.materials.find(i.first) != recipe.materials.end()) {
                activityBuffValue += i.second;
            }
        }
        for (auto &i : activityBuff->rarityBuff) {
            if (recipe.rarity == i.first) {
                activityBuffValue += i.second;
            }
        }
        for (auto &i : activityBuff->flavorBuff) {
            if (recipe.flavor.get_flavor() == i.first) {
                activityBuffValue += i.second;
            }
        }
    }
    // std::cout << "activityBuff: " << activityBuff << std::endl;
    auto rb = recipe.rarityBuff[recipe.rarity - 1];
    int skillBuff = recipe.flavor * skill.flavorBuff +
                    recipe.cookAbility * skill.abilityBuff +
                    recipe.materialCategories * skill.materialBuff +
                    rb.dishBuff;
    int buff = gradebuff + skillBuff + (Chef::coinBuffOn ? skill.coinBuff : 0) +
               activityBuffValue;
    // std::cout << buff << std::endl;
    double bonus = 1.0 + buff / 100.0;
    // std::cout << bonus << std::endl;
    int singlePrice = (int)std::ceil(recipe.price * bonus);
    // std::cout << singlePrice << std::endl;
    int totalPrice = singlePrice * rb.dishNum;
    if (verbose) {
        // chef.print();
        recipe.print();
        std::cout << "等级: " << grade << "，加成" << gradebuff << "%"
                  << std::endl;
        std::cout << "Skill: " << skillBuff
                  << " (=" << recipe.flavor * skill.flavorBuff << " + "
                  << recipe.cookAbility * skill.abilityBuff << " + "
                  << recipe.materialCategories * skill.materialBuff << " + "
                  << rb.dishBuff << ")"
                  << " + 金币" << (Chef::coinBuffOn ? skill.coinBuff : 0) << ")"
                  << std::endl;
        if (activityBuff != NULL)
            std::cout << "规则: " << activityBuffValue << std::endl;
        std::cout << "Buff: " << buff << std::endl;
        std::cout << "Price: " << totalPrice << std::endl;
    }
    return totalPrice;
}

BanquetInfo getPrice(Skill &skill, Recipe *recipe, BanquetRule r,
                     bool verbose) {
    // std::cout << chef.name << " " << recipe.name << std::endl;
    int grade = skill.ability / recipe->cookAbility;
    // std::cout << grade << std::endl;
    int gradebuff = 0;
    switch (grade) {
    case 0: {
        if (verbose)
            std::cout << "Grade 0" << std::endl;
        {
            BanquetInfo b = {0, 500};
            return b;
        }
    }
    case 1:
        gradebuff = 0;
        break;
    case 2:
        gradebuff = 10;
        break;
    case 3:
        gradebuff = 30;
        break;
    case 4:
        gradebuff = 50;
        break;
    default:
        gradebuff = 100;
    }
    gradebuff += skill.rarityBuff[recipe->rarity];
    auto rb = recipe->rarityBuff[recipe->rarity - 1];
    r.lenientRule.merge(r.strictRule); // vscode报错不认友元，但是编译没问题
    BanquetLenientRule rule = r.lenientRule;
    int intentionAddBuff = rule.addRule.buff;
    int intentionBaseBuff = rule.baseRule.buff;
    int skillBuff = recipe->flavor * skill.flavorBuff +
                    recipe->cookAbility * skill.abilityBuff +
                    recipe->materialCategories * skill.materialBuff +
                    rb.dishBuff + (Chef::coinBuffOn ? skill.coinBuff : 0);
    int buff = gradebuff + skillBuff + intentionAddBuff;
    int singlePrice = (int)std::ceil((recipe->price + rule.baseRule.directAdd) *
                                     (1.0 + intentionBaseBuff / 100.0) *
                                     (1.0 + buff / 100.0));
    // std::cout << singlePrice << std::endl;
    int totalPrice = singlePrice * rb.dishNum;
    int full;
    if (rule.addRule.fullAdd) {
        full = recipe->rarity + rule.addRule.full;
    } else {
        full = rule.addRule.full;
    }
    BanquetInfo b = {totalPrice, full};
    if (verbose) {
        // chef->print();
        std::cout << "╭--> ";
        recipe->print();
        std::cout << "等级: " << grade << "，加成" << gradebuff << "%"
                  << std::endl;
        std::cout << "Skill: " << skillBuff << "% ( = 味道"
                  << recipe->flavor * skill.flavorBuff << " + 技法"
                  << recipe->cookAbility * skill.abilityBuff << " + 食材"
                  << recipe->materialCategories * skill.materialBuff
                  << " + 修炼" << rb.dishBuff << " + 金币"
                  << (Chef::coinBuffOn ? skill.coinBuff : 0) << ")"
                  << std::endl;
        std::cout << "Intention: (基础+" << rule.baseRule.directAdd << "，+"
                  << intentionBaseBuff << "%；售价+" << intentionAddBuff
                  << "%) " << std::endl;
        std::cout << "售价总计Buff: " << buff << "%" << std::endl;
        std::cout << "╰--> 饱腹度: " << full << "\t总价: " << totalPrice
                  << std::endl;
    }
    return b;
}
