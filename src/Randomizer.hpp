#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

#include "functions.hpp"
class Randomizer {
  public:
    CList *c;
    RList *r;
    const RuleInfo *rl;
    int success;
    int calls;
    Randomizer(CList *c, RList *r, const RuleInfo *rl)
        : c(c), r(r), rl(rl), success(0), calls(0) {}
    Randomizer() : success(0), calls(0) {}
    virtual void operator()(States &s) = 0;
    virtual ~Randomizer() {}

  protected:
    bool swapRecipe(States &s) const;
    bool unrepeatedRandomRecipe(const Skill &skill, Recipe **rs, int size, int index,
                                int repeats = RANDOM_SEARCH_TIMEOUT) const;
};
class RecipeRandomizer : public Randomizer {

  public:
    RecipeRandomizer(CList *c, RList *r, const RuleInfo *rl)
        : Randomizer(c, r, rl) {}
    void operator()(States &s) override;

  private:
    bool randomRecipe(States &s) const;
};
class ChefRandomizer : public Randomizer {
    int targetScore;

  public:
    ChefRandomizer(CList *c, RList *r, const RuleInfo *rl, int targetScore)
        : Randomizer(c, r, rl), targetScore(targetScore) {}
    void operator()(States &s) override;

  private:
    bool randomChef(States &s) const;
    bool swapChefTool(States &s) const;
};

#endif