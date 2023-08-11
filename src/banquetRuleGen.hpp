#ifndef BANQUET_RULE_GEN_HPP
#define BANQUET_RULE_GEN_HPP
#include "Recipe.hpp"
#include "../config.hpp"
#include "Calculator.hpp"
#include "Types.hpp"
#include <cassert>
#include "utils/json.hpp"
#include "States.hpp"
extern double generateBanquetRuleTime;
class Effect {
  public:
    bool strict;
    virtual void operator()(BanquetRuleTogether *brt, int dishNum,
                            States &s) const = 0;

    Effect(bool strict = false) : strict(strict) {}
    virtual ~Effect(){};
};
class Rule {
  public:
    Effect *effect;
    Rule(Effect *effect) : effect(effect) {}
    virtual void operator()(BanquetRuleTogether *brt, States &s,
                            int overrideDishStart = -1) const = 0;
    virtual ~Rule() { delete effect; }
};

class Condition {
  public:
    int start;
    int lasts;

    virtual int operator()(States &s, int overrideDishStart = -1) const = 0;
    Condition(int start, int lasts) : start(start), lasts(lasts) {}
    Condition() : start(0), lasts(0) {}
    virtual ~Condition(){};
};

class RarityCondition : public Condition {
  public:
    int rarity;
    RarityCondition(int start, int rarity, int lasts = DISH_PER_CHEF,
                    bool strict = false)
        : Condition(start, lasts), rarity(rarity) {}
    int operator()(States &s, int overrideDishStart = -1) const override {
        int start = overrideDishStart == -1 ? this->start : overrideDishStart;
        auto &recipes = s.recipe;
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
    ToolEnum skill;
    SkillCondition(int start, std::string skill, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts) {
        this->skill = ToolEnum(getEnum(skill));
    }
    int operator()(States &s, int overrideDishStart = -1) const override {
        int start = overrideDishStart == -1 ? this->start : overrideDishStart;
        auto &recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            if (recipes[i]->cookAbility[(int)skill] > 0) {
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
    int operator()(States &s, int overrideDishStart = -1) const override {
        assert(overrideDishStart == -1);
        return start + order - 1;
    }
};
class FlavorCondition : public Condition {
  public:
    FlavorEnum flavor;
    FlavorCondition(int start, std::string flavor, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts) {
        this->flavor = FlavorEnum(getEnum(flavor));
    }
    int operator()(States &s, int overrideDishStart = -1) const override {
        int start = overrideDishStart == -1 ? this->start : overrideDishStart;
        auto &recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            if (recipes[i]->flavor == flavor) {
                return i;
            }
        }
        return -1;
    }
};
class GroupCondition : public Condition {
  public:
    int group;
    bool flavor;
    GroupCondition(int start, std::string groupStr, int lasts = DISH_PER_CHEF)
        : Condition(start, lasts) {
        int group = getEnum(groupStr);
        this->group = group;
        if (group >= ABILITY_ENUM_START && group <= ABILITY_ENUM_END) {
            this->flavor = false;
        } else if (group >= FLAVOR_ENUM_START && group <= FLAVOR_ENUM_END) {
            this->flavor = true;
        } else {
            std::cout << "Unknown group: " << groupStr << std::endl;
            exit(1);
        }
    }
    int operator()(States &s, int overrideDishStart = -1) const override {
        assert(overrideDishStart == -1);
        auto &recipes = s.recipe;
        if (flavor) {
            for (int i = start; i < start + lasts; i++) {
                if ((int)recipes[i]->flavor != group) {
                    return -1;
                }
            }
        } else {
            for (int i = start; i < start + lasts; i++) {
                if (!(recipes[i]->cookAbility[group] > 0)) {
                    return -1;
                }
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
    int operator()(States &s, int overrideDishStart = -1) const override {
        int start = overrideDishStart == -1 ? this->start : overrideDishStart;
        auto skills = s.getSkills();
        auto &recipes = s.recipe;
        for (int i = start; i < start + lasts; i++) {
            auto &skill = skills[i / DISH_PER_CHEF];
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
    int operator()(States &s, int overrideDishStart = -1) const override {
        int start = overrideDishStart == -1 ? this->start : overrideDishStart;
        return start;
    }
};

class IntentAddEffect : public Effect {
  public:
    IntentAddEffect() {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        // assert(strict == false);
        brt[i].lenientRule.oneMore();
    }
};
class NextRuleEffect : public Effect {
  public:
    Rule *rule;
    NextRuleEffect(Rule *rule, bool strict = false) : rule(rule) {
        rule->effect->strict = strict;
    }
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if ((i + 1) % DISH_PER_CHEF != 0) {
            (*rule)(brt, s, i + 1);
        }
    }
    ~NextRuleEffect() override { delete rule; }
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
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        for (int j = 0; j < len; j++) {
            (*rule)(brt, s, i + j);
        }
    }
    ~CreateRulesEffect() override { delete rule; }
};
class FullAddEffect : public Effect {
  public:
    int full;
    FullAddEffect(int full) : full(full) {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if (strict) {
            brt[i].strictRule.addRule.full += full;
        } else {
            brt[i].lenientRule.addRule.full += full;
        }
    }
};
class FullSetEffect : public Effect {
  public:
    int full;
    FullSetEffect(int full) : full(full) {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if (strict) {
            brt[i].strictRule.addRule.full = full;
            brt[i].strictRule.addRule.fullAdd = false;
        } else {
            brt[i].lenientRule.addRule.full = full;
            brt[i].lenientRule.addRule.fullAdd = false;
        }
    }
};

class BasePricePercentEffect : public Effect {
  public:
    int percent;
    BasePricePercentEffect(int percent) : percent(percent) {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if (strict) {
            brt[i].strictRule.baseRule.buff += percent;
        } else {
            brt[i].lenientRule.baseRule.buff += percent;
        }
    }
};
class BasePriceAddEffect : public Effect {
  public:
    int add;
    BasePriceAddEffect(int add) : add(add) {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if (strict) {
            brt[i].strictRule.baseRule.directAdd += add;
        } else {
            brt[i].lenientRule.baseRule.directAdd += add;
        }
    }
};
class PricePercentEffect : public Effect {
  public:
    int percent;
    PricePercentEffect(int percent) : percent(percent) {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
        if (strict) {
            brt[i].strictRule.addRule.buff += percent;
        } else {
            brt[i].lenientRule.addRule.buff += percent;
        }
    }
};
class NoEffect : public Effect {
  public:
    NoEffect() {}
    void operator()(BanquetRuleTogether *brt, int i, States &s) const override {
    }
};

class SingleConditionRule : public Rule {
  public:
    Condition *condition;
    SingleConditionRule(Condition *condition, Effect *effect)
        : Rule(effect), condition(condition) {}
    void operator()(BanquetRuleTogether *brt, States &s,
                    int overrideDishStart = -1) const override {
        auto recipes = s.recipe;
        int start = overrideDishStart == -1 ? -1 : overrideDishStart;
        int recipe = (*condition)(s, start);
        if (recipe != -1) {
            (*effect)(brt, recipe, s);
        }
    }
    ~SingleConditionRule() override { delete condition; }
};
struct RuleInfo {
    std::vector<Rule *> rl;
    int bestFull[NUM_GUESTS];
    ~RuleInfo() {
        // std::cout << "RuleInfo destructor called" << std::endl;
        for (auto &r : rl) {
            delete r;
        }
    }
};
void banquetRuleGenerated(BanquetRuleTogether *rule, States &s,
                          const RuleInfo &allRules);
int loadBanquetRule(RuleInfo &, const Json::Value &gameData, int ruleID,
                    bool print = false);
int loadBanquetRuleFromInput(RuleInfo &, const Json::Value &ruleData,
                             bool print = false);
#endif