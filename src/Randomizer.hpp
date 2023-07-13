#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

#include "functions.hpp"
class Randomizer {
  public:
    CList *c;
    RList *r;
    int success;
    int calls;
    Randomizer(CList *c, RList *r) : c(c), r(r), success(0), calls(0) {}
    Randomizer() : success(0), calls(0) {}
    virtual States operator()(States s) = 0;
    virtual ~Randomizer() {}

  protected:
    bool swapRecipe(States &s) const;
    bool unrepeatedRandomRecipe(Skill &skill, Recipe **rs, int size, int index,
                                int repeats = RANDOM_SEARCH_TIMEOUT) const;
};
class RecipeRandomizer : public Randomizer {

  public:
    RecipeRandomizer(CList *c, RList *r) : Randomizer(c, r) {}
    States operator()(States s) override;

  private:
    bool randomRecipe(States &s) const;
};
class ChefRandomizer : public Randomizer {
    int targetScore;

  public:
    ChefRandomizer(CList *c, RList *r, int targetScore)
        : Randomizer(c, r), targetScore(targetScore) {}
    States operator()(States s) override;

  private:
    bool randomChef(States &s) const;
    bool swapChefTool(States &s) const;
};

#endif