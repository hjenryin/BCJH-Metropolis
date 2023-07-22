#ifndef TYPES_HPP
#define TYPES_HPP
#include "../config.hpp"
#include "Values.hpp"
#include "include/json/json.h"
#include <iostream>

class MaterialCategoryBuff {
  public:
    int vegetable;
    int meat;
    int fish;
    int creation;
    MaterialCategoryBuff() : vegetable(0), meat(0), fish(0), creation(0) {}
    void add(const MaterialCategoryBuff &m) {
        this->vegetable += m.vegetable;
        this->meat += m.meat;
        this->fish += m.fish;
        this->creation += m.creation;
    }
    void print() const {
        std::cout << "MaterialCategoryBuff: Vegetable: " << this->vegetable
                  << "; Meat: " << this->meat << "; Fish: " << this->fish
                  << "; Creation: " << this->creation << std::endl;
    }
};

class FlavorBuff {
  public:
    // Order MATTERS!
    int sweet;
    int salty;
    int sour;
    int bitter;
    int spicy;
    int tasty;
    FlavorBuff() : sweet(0), salty(0), sour(0), bitter(0), spicy(0), tasty(0) {}
    void add(const FlavorBuff &f) {
        this->sweet += f.sweet;
        this->salty += f.salty;
        this->sour += f.sour;
        this->bitter += f.bitter;
        this->spicy += f.spicy;
        this->tasty += f.tasty;
    }
    void print() const {
        std::cout << "FlavorBuff: Sweet: " << this->sweet
                  << "; Salty: " << this->salty << "; Sour: " << this->sour
                  << "; Bitter: " << this->bitter << "; Spicy: " << this->spicy
                  << "; Tasty: " << this->tasty << std::endl;
    }
    int operator*(const FlavorEnum f) const {
        const int *ptr = &this->sweet;
        return ptr[f - FLAVOR_ENUM_START - 1];
    }
};
class Ability {
  public:
    int stirfry = 0;
    int bake = 0;
    int boil = 0;
    int steam = 0;
    int fry = 0;
    int knife = 0;
    Ability() : stirfry(0), bake(0), boil(0), steam(0), fry(0), knife(0) {}
    Ability(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : stirfry(stirfry), bake(bake), boil(boil), steam(steam), fry(fry),
          knife(knife) {}
    Ability(ToolEnum t) {
        if (t == NOT_EQUIPPED || t == NO_TOOL)
            return;
        int *ptr = &this->stirfry;
        ptr[t - ABILITY_ENUM_START] = 100;
    }
    void multiply(double a) {
        int *ptr = &this->stirfry;
        for (int i = 0; i < 6; i++) {
            ptr[i] = int(ptr[i] * a);
        }
    }

    int operator/(const Ability &a) const {
        int grade = 5;
        const int *thisptr = &this->stirfry;
        const int *aptr = &a.stirfry;
        for (int i = 0; i < 6; i++) {
            if (aptr[i] != 0) {
                grade = grade < (thisptr[i] / aptr[i]) ? grade
                                                       : (thisptr[i] / aptr[i]);
            }
        }
        return grade;
    };
    Ability operator+(ToolEnum t) const {
        Ability tmp(t);
        tmp.add(*this);
        return tmp;
    }
    void add(const Ability &a) {
        int *thisptr = &this->stirfry;
        const int *aptr = &a.stirfry;
        for (int i = 0; i < 6; i++) {
            thisptr[i] += aptr[i];
        }
    }
    void add(int a) {
        int *ptr = &this->stirfry;
        for (int i = 0; i < 6; i++) {
            ptr[i] += a;
        }
    }
    Ability operator+(const Ability &a) const {
        Ability tmp(a);
        tmp.add(*this);
        return tmp;
    }
    Ability operator&&(const Ability &a) const {
        Ability tmp;
        const int *thisptr = &this->stirfry;
        const int *aptr = &a.stirfry;
        int *tmpptr = &tmp.stirfry;
        for (int i = 0; i < 6; i++) {
            tmpptr[i] = thisptr[i] && aptr[i];
        }
        return tmp;
    }
    int operator!=(int a) const {
        const int *ptr = &this->stirfry;
        int count = 0;
        for (int i = 0; i < 6; i++) {
            count += (ptr[i] != a);
        }
        return count;
    }
    void print(std::string title, std::string end = "\n",
               bool percent = false) const {
        auto perstr = percent ? "%" : "";
        std::cout
            << title
            << (this->stirfry
                    ? "炒" + std::to_string(this->stirfry) + perstr + " "
                    : "")
            << (this->bake ? "烤" + std::to_string(this->bake) + perstr + " "
                           : "")
            << (this->boil ? "煮" + std::to_string(this->boil) + perstr + " "
                           : "")
            << (this->steam ? "蒸" + std::to_string(this->steam) + perstr + " "
                            : "")
            << (this->fry ? "炸" + std::to_string(this->fry) + perstr + " "
                          : "")
            << (this->knife ? "切" + std::to_string(this->knife) + perstr + " "
                            : "");
        if (!(this->stirfry || this->bake || this->boil || this->steam ||
              this->fry || this->knife))
            std::cout << "无";
        std::cout << end;
    }
    /* Knife, Stirfry, Bake, Boil, Steam, Fry */
    const int operator[](int name) {
        if (name == KNIFE) {
            return this->knife;
        }
        if (name == STIRFRY) {
            return this->stirfry;
        }
        if (name == BAKE) {
            return this->bake;
        }
        if (name == BOIL) {
            return this->boil;
        }
        if (name == STEAM) {
            return this->steam;
        }
        if (name == FRY) {
            return this->fry;
        }
        std::cout << "Ability::operator[]: Error: " << name << std::endl;
        exit(1);
    }
};

class AbilityBuff : public Ability {
  public:
    int basic = 0;
    AbilityBuff() {}
    AbilityBuff(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    void print() const {
        this->Ability::print("AbilityBuff: ", "", true);
        std::cout << " Basic: " << this->basic << std::endl;
    }
    void add(const AbilityBuff &a) {
        this->Ability::add(a);
        this->basic += a.basic;
    }
    void add(const Ability &a) { this->Ability::add(a); }
    void add(int a) { this->Ability::add(a); }
};
class CookAbility : public Ability {

  public:
    CookAbility(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    CookAbility() : Ability() {}
    CookAbility(const Json::Value &v);
    // int operator/(const Ability &a) const;
    void print() const { this->Ability::print("CookAbility: "); }
    int operator*(const AbilityBuff &a) const;
    void add(const CookAbility &a) { this->Ability::add(a); }
    void add(const Ability &a) { this->Ability::add(a); }
    void add(int a) { this->Ability::add(a); }
};
class DiscretizedBuff {
    int data[5] = {0, 0, 0, 0, 0};

  public:
    /*几火/几级就是几，不用减一*/
    int &operator[](int i) { return data[i - 1]; }
    int operator[](int i) const { return data[i - 1]; }
    void add(const DiscretizedBuff &r) {
        for (int i = 0; i < 5; i++)
            this->data[i] += r.data[i];
    }
    void print(const std::string &name) const {
        std::cout << name << ": ";
        for (int i = 0; i < 5; i++) {
            std::cout << data[i] << "% ";
        }
        std::cout << std::endl;
    }
};
class BuffCondition;
class ConditionalBuff;

class Skill {

  public:
    enum Type { SELF, NEXT, PARTIAL };
    Type type = SELF;

    static std::map<int, Skill> skillList;
    CookAbility ability;
    CookAbility cookAbilityPercentBuff;
    AbilityBuff abilityBuff;
    FlavorBuff flavorBuff;
    MaterialCategoryBuff materialBuff;
    DiscretizedBuff rarityBuff;
    DiscretizedBuff gradeBuff; // 几级就填当前那一级，比它高的不用填。

    int pricePercentBuff = 0;
    int baseAddBuff = 0;
    std::vector<ConditionalBuff *> conditionalEffects;
    // Skill(CookAbility ability, AbilityBuff abilityBuff, FlavorBuff
    // flavorBuff,
    //       MaterialCategoryBuff materialBuff, DiscretizedBuff rarityBuff,
    //       DiscretizedBuff gradeBuff, int coinBuff,
    //       std::vector<ConditionalBuff *> conditionalEffects)
    //     : ability(ability), abilityBuff(abilityBuff), flavorBuff(flavorBuff),
    //       materialBuff(materialBuff), rarityBuff(rarityBuff),
    //       gradeBuff(gradeBuff), pricePercentBuff(coinBuff),
    //       conditionalEffects(conditionalEffects) {}
    Skill() {
        this->ability = CookAbility();
        this->cookAbilityPercentBuff = CookAbility();
        this->abilityBuff = AbilityBuff();
        this->flavorBuff = FlavorBuff();
        this->materialBuff = MaterialCategoryBuff();
    }
    Skill getSkill(int id) { return skillList[id]; }
    static void loadJson(const Json::Value &v);
    void operator+=(const Skill &s) {
        this->ability.add(s.ability);
        this->cookAbilityPercentBuff.add(s.cookAbilityPercentBuff);
        this->abilityBuff.add(s.abilityBuff);
        this->flavorBuff.add(s.flavorBuff);
        this->materialBuff.add(s.materialBuff);
        this->rarityBuff.add(s.rarityBuff);
        this->gradeBuff.add(s.gradeBuff);
        this->pricePercentBuff += s.pricePercentBuff;
        this->baseAddBuff += s.baseAddBuff;
        this->conditionalEffects.insert(this->conditionalEffects.end(),
                                        s.conditionalEffects.begin(),
                                        s.conditionalEffects.end());
    }
    Skill operator+(const Skill &s) {
        Skill tmp(*this);
        tmp += s;
        return tmp;
    }
    void print() const {
        this->ability.print();
        this->abilityBuff.print();
        this->flavorBuff.print();
        this->materialBuff.print();
        std::cout << "CoinBuff: " << this->pricePercentBuff << "; ";
        this->rarityBuff.print("RarityBuff");
        this->gradeBuff.print("GradeBuff");
    }
};

class Recipe;
// Records nonconventional buffs
class BuffCondition {

  public:
    const std::string name;
    BuffCondition(const std::string &name = "") : name(name) {}
    virtual int test(const Skill *s, Recipe **r) = 0;
};
class GradeBuffCondition : public BuffCondition {
  public:
    int grade;
    GradeBuffCondition(int grade)
        : grade(grade),
          BuffCondition(std::string("等级做到") + (char)('0' + grade)) {}
    int test(const Skill *s, Recipe **r);
};
class ThreeSameCookAbilityBuffCondition : public BuffCondition {
  public:
    ThreeSameCookAbilityBuffCondition() : BuffCondition("三技法相同") {}
    int test(const Skill *s, Recipe **r);
};

class ConditionalBuff {
  public:
    BuffCondition *conditionFunc;
    Skill skill;
    ConditionalBuff(BuffCondition *conditionFunc, Skill skill)
        : conditionFunc(conditionFunc), skill(skill) {}
    ConditionalBuff() : conditionFunc(NULL), skill(Skill()) {}
};

#endif
