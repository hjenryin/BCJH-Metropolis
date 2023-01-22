#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Calculator.hpp"
#include <algorithm>
#include <cmath>
#include "../config.hpp"
typedef std::map<Chef *, std::vector<Recipe *>> CRPairs;
typedef std::map<int, Chef> CList;
typedef std::map<int, Recipe> RList;
class SARunner;
struct States {
    Chef *chef[NUM_CHEFS];
    Recipe *recipe[DISH_PER_CHEF * NUM_CHEFS];
};

void swap(Recipe *&a, Recipe *&b);

template <typename T> bool inArray(T **array, int size, T *value);
bool chefCanCook(CRPairs *, Chef *, Recipe *);

namespace r {
typedef States (*RandomMove)(States, CList *, RList *, CRPairs *);
States randomRecipe(States, CList *, RList *, CRPairs *);
States randomChef(States, CList *, RList *, CRPairs *);

} // namespace r
namespace e {

typedef int (*GetEnergy)(States, CList *, RList *, CRPairs *, bool);

int sumPrice(States s, CList *c, RList *r, CRPairs *p, bool vb = false);
int getOptimalPrice(States s, CList *c, RList *r, CRPairs *p, bool vb = false);
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
#endif