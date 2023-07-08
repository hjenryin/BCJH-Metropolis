#ifndef VALUE_HPP
#define VALUE_HPP
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
    void print() {
        std::cout << "MaterialCategoryBuff: Vegetable: " << this->vegetable
                  << "; Meat: " << this->meat << "; Fish: " << this->fish
                  << "; Creation: " << this->creation << std::endl;
    }
};

class FlavorBuff {
  public:
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
    void print() {
        std::cout << "FlavorBuff: Sweet: " << this->sweet
                  << "; Salty: " << this->salty << "; Sour: " << this->sour
                  << "; Bitter: " << this->bitter << "; Spicy: " << this->spicy
                  << "; Tasty: " << this->tasty << std::endl;
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
        this->stirfry = int(this->stirfry * a);
        this->bake = int(this->bake * a);
        this->boil = int(this->boil * a);
        this->steam = int(this->steam * a);
        this->fry = int(this->fry * a);
        this->knife = int(this->knife * a);
    }
    void add(const Ability &a) {
        this->stirfry += a.stirfry;
        this->bake += a.bake;
        this->boil += a.boil;
        this->steam += a.steam;
        this->fry += a.fry;
        this->knife += a.knife;
    }
    void add(int a) {
        this->stirfry += a;
        this->bake += a;
        this->boil += a;
        this->steam += a;
        this->fry += a;
        this->knife += a;
    }
    void print(std::string title, std::string end = "\n",
               bool percent = false) {
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
};

class AbilityBuff : public Ability {
  public:
    AbilityBuff() {}
    AbilityBuff(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    void print() { this->Ability::print("AbilityBuff: ", "\n", true); }
};
class CookAbility : public Ability {

  public:
    CookAbility(int stirfry, int bake, int boil, int steam, int fry, int knife)
        : Ability(stirfry, bake, boil, steam, fry, knife) {}
    CookAbility() : Ability() {}
    CookAbility(Json::Value &v);
    int operator/(const Ability &a);
    void print() { this->Ability::print("CookAbility: "); }
    int operator*(const AbilityBuff &a);
};
class RarityBuff {
    int data[5] = {0, 0, 0, 0, 0};

  public:
    /*几火就是几，不用减一*/
    int &operator[](int i) { return data[i - 1]; }
    void print() {
        std::cout << "RarityBuff: ";
        for (int i = 0; i < 5; i++) {
            std::cout << data[i] << "% ";
        }
        std::cout << std::endl;
    }
};
class Skill {
  public:
    enum Type { SELF, NEXT, PARTIAL };
    Type type = SELF;
    bool next = false;
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
    static void loadJson(Json::Value &v);
    void add(Skill &s) {
        this->ability.add(s.ability);
        this->abilityBuff.add(s.abilityBuff);
        this->flavorBuff.add(s.flavorBuff);
        this->materialBuff.add(s.materialBuff);
        this->coinBuff += s.coinBuff;
        for (int i = 0; i < 5; i++) {
            this->rarityBuff[i + 1] += s.rarityBuff[i + 1];
        }
    }
    Skill operator+(Skill &s) {
        Skill tmp(*this);
        tmp.add(s);
        return tmp;
    }
    void print() {
        this->ability.print();
        this->abilityBuff.print();
        this->flavorBuff.print();
        this->materialBuff.print();
        std::cout << "CoinBuff: " << this->coinBuff << "; ";
        this->rarityBuff.print();
    }
};
enum ToolEnum {
    NO_TOOL = -2,
    NOT_EQUIPPED = -1,
    STIRFRY,
    BAKE,
    BOIL,
    STEAM,
    FRY,
    KNIFE
};
enum FlavorEnum { UNIDENTIFIED = -1, SWEET, SALTY, SOUR, BITTER, SPICY, TASTY };
enum ToolFileType {
    NO_FILE__NO_TOOL,
    EMPTY_FILE__NOT_EQUIPPED,
    CUSTOMIZE_TOOL
};
#endif