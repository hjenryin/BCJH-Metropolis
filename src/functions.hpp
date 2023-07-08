#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Calculator.hpp"
#include <algorithm>
#include <cmath>
#include "../config.hpp"
#include "../src/Chef.hpp"
#include "banquetRuleGen.hpp"
#include "types.hpp"
class SARunner;

namespace r00 {
// void unrepeatedRandomChef(CList *, Chef **&, int);
void unrepeatedRandomRecipe(std::vector<Recipe *> *, Recipe **, int, int);
} // namespace r00

void swap(Recipe *&a, Recipe *&b);

template <typename T> bool inArray(T **array, int size, T *value) {
    for (int i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}
bool chefCanCook(Chef *, Recipe *);

class Randomizer {
  public:
    CList *c;
    RList *r;
    const RuleInfo *rl;
    Randomizer(CList *c, RList *r, const RuleInfo *rl) : c(c), r(r), rl(rl) {}
    Randomizer() {}
    virtual States operator()(States s) const = 0;

  protected:
    void unrepeatedRandomRecipe(std::vector<Recipe *> *rl, Recipe **rs,
                                int size, int index) const;
};
class RecipeRandomizer : public Randomizer {
  public:
    RecipeRandomizer(CList *c, RList *r, const RuleInfo *rl)
        : Randomizer(c, r, rl) {}
    States operator()(States s) const override;

  private:
    States swapRecipe(States &s) const;
    States randomRecipe(States &s) const;
};
class ChefRandomizer : public Randomizer {
  public:
    ChefRandomizer(CList *c, RList *r, const RuleInfo *rl)
        : Randomizer(c, r, rl) {}
    States operator()(States s) const override;

  private:
    States &randomChef(States &s) const;
    States &swapChefTool(States &s) const;
};
typedef States (*RandomMove)(States, CList *, RList *);

namespace e0 {
int sumPrice(const RuleInfo &rl, States s, CList *c = NULL, RList *r = NULL,
             int log = false, bool exactChefTool = false);
}
namespace e {

typedef int (*GetEnergy)(const RuleInfo &, States, CList *, RList *, bool);
int getTotalPrice(const RuleInfo &rl, States s, CList *c, RList *r,
                  bool vb = false);
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
States perfectChef(const RuleInfo &rl, States &s, CList *c);
#endif