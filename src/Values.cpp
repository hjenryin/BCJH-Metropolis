#include "Values.hpp"
#include <string>

int getEnum(const std::string &s) {
    if (s == "Stirfry") {
        return STIRFRY;
    } else if (s == "Bake") {
        return BAKE;
    } else if (s == "Fry") {
        return FRY;
    } else if (s == "Boil") {
        return BOIL;
    } else if (s == "Steam") {
        return STEAM;
    } else if (s == "Knife") {
        return KNIFE;
    } else if (s == "Sweet") {
        return SWEET;
    } else if (s == "Sour") {
        return SOUR;
    } else if (s == "Bitter") {
        return BITTER;
    } else if (s == "Tasty") {
        return TASTY;
    } else if (s == "Spicy") {
        return SPICY;
    } else if (s == "Salty") {
        return SALTY;
    }
    return -1;
}
