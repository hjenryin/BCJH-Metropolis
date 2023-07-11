#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Calculator.hpp"
#include <algorithm>
#include <cmath>
#include "../config.hpp"
#include "../src/Chef.hpp"
#include "States.hpp"
#include "exception.hpp"
class SARunner;
extern double randomRecipeTime;
extern double randomChefTime;
extern double banquetRuleTime;

void swap(Recipe *&a, Recipe *&b);

template <typename T> bool inArray(T **array, int size, T *value) {
    for (int i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}
inline bool hasRepeatedRecipe(Recipe **recipelist) {
#ifndef DEBUG_REPEAT_CHEF
    return false;
#else
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        for (int j = i + 1; j < NUM_CHEFS * DISH_PER_CHEF; j++) {
            if (recipelist[i] == recipelist[j]) {
                return true;
            }
        }
    }
    return false;
#endif
}
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
typedef States (*RandomMove)(States, CList *, RList *);

namespace e0 {
int sumPrice(States s, CList *c = NULL, RList *r = NULL, int log = false,
             bool exactChefTool = false);
}
namespace e {

typedef int (*GetEnergy)(States, CList *, RList *, bool);
int getTotalPrice(States s, CList *c, RList *r, bool vb = false);
} // namespace e

namespace f {
typedef double (*CoolingSchedule)(int, int, double, double);
double exponential_multiplicative(int stepMax, int step, double tMax,
                                  double tMin);
double linear(int stepMax, int step, double tMax, double tMin);
double t_dist_fast(int stepMax, int step, double tMax, double tMin);
double t_dist_slow(int stepMax, int step, double tMax, double tMin);
double linear_mul(int stepMax, int step, double tMax, double tMin);
double zipf(int stepMax, int step, double tMax, double tMin);
double one_over_n(int stepMax, int step, double tMax, double tMin);
} // namespace f
States perfectChef(States &s, CList *c);
#endif