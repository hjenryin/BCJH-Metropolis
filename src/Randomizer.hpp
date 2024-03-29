#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

#include "functions.hpp"
inline bool debugIntegrity(States &s) {
#ifndef DEBUG_INTEGRITY
    return true;
#else
    bool result = true;
    if (!s.capable()) {
        std::cout << "NOT CAPABLE" << std::endl;
        result = false;
    }
    if (s.repeatedRecipe()) {
        std::cout << "REPEATED RECIPE" << std::endl;
        result = false;
    }
    if (s.repeatedChef()) {
        std::cout << "REPEATED CHEF" << std::endl;
        result = false;
    }
    if (result == false) {
        throw std::runtime_error("Integrity check failed");
    }
    return result;
#endif
}

class Randomizer {
  public:
    const CList *c;
    RList *r;
    const RuleInfo *rl;
    int success;
    int calls;
    Randomizer(const CList *c, RList *r, const RuleInfo *rl)
        : c(c), r(r), rl(rl), success(0), calls(0) {}
    Randomizer() : success(0), calls(0) {}
    virtual States operator()(States s) = 0;
    virtual ~Randomizer() {}

  protected:
    bool swapRecipe(States &s) const;
    bool unrepeatedRandomRecipe(const Skill &skill, Recipe **rs, int size,
                                int index,
                                int repeats = RANDOM_SEARCH_TIMEOUT) const;
};
class RecipeRandomizer : public Randomizer {

  public:
    RecipeRandomizer(const CList *c, RList *r, const RuleInfo *rl)
        : Randomizer(c, r, rl) {}
    States operator()(States s) override;
    ~RecipeRandomizer() override {}

  private:
    bool randomRecipe(States &s) const;
};
class ChefRandomizer : public Randomizer {
    int targetScore;

  public:
    ChefRandomizer(const CList *c, RList *r, const RuleInfo *rl,
                   int targetScore)
        : Randomizer(c, r, rl), targetScore(targetScore) {}
    States operator()(States s) override;
    ~ChefRandomizer() override {}

  private:
    bool randomChef(States &s) const;
    bool swapChefTool(States &s) const;
};

#endif