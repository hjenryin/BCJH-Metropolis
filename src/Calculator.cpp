#include "Calculator.hpp"
#include "Chef.hpp"
#include <cmath>

int getPrice(Chef &chef, Recipe &recipe, bool verbose) {
    // if (verbose)
    //     std::cout << chef.name << " " << recipe.name << std::endl;
    int grade = chef.skill.ability / recipe.cookAbility;
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
    auto rb = recipe.rarityBuff[recipe.rarity - 1];
    int skillBuff = recipe.flavor * chef.skill.flavorBuff +
                    recipe.cookAbility * chef.skill.abilityBuff +
                    recipe.materials * chef.skill.materialBuff + rb.dishBuff;
    int buff =
        gradebuff + skillBuff + (chef.coinBuffOn ? chef.skill.coinBuff : 0);
    // std::cout << buff << std::endl;
    double bonus = 1.0 + buff / 100.0;
    // std::cout << bonus << std::endl;
    int singlePrice = std::ceil(recipe.price * bonus);
    // std::cout << singlePrice << std::endl;
    int totalPrice = singlePrice * rb.dishNum;
    if (verbose) {
        chef.print();
        recipe.print();
        std::cout << "Grade: " << grade << std::endl;
        std::cout << "Skill: " << skillBuff
                  << " (=" << recipe.flavor * chef.skill.flavorBuff << " + "
                  << recipe.cookAbility * chef.skill.abilityBuff << " + "
                  << recipe.materials * chef.skill.materialBuff << " + "
                  << rb.dishBuff << ")" << std::endl;
        std::cout << "Buff: " << buff << std::endl;
        std::cout << "Price: " << totalPrice << std::endl;
    }
    return totalPrice;
}

BanquetInfo getPrice(Chef *chef, Recipe *recipe, BanquetRule rule,
                     bool verbose) {
    // std::cout << chef.name << " " << recipe.name << std::endl;
    int grade = chef->skill.ability / recipe->cookAbility;
    // std::cout << grade << std::endl;
    int gradebuff = 0;
    switch (grade) {
    case 0: {
        // BanquetInfo b = {-10000000, recipe->rarity - rule.addRule.full};
        std::cout << "Grade 0" << std::endl;
        exit(0);
        // return b;
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
    auto rb = recipe->rarityBuff[recipe->rarity - 1];

    Skill addBuff = rule.addRule.skillBuff;
    Skill baseBuff = rule.baseRule.skillBuff;
    int intentionAddBuff = recipe->flavor * addBuff.flavorBuff +
                           recipe->cookAbility * addBuff.abilityBuff +
                           recipe->materials * addBuff.materialBuff +
                           rule.addRule.buff;
    int intentionBaseBuff = recipe->flavor * baseBuff.flavorBuff +
                            recipe->cookAbility * baseBuff.abilityBuff +
                            recipe->materials * baseBuff.materialBuff +
                            rule.baseRule.buff;
    int skillBuff = recipe->flavor * chef->skill.flavorBuff +
                    recipe->cookAbility * chef->skill.abilityBuff +
                    recipe->materials * chef->skill.materialBuff + rb.dishBuff;
    int buff = gradebuff + skillBuff + intentionAddBuff +
               (chef->coinBuffOn ? chef->skill.coinBuff : 0);
    int singlePrice =
        std::ceil((recipe->price + rule.baseRule.directAdd) *
                  (1.0 + intentionBaseBuff / 100.0) * (1.0 + buff / 100.0));
    // std::cout << singlePrice << std::endl;
    int totalPrice = singlePrice * rb.dishNum;
    if (verbose) {
        // chef->print();
        recipe->print();
        std::cout << "Grade: " << grade << std::endl;
        std::cout << "Skill: " << skillBuff
                  << " (=" << recipe->flavor * chef->skill.flavorBuff << " + "
                  << recipe->cookAbility * chef->skill.abilityBuff << " + "
                  << recipe->materials * chef->skill.materialBuff << " + "
                  << rb.dishBuff << ")" << std::endl;
        std::cout << "Intention: " << intentionAddBuff << " + "
                  << intentionBaseBuff << std::endl;
        std::cout << "Buff: " << buff << std::endl;
        std::cout << "Price: " << totalPrice << std::endl;
    }
    BanquetInfo b = {totalPrice, recipe->rarity - rule.addRule.full};
    return b;
}
