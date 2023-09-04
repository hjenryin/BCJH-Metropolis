#include "Calculator.hpp"
#include "Chef.hpp"
#include "utils/Printer.hpp"
#include "utils/math.hpp"
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
    int grade = skill.ability / recipe->cookAbility;
    int gradebuff = 0;
    switch (grade) {
    case 0: {
        if (verbose)
            std::cout << "Grade 0" << std::endl;
        {
            BanquetInfo b = {0, 500};
            // Need this when exacting chef tool.
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
    for (int i = 1; i <= grade; i++) {
        gradebuff += skill.gradeBuff[i];
    }
    auto rb = Recipe::rarityBuff[recipe->rarity - 1];
    r.lenientRule.merge(r.strictRule);
    BanquetLenientRule rule = r.lenientRule;
    int intentionAddBuff = rule.addRule.buff;
    int intentionBaseBuff = rule.baseRule.buff;
    int skillBuff = skill.flavorBuff * recipe->flavor +
                    recipe->cookAbility * skill.abilityBuff +
                    recipe->materialCategories * skill.materialBuff +
                    (rb.dishBuff + skill.rarityBuff[recipe->rarity]) +
                    (Chef::coinBuffOn ? skill.pricePercentBuff : 0);
    int buff = gradebuff + skillBuff + intentionAddBuff;
    int singlePrice =
        int_ceil((recipe->price + rule.baseRule.directAdd) *
                 (1.0 + (intentionBaseBuff + skill.baseAddBuff) / 100.0) *
                 (1.0 + buff / 100.0));
    int totalPrice = singlePrice * rb.dishNum;
    int full;
    if (rule.addRule.fullAdd) {
        full = recipe->rarity + rule.addRule.full;
    } else {
        full = rule.addRule.full;
    }
    BanquetInfo b = {totalPrice, full};
    if (verbose) {
        Printer skillPercentPrinter("售价", true);
        skillPercentPrinter.add("味道", skill.flavorBuff * recipe->flavor);
        skillPercentPrinter.add("技法",
                                recipe->cookAbility * skill.abilityBuff);
        skillPercentPrinter.add("食材", recipe->materialCategories *
                                            skill.materialBuff);
        skillPercentPrinter.add("火数修炼", rb.dishBuff);
        skillPercentPrinter.add(
            "金币", (Chef::coinBuffOn ? skill.pricePercentBuff : 0));

        Printer skillBasePrinter("基础", true);
        skillBasePrinter.noValue();
        skillBasePrinter.add("", skill.baseAddBuff);

        Printer skillPrinter("技能");
        skillPrinter.noValue();
        skillPrinter.add(skillBasePrinter);
        skillPrinter.add(skillPercentPrinter);

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

        std::cout << "│ 售价总计Buff: " << int2signed_str(buff) << "%"
                  << std::endl;
        std::cout << "╰─> 饱腹度: " << full << "\t总价: " << totalPrice
                  << std::endl;
    }
    return b;
}
