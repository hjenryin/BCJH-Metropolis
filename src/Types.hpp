#ifndef TYPES_HPP
#define TYPES_HPP
#include <iostream>
#include "include/json/json.h"
#include "Values.hpp"
#include "utils/Printer.hpp"
struct Tool {
    ToolEnum type = NOT_EQUIPPED;
    int value = 100;
};
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
    std::vector<Printer> getPrinters() const {
        std::vector<Printer> p;
        p.push_back(Printer("菜", vegetable));
        p.push_back(Printer("肉", meat));
        p.push_back(Printer("鱼", fish));
        p.push_back(Printer("面", creation));
        return p;
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
    std::vector<Printer> getPrinters() const {
        std::vector<Printer> p;
        p.push_back(Printer("甜", sweet));
        p.push_back(Printer("咸", salty));
        p.push_back(Printer("酸", sour));
        p.push_back(Printer("苦", bitter));
        p.push_back(Printer("辣", spicy));
        p.push_back(Printer("鲜", tasty));
        return p;
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
    Ability(Tool tool) {
        ToolEnum t = tool.type;
        int value = tool.value;
        if (t == NOT_EQUIPPED || t == NO_TOOL)
            return;
        int *ptr = &this->stirfry;
        ptr[t - ABILITY_ENUM_START] = value;
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
    Ability operator+(Tool t) const {
        Ability tmp(t);
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
    std::vector<Printer> getPrinters(bool percent) const {
        std::vector<Printer> p;
        p.push_back(Printer("炒", stirfry, percent));
        p.push_back(Printer("烤", bake, percent));
        p.push_back(Printer("煮", boil, percent));
        p.push_back(Printer("蒸", steam, percent));
        p.push_back(Printer("炸", fry, percent));
        p.push_back(Printer("切", knife, percent));
        return p;
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
    AbilityBuff() {}
    AbilityBuff(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
};
class CookAbility : public Ability {

  public:
    CookAbility(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    CookAbility() : Ability() {}
    CookAbility(const Json::Value &v);
    // int operator/(const Ability &a) const;
    void print(std::string end = "\t") const {
        Printer p("技法");
        p.noValue();
        p.add(Ability::getPrinters(false));
        p.print("", " ", end.c_str());
    }
    int operator*(const AbilityBuff &a) const;
};
class RarityBuff {
    int data[5] = {0, 0, 0, 0, 0};

  public:
    /*几火就是几，不用减一*/
    int &operator[](int i) { return data[i - 1]; }
    int operator[](int i) const { return data[i - 1]; }
    bool print() const {
        if (data[0] || data[1] || data[2] || data[3] || data[4]) {
            std::cout << "稀有度加成: ";
            for (int i = 0; i < 5; i++) {
                std::cout << data[i] << "% ";
            }
            return true;
        } else {
            return false;
        }
    }
};
class DiscretizedBuff {
    int data[5] = {0, 0, 0, 0, 0};

  public:
    typedef std::tuple<bool, bool, bool, bool, bool> Mask;
    /*几火/几级就是几，不用减一*/
    int &operator[](int i) { return data[i - 1]; }
    int operator[](int i) const { return data[i - 1]; }
    void add(const DiscretizedBuff &r) {
        for (int i = 0; i < 5; i++)
            this->data[i] += r.data[i];
    }
    void print(const std::string &name) const {
        int sum = 0;
        for (int i = 0; i < 5; i++) {
            sum += data[i];
        }
        if (sum > 0) {
            std::cout << name << ": ";
            for (int i = 0; i < 5; i++) {
                std::cout << data[i] << "% ";
            }
        }
    }
    void masked_add(Mask m, int value) {
        bool mask[5] = {std::get<0>(m), std::get<1>(m), std::get<2>(m),
                        std::get<3>(m), std::get<4>(m)};
        for (int i = 0; i < 5; i++) {
            if (mask[i]) {
                this->data[i] += value;
            }
        }
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
    int multiToolEffect = 1;   // 1: 正常, 2: 翻倍

    int pricePercentBuff = 0;
    int baseAddBuff = 0;
    std::vector<ConditionalBuff *> conditionalEffects;
    Skill() = default;
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
        this->ability.print("\t");
        Printer p("\n加成");
        p.noValue();
        p.add(abilityBuff.getPrinters(true));
        p.add(flavorBuff.getPrinters());
        p.add(materialBuff.getPrinters());
        p.add("金币", this->pricePercentBuff, true);
        p.add("基础售价", this->baseAddBuff, true);
        p.print("", "  ", "\t");
        this->rarityBuff.print("菜品火数加成");
        this->gradeBuff.print("菜品品级加成");

        std::cout << std::endl;
    }
    ~Skill() {} // conditionalEffects should be handled manually.
};
class Recipe;
// Records nonconventional buffs
class BuffCondition {

  public:
    const std::string name;
    BuffCondition(const std::string &name = "") : name(name) {}
    virtual int test(const Skill *s, Recipe **r) = 0;
    virtual ~BuffCondition() {}
};
class GradeBuffCondition : public BuffCondition {
  public:
    int grade;
    GradeBuffCondition(int grade)
        : grade(grade),
          BuffCondition(std::string("等级做到") + (char)('0' + grade)) {}
    int test(const Skill *s, Recipe **r) override;
    ~GradeBuffCondition() override {}
};
class ThreeSameCookAbilityBuffCondition : public BuffCondition {
  public:
    ThreeSameCookAbilityBuffCondition() : BuffCondition("三技法相同") {}
    int test(const Skill *s, Recipe **r) override;
    ~ThreeSameCookAbilityBuffCondition() override {}
};

class ConditionalBuff {
  public:
    BuffCondition *conditionFunc;
    Skill skill;
    ConditionalBuff(BuffCondition *conditionFunc, Skill skill)
        : conditionFunc(conditionFunc), skill(skill) {}
    ConditionalBuff() : conditionFunc(NULL), skill(Skill()) {}
    ~ConditionalBuff() {
        // if (conditionFunc != NULL) // wierd, shouldn't need this.
        delete conditionFunc;
    }
};
#endif
