#ifndef TYPES_HPP
#define TYPES_HPP
#include <iostream>
#include "include/json/json.h"
#include "Values.hpp"

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
    bool print() const {
        if (vegetable) {
            std::cout << "菜" << vegetable << "% ";
        }
        if (meat) {
            std::cout << "肉" << meat << "% ";
        }
        if (fish) {
            std::cout << "鱼" << fish << "% ";
        }
        if (creation) {
            std::cout << "面" << creation << "% ";
        }
        if ((vegetable || meat || fish || creation))
            return true;
        else
            return false;
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
    bool print() const {
        if (sweet) {
            std::cout << "甜" << sweet << "% ";
        }
        if (salty) {
            std::cout << "咸" << salty << "% ";
        }
        if (sour) {
            std::cout << "酸" << sour << "% ";
        }
        if (bitter) {
            std::cout << "苦" << bitter << "% ";
        }
        if (spicy) {
            std::cout << "辣" << spicy << "% ";
        }
        if (tasty) {
            std::cout << "鲜" << tasty << "% ";
        }
        if ((sweet || salty || sour || bitter || spicy || tasty))
            return true;
        else
            return false;
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
    AbilityBuff() {}
    AbilityBuff(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    void print() const { this->Ability::print("加成: ", "\t", true); }
};
class CookAbility : public Ability {

  public:
    CookAbility(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    CookAbility() : Ability() {}
    CookAbility(const Json::Value &v);
    // int operator/(const Ability &a) const;
    void print(std::string end = "") const {
        this->Ability::print("技法: ", end);
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
class Skill {

  public:
    enum Type { SELF, NEXT, PARTIAL };
    Type type = SELF;

    static std::map<int, Skill> skillList;
    CookAbility ability;
    AbilityBuff abilityBuff;
    FlavorBuff flavorBuff;
    MaterialCategoryBuff materialBuff;
    RarityBuff rarityBuff;
    int coinBuff;
    Skill(CookAbility ability, AbilityBuff abilityBuff, FlavorBuff flavorBuff,
          MaterialCategoryBuff materialBuff, RarityBuff rarityBuff,
          int coinBuff)
        : ability(ability), abilityBuff(abilityBuff), flavorBuff(flavorBuff),
          materialBuff(materialBuff), coinBuff(coinBuff),
          rarityBuff(rarityBuff) {}
    Skill() {
        this->ability = CookAbility();
        this->abilityBuff = AbilityBuff();
        this->flavorBuff = FlavorBuff();
        this->materialBuff = MaterialCategoryBuff();
        this->coinBuff = 0;
    }
    Skill getSkill(int id) { return skillList[id]; }
    static void loadJson(const Json::Value &v);
    void operator+=(const Skill &s) {
        this->ability.add(s.ability);
        this->abilityBuff.add(s.abilityBuff);
        this->flavorBuff.add(s.flavorBuff);
        this->materialBuff.add(s.materialBuff);
        this->coinBuff += s.coinBuff;
        for (int i = 0; i < 5; i++) {
            this->rarityBuff[i + 1] += s.rarityBuff[i + 1];
        }
    }
    Skill operator+(const Skill &s) {
        Skill tmp(*this);
        tmp += s;
        return tmp;
    }
    void print() const {
        this->ability.print();
        this->abilityBuff.print();
        if (flavorBuff.print())
            std::cout << "\t";
        if (materialBuff.print())
            std::cout << "\t";
        if (coinBuff)
            std::cout << "金币加成: " << this->coinBuff << "%; ";
        this->rarityBuff.print();
        std::cout << std::endl;
    }
};

#endif
