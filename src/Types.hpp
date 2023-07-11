#ifndef TYPES_HPP
#define TYPES_HPP
#include <iostream>
#include "include/json/json.h"

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
    int stirfry;
    int bake;
    int boil;
    int steam;
    int fry;
    int knife;
    Ability() : stirfry(0), bake(0), boil(0), steam(0), fry(0), knife(0) {}
    Ability(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : stirfry(stirfry), bake(bake), boil(boil), steam(steam), fry(fry),
          knife(knife) {}
    void multiply(double a) {
        int *ptr = &this->stirfry;
        for (int i = 0; i < 6; i++) {
            ptr[i] = int(ptr[i] * a);
        }
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
    void add(int a){ this->Ability::add(a); }
};
class CookAbility : public Ability {

  public:
    Ability percent = Ability();
    CookAbility(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    CookAbility() : Ability() {}
    CookAbility(const Json::Value &v);
    int operator/(const Ability &a) const;
    void print() const {
        this->Ability::print("CookAbility: ");
        this->percent.print("CookAbilityPercent: ");
    }
    int operator*(const AbilityBuff &a) const;
    void add(const CookAbility &a) {
        this->Ability::add(a);
        this->percent.add(a.percent);
    }
    void add(const Ability &a) { this->Ability::add(a); }
    void add(int a) { this->Ability::add(a); }
    void handle_percent() {
        if (this->percent.stirfry)
            this->stirfry = int(ceil(this->stirfry * (this->percent.stirfry + 100) / 100.0));
        if (this->percent.bake)
            this->bake = int(ceil(this->bake * (this->percent.bake + 100) / 100.0));
        if (this->percent.boil)
            this->boil = int(ceil(this->boil * (this->percent.boil + 100) / 100.0));
        if (this->percent.steam)
            this->steam = int(ceil(this->steam * (this->percent.steam + 100) / 100.0));
        if (this->percent.fry)
            this->fry = int(ceil(this->fry * (this->percent.fry + 100) / 100.0));
        if (this->percent.knife)
            this->knife = int(ceil(this->knife * (this->percent.knife + 100) / 100.0));
    }
};
class RarityBuff {
    int data[5] = {0, 0, 0, 0, 0};

  public:
    /*几火就是几，不用减一*/
    int &operator[](int i) { return data[i - 1]; }
    int operator[](int i) const { return data[i - 1]; }
    void add(const RarityBuff &r) { for (int i = 0; i < 5; i++) this->data[i] += r.data[i]; }
    void print() const {
        std::cout << "RarityBuff: ";
        for (int i = 0; i < 5; i++) {
            std::cout << data[i] << "% ";
        }
        std::cout << std::endl;
    }
};
class OtherBuff {
  public:
    DishBuff ExcessCookbookNum;
    DishBuff Rank;
    OtherBuff() {
        this->ExcessCookbookNum.dishNum = -1;
        this->Rank.dishNum = -1;
    }
    void add(const OtherBuff &s) {
        if (~s.ExcessCookbookNum.dishNum) {
            if (~this->ExcessCookbookNum.dishNum) {
                this->ExcessCookbookNum.dishBuff += s.ExcessCookbookNum.dishBuff;
            } else {
                this->ExcessCookbookNum.dishNum = s.ExcessCookbookNum.dishNum;
                this->ExcessCookbookNum.dishBuff = s.ExcessCookbookNum.dishBuff;
            }
        }
        if (~s.Rank.dishNum) {
            if (~this->Rank.dishNum) {
                this->Rank.dishBuff += s.Rank.dishBuff;
            } else {
                this->Rank.dishNum = s.Rank.dishNum;
                this->Rank.dishBuff = s.Rank.dishBuff;
            }
        }
    }
    void print() const {
        if (~this->ExcessCookbookNum.dishNum)
            std::cout << "ExcessCookbookNum: " << this->ExcessCookbookNum.dishNum 
                    << "(" << this->ExcessCookbookNum.dishBuff << ")" << std::endl;
        if (~this->Rank.dishNum)
            std::cout << "Rank: " << this->Rank.dishNum 
                    << "(" << this->Rank.dishBuff << ")" << std::endl;
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
    OtherBuff otherBuff;
    int coinBuff;
    Skill(CookAbility ability, AbilityBuff abilityBuff, FlavorBuff flavorBuff,
          MaterialCategoryBuff materialBuff, RarityBuff rarityBuff, OtherBuff otherBuff,
          int coinBuff)
        : ability(ability), abilityBuff(abilityBuff), flavorBuff(flavorBuff),
          materialBuff(materialBuff), rarityBuff(rarityBuff), otherBuff(otherBuff),
          coinBuff(coinBuff) {}
    Skill() {
        this->ability = CookAbility();
        this->abilityBuff = AbilityBuff();
        this->flavorBuff = FlavorBuff();
        this->materialBuff = MaterialCategoryBuff();
        this->otherBuff = OtherBuff();
        this->coinBuff = 0;
    }
    Skill getSkill(int id) { return skillList[id]; }
    static void loadJson(const Json::Value &v);
    void operator+=(const Skill &s) {
        this->ability.add(s.ability);
        this->abilityBuff.add(s.abilityBuff);
        this->flavorBuff.add(s.flavorBuff);
        this->materialBuff.add(s.materialBuff);
        this->rarityBuff.add(s.rarityBuff);
        this->otherBuff.add(s.otherBuff);
        this->coinBuff += s.coinBuff;
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
        std::cout << "CoinBuff: " << this->coinBuff << "; ";
        this->rarityBuff.print();
        this->otherBuff.print();
    }
};

#endif
