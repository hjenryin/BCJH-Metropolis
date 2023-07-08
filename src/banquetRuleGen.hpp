#ifndef BANQUET_RULE_GEN_HPP
#define BANQUET_RULE_GEN_HPP
#include "Recipe.hpp"
#include "../config.hpp"
#include "Calculator.hpp"
#include "types.hpp"
#include <cassert>
class Effect {
  public:
    bool strict;
    virtual void operator()(BanquetStrictRule **strictRule,
                            BanquetLenientRule **lenientRule, int i,
                            States &s) const = 0;

    Effect(bool strict = false) : strict(strict) {}
};
class Rule {
  public:
    Effect *effect;
    Rule(Effect *effect) : effect(effect) {}
    virtual void operator()(BanquetStrictRule **strictRule,
                            BanquetLenientRule **lenientRule, States &s,
                            int overrideDishStart = -1) const = 0;
    ~Rule() { delete effect; }
};

class Condition {
  public:
    int start;
    int lasts;

    virtual int operator()(States &s) const = 0;
    Condition(int start, int lasts) : start(start), lasts(lasts) {}
    Condition() : start(0), lasts(0) {}
};

class RarityCondition : public Condition {
  public:
    int rarity;
    RarityCondition(int start, int rarity, int lasts = DISH_PER_CHEF,
                    bool strict = false)
        : Condition(start, lasts), rarity(rarity) {}
    int operator()(States &s) const override {
        auto recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            if (recipes[i]->rarity == rarity) {
                return i;
            }
        }
        return -1;
    }
};
class SkillCondition : public Condition {
  public:
    std::string skill;
    SkillCondition(int start, std::string skill, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts), skill(skill) {}
    int operator()(States &s) const override {
        auto recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            if (recipes[i]->cookAbility[skill] > 0) {
                return i;
            }
        }
        return -1;
    }
};
class OrderCondition : public Condition {
  public:
    int order;
    OrderCondition(int start, int order)
        : Condition(start, DISH_PER_CHEF), order(order) {}
    int operator()(States &s) const override { return start + order - 1; }
};
class FlavorCondition : public Condition {
  public:
    std::string flavor;
    FlavorCondition(int start, std::string flavor, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts), flavor(flavor) {}
    int operator()(States &s) const override {
        auto recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            if (recipes[i]->flavor[flavor]) {
                return i;
            }
        }
        return -1;
    }
};
class GroupCondition : public Condition {
  public:
    std::string group;
    GroupCondition(int start, std::string group, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts), group(group) {}
    int operator()(States &s) const override {
        auto recipes = s.recipe;

        for (int i = start; i < start + lasts; i++) {
            if (!(recipes[i]->flavor[group] ||
                  recipes[i]->cookAbility[group] > 0)) {
                return -1;
            }
        }
        return start + DISH_PER_CHEF;
    }
};
class RankCondition : public Condition {
  public:
    int rank;
    RankCondition(int start, int rank, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts), rank(rank) {}
    int operator()(States &s) const override {
        Skill companyBuff[NUM_GUESTS];
        for (int i = 0; i < NUM_CHEFS; i++) {
            companyBuff[i / CHEFS_PER_GUEST].add(s.chef[i]->companyBuff);
        }
        auto recipes = s.recipe;
        auto chefs = s.chef;
        for (int i = start; i < start + lasts; i++) {
            auto skill = chefs[i / DISH_PER_CHEF]->skill +
                         companyBuff[i / DISH_PER_CHEF / CHEFS_PER_GUEST];
            if (skill.ability / recipes[i]->cookAbility >= rank) {
                return i;
            }
        }
        return -1;
    }
};
class AlwaysTrueCondition : public Condition {
  public:
    AlwaysTrueCondition(int start, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts) {}
    int operator()(States &s) const override { return start; }
};

class IntentAddEffect : public Effect {
  public:
    IntentAddEffect() {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        // assert(strict == false);
        lenientRule[i]->oneMore();
    }
};
class CreateRuleEffect : public Effect {
  public:
    Rule *rule;
    CreateRuleEffect(Rule *rule, bool strict = false) : rule(rule) {
        rule->effect->strict = strict;
    }
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if ((i + 1) % 3 != 0) {
            (*rule)(strictRule, lenientRule, s, i + 1);
        }
    }
};
class CreateRulesEffect : public Effect {
  public:
    Rule *rule;
    int len;
    CreateRulesEffect(Rule *rule, int len, bool strict = false)
        : rule(rule), len(len) {
        for (int i = 0; i < len; i++)
            rule->effect->strict = strict;
    }
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        for (int j = 0; j < len; j++) {
            (*rule)(strictRule, lenientRule, s, i + j);
        }
    }
};
class FullAddEffect : public Effect {
  public:
    int full;
    FullAddEffect(int full) : full(full) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if (strict) {
            strictRule[i]->addRule.full += full;
        } else {
            lenientRule[i]->addRule.full += full;
        }
    }
};
class FullSetEffect : public Effect {
  public:
    int full;
    FullSetEffect(int full) : full(full) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if (strict) {
            strictRule[i]->addRule.full = full;
            strictRule[i]->addRule.fullAdd = false;
        } else {
            lenientRule[i]->addRule.full = full;
            lenientRule[i]->addRule.fullAdd = false;
        }
    }
};

class BasePricePercentEffect : public Effect {
  public:
    int percent;
    BasePricePercentEffect(int percent) : percent(percent) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if (strict) {
            strictRule[i]->baseRule.buff += percent;
        } else {
            lenientRule[i]->baseRule.buff += percent;
        }
    }
};
class BasePriceAddEffect : public Effect {
  public:
    int add;
    BasePriceAddEffect(int add) : add(add) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if (strict) {
            strictRule[i]->baseRule.directAdd += add;
        } else {
            lenientRule[i]->baseRule.directAdd += add;
        }
    }
};
class PricePercentEffect : public Effect {
  public:
    int percent;
    PricePercentEffect(int percent) : percent(percent) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {
        if (strict) {
            strictRule[i]->addRule.buff += percent;
        } else {
            lenientRule[i]->addRule.buff += percent;
        }
    }
};
class NoEffect : public Effect {
  public:
    NoEffect() {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, int i,
                    States &s) const override {}
};

class SingleConditionRule : public Rule {
  public:
    Condition *condition;
    SingleConditionRule(Condition *condition, Effect *effect)
        : Rule(effect), condition(condition) {}
    void operator()(BanquetStrictRule **strictRule,
                    BanquetLenientRule **lenientRule, States &s,
                    int overrideDishStart) const override {
        auto recipes = s.recipe;
        int recipe = (*condition)(s);
        if (overrideDishStart != -1) {
            condition->start = overrideDishStart;
        }
        if (recipe != -1) {
            (*effect)(strictRule, lenientRule, recipe, s);
        }
    }
    ~SingleConditionRule() { delete condition; }
};
struct RuleInfo {
    std::vector<Rule *> rl;
    int bestFull[NUM_GUESTS];
    ~RuleInfo() {
        std::cout << "RuleInfo destructor called" << std::endl;
        for (auto &r : rl) {
            delete r;
        }
    }
};
void banquetRuleGenerated(BanquetRuleTogether *rule, States &s,
                          const RuleInfo &allRules);
bool loadBanquetRule(RuleInfo &, Json::Value &gameData, int ruleID);
#endif