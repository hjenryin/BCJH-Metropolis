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

int sumPrice(States s, int log = false, int toolValue = 100,
             int chefIdForTool = -1);
void exactChefTool(States &s);
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
States perfectChef(States s, CList *c);
#endif