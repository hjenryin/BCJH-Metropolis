#include "Calculator.hpp"
#include "Chef.hpp"
#include <cmath>
#include "utils/Printer.hpp"
std::string gradeName(int i) {
    switch (i) {
    case 1:
        return "可级";
    case 2:
        return "优级";
    case 3:
        return "特级";
    case 5:
        return "传级";
    case 4:
        return "神级";
    default:
        return "未知，是BUG，菜谱等级为" + std::to_string(i);
    }
}
BanquetInfo getPrice(const Skill &skill, Recipe *recipe, BanquetRuleTogether &r,
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
    int skillBuff = skill.flavorBuff * recipe->flavor +
                    recipe->cookAbility * skill.abilityBuff +
                    recipe->materialCategories * skill.materialBuff +
                    rb.dishBuff +
                    (Chef::coinBuffOn ? skill.pricePercentBuff : 0);
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
        Printer skillPrinter("技能", true);
        skillPrinter.add("味道", skill.flavorBuff * recipe->flavor);
        skillPrinter.add("技法", recipe->cookAbility * skill.abilityBuff);
        skillPrinter.add("食材",
                         recipe->materialCategories * skill.materialBuff);
        skillPrinter.add("火数修炼", rb.dishBuff);
        skillPrinter.add("金币",
                         (Chef::coinBuffOn ? skill.pricePercentBuff : 0));

        Printer intentionPrinter("意图");
        intentionPrinter.noValue();
        intentionPrinter.add("基础售价", rule.baseRule.directAdd, false);
        intentionPrinter.add("基础售价", intentionBaseBuff, true);
        intentionPrinter.add("售价", intentionAddBuff, true);

        std::cout << "╭─> ";
        recipe->print("│ ");
        std::cout << "" << gradeName(grade) << " +" << gradebuff << "%"
                  << std::endl;
        skillPrinter.print("│ ", " + ", "\n");
        intentionPrinter.print("│ ", "; ", "\n");
        std::cout << "│ 售价总计Buff: +" << buff << "%" << std::endl;
        std::cout << "╰─> 饱腹度: " << full << "\t总价: " << totalPrice << "💰"
                  << std::endl;
    }
    return b;
}
