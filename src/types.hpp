#ifndef TYPES_HPP
#define TYPES_HPP
#include "Chef.hpp"
#include "Recipe.hpp"
#include "Values.hpp"
#include "../config.hpp"
struct States {
    Chef *chef[NUM_CHEFS];
    ToolEnum toolCKPT[NUM_CHEFS];
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS];
};

#endif