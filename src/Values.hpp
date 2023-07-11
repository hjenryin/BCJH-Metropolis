#ifndef VALUE_HPP
#define VALUE_HPP
#include <iostream>
#include "include/json/json.h"
#define TOOL_ENUM_START 0
#define FLAVOR_ENUM_START 0x100
enum ToolEnum {
    NO_TOOL = TOOL_ENUM_START,
    NOT_EQUIPPED,
    STIRFRY,
    BAKE,
    BOIL,
    STEAM,
    FRY,
    KNIFE,
    TOOL_ENUM_END
};
const int ABILITY_ENUM_START = TOOL_ENUM_START + 2;
const int ABILITY_ENUM_END = TOOL_ENUM_END;
enum FlavorEnum {
    UNIDENTIFIED_FLAVOR = FLAVOR_ENUM_START,
    // Order MATTERS!
    SWEET,
    SALTY,
    SOUR,
    BITTER,
    SPICY,
    TASTY,
    FLAVOR_ENUM_END
};

enum ToolFileType {
    NO_FILE__NO_TOOL,
    EMPTY_FILE__NOT_EQUIPPED,
    CUSTOMIZE_TOOL
};

#endif