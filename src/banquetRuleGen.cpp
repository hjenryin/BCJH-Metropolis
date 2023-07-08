#include "banquetRuleGen.hpp"
#include "utils/json.hpp"
#include "functions.hpp"
#include <cassert>
Json::Value &getIntentById(Json::Value &intentsGD, int intentId) {
    for (auto &intent : intentsGD) {
        if (intent["intentId"].asInt() == intentId) {
            return intent;
        }
    }
    throw std::runtime_error("Intent not found " + std::to_string(intentId));
}
Json::Value &getBuffById(Json::Value &intentsGD, int intentId) {
    for (auto &intent : intentsGD) {
        if (intent["buffId"].asInt() == intentId) {
            return intent;
        }
    }
    throw std::runtime_error("Buff not found " + std::to_string(intentId));
}
Rule *getRuleFromJson(Json::Value &intent, int d, Json::Value &allIntents,
                      Json::Value &allBuffs,
                      int remainingDishes = DISH_PER_CHEF);

bool loadBanquetRule(RuleInfo &ruleInfo, Json::Value &gameData, int ruleID) {
    auto buffsGD = gameData["buffs"];
    auto intentsGD = gameData["intents"];
    auto rulesGD = gameData["rules"];
    // find the rule in rulesGD with Id ruleID
    Json::Value ruleGD;
    for (auto &r : rulesGD) {
        if (r["Id"].asInt() == ruleID) {
            ruleGD = r;
            break;
        }
    }
    std::cout << "规则: " << ruleGD["Title"].asString() << std::endl;
    rulesGD = ruleGD["Group"];

    int d = 0;
    int bestFull[NUM_GUESTS];
    for (auto &guest : rulesGD) {
        ruleInfo.bestFull[d / DISH_PER_CHEF / CHEFS_PER_GUEST] =
            guest["Satiety"].asInt();
        auto intents = guest["IntentList"];
        for (auto &phaseIntents : intents) {
            for (auto &intent : phaseIntents) {
                int intentId = intent.asInt();
                auto intentContent = getIntentById(intentsGD, intentId);
                ruleInfo.rl.push_back(
                    getRuleFromJson(intentContent, d, intentsGD, buffsGD));
            }
            d += DISH_PER_CHEF;
        }
    }
    return true;
}

Rule *getRuleFromJson(Json::Value &intent, int d, Json::Value &allIntents,
                      Json::Value &allBuffs, int remainingDishes) {
    Condition *c;
    auto effectType = intent["effectType"].asString();
    auto effectValue = intent["effectValue"].asInt();
    std::string conditionType;
    std::string conditionValue;

    if (intent.isMember("conditionType")) {
        conditionType = intent["conditionType"].asString();
        conditionValue = intent["conditionValue"].asString();
        if (conditionType == "CookSkill") {
            c = new SkillCondition(d, conditionValue, remainingDishes);
        } else if (conditionType == "CondimentSkill") {
            c = new FlavorCondition(d, conditionValue, remainingDishes);
        } else if (conditionType == "Order") {
            c = new OrderCondition(d, getInt(intent["conditionValue"]));
        } else if (conditionType == "Rarity") {
            c = new RarityCondition(d, getInt(intent["conditionValue"]),
                                    remainingDishes);
        } else if (conditionType == "Group") {
            assert(effectType == "CreateBuff");
            c = new GroupCondition(d, conditionType, remainingDishes);
        } else if (conditionType == "Rank") {
            c = new RankCondition(d, getInt(intent["conditionValue"]),
                                  remainingDishes);
        } else {
            std::cout << "Unknown condition type: " << conditionType
                      << std::endl;
        }
    } else {
        assert(getInt(intent["intentId"]) == 1);
        c = new AlwaysTrueCondition(d);
    }

    Effect *e;
    if (effectType == "BasicPriceChange") {
        e = new BasePriceAddEffect(effectValue);
    } else if (effectType == "BasicPriceChangePercent") {
        e = new BasePricePercentEffect(effectValue);
    } else if (effectType == "PriceChangePercent") {
        e = new PricePercentEffect(effectValue);
    } else if (effectType == "SatietyChange") {
        e = new FullAddEffect(effectValue);
    } else if (effectType == "SetSatietyValue") {
        e = new FullSetEffect(effectValue);
    } else if (effectType == "IntentAdd") {
        e = new IntentAddEffect();
    } else if (effectType == "CreateIntent") {
        auto newIntent = getIntentById(allIntents, effectValue);
        auto newRule = getRuleFromJson(newIntent, d + 1, allIntents, allBuffs,
                                       remainingDishes - 1);
        e = new CreateRuleEffect(newRule);
    } else if (effectType == "CreateBuff") {
        assert(conditionType == "Group");
        auto newIntent = getBuffById(allBuffs, effectValue);
        int lastRounds = getInt(newIntent["lastRounds"]);
        auto newRule = getRuleFromJson(newIntent, d + DISH_PER_CHEF, allIntents,
                                       allBuffs, lastRounds * DISH_PER_CHEF);
        e = new CreateRuleEffect(newRule, true);
    }
    return new SingleConditionRule(c, e);
}
void banquetRuleGenerated(BanquetRuleTogether *rule, States &s,
                          const RuleInfo &allRules) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }
    for (auto &r : allRules.rl) {
        (*r)(strictRule, lenientRule, s);
    }
}