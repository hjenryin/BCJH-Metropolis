#include "functions.hpp"
#include "SARunner.hpp"
#include "../config.hpp"
#include "exception.hpp"
#include "activityRule.hpp"
#include <cassert>
#include "banquetRuleGen.hpp"

extern double generateBanquetRuleTime, generateBanquetRuleTimeOut;
extern double calculatePriceTime, calculatePriceTimeOut;

/**
 * @warning this function involves a large copy constructor.
 * @return whether after deduction, the price is still the same
 */
bool deductTool(const RuleInfo &rl, States s, int chefId, int deduction) {
    Chef chef(*s[chefId]);
    int tool = chef.getTool();
    int *cookAbility;
    switch (tool) {
    case STIRFRY:
        cookAbility = &chef.skill->ability.stirfry;
        break;
    case BOIL:
        cookAbility = &chef.skill->ability.boil;
        break;
    case FRY:
        cookAbility = &chef.skill->ability.fry;
        break;
    case STEAM:
        cookAbility = &chef.skill->ability.steam;
        break;
    case BAKE:
        cookAbility = &chef.skill->ability.bake;
        break;
    case KNIFE:
        cookAbility = &chef.skill->ability.knife;
        break;
    case NO_TOOL:
        return true;
    default:
        // std::cout << "Not using a tool" << std::endl;
        return true;
    }
    *cookAbility -= deduction;
    int bestPrice = sumPrice(rl, s);
    States newState = s;
    newState.setChef(chefId, chef);
    int newPrice = sumPrice(rl, newState);
    return newPrice == bestPrice;
}
/**
 * @brief
 * @param exactChefTool whether to use the exact tool deduction.Warning: it
 * should only be set true at the end of the function as it modifies the name of
 * the chefs.
 */
int sumPrice(const RuleInfo &rl, States s, int log, bool exactChefTool) {
    if (exactChefTool) {

        // std::cout << "exactChefTool" << std::endl;
        for (int i = 0; i < NUM_CHEFS; i++) {
            ToolEnum tool = s.getTool(i);
            std::string toolName = getToolName(tool);
            toolName = "-" + toolName;
            if (deductTool(rl, s, i, 40)) {
                if (deductTool(rl, s, i, 70)) {
                    if (deductTool(rl, s, i, 100)) {
                        s.appendName(i, toolName + "(0)");
                    } else {
                        s.appendName(i, toolName + "(30)");
                    }
                } else {
                    s.appendName(i, toolName + "(60)");
                }
            } else {
                s.appendName(i, toolName + "(100)");
            }
        }
    }
    if (MODE == 1) {

        BanquetRuleTogether rule[NUM_CHEFS * DISH_PER_CHEF];
        banquetRuleGenerated(rule, s, rl);
#ifdef MEASURE_TIME
        struct timespec start, finish;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
#endif
        BanquetInfo biCache;
        int totalScore = 0;
        int totalFull = 0;
        int scoreCache = 0;
        int fullCache = 0;
        int ans = 0;
        int dishStart = 0;
        int chefStart = 0;
        for (int g = 0; g < NUM_GUESTS; g++) {
            dishStart = DISH_PER_CHEF * CHEFS_PER_GUEST * g;
            chefStart = CHEFS_PER_GUEST * g;
            totalScore = 0;
            totalFull = 0;
            scoreCache = 0;
            fullCache = 0;
            auto skills = s.getSkills();
            for (int i = 0; i < DISH_PER_CHEF * CHEFS_PER_GUEST; i++) {
                if ((log & 0x10) && i % 3 == 0) {
                    std::cout << "详细信息************" << std::endl;
                    s[chefStart + i / 3]->print();
                    std::cout << "************" << std::endl;
                }
                biCache =
                    getPrice(skills[chefStart + i / 3], s.recipe[dishStart + i],
                             rule[dishStart + i], (log & 0x10));
                totalFull += biCache.full;
                totalScore += biCache.price;
                scoreCache += biCache.price;
                fullCache += biCache.full;
                if ((log & 0x1) && i % 3 == 2) {
                    std::cout << "  厨师：" << *s[chefStart + i / 3]->name
                              << " -> " << fullCache << " / " << scoreCache
                              << std::endl;
                    scoreCache = 0;
                    fullCache = 0;
                    std::cout << "  菜谱：" << s.recipe[dishStart + i - 2]->name
                              << "；" << s.recipe[dishStart + i - 1]->name
                              << "；" << s.recipe[dishStart + i]->name
                              << std::endl;
                }
            }
            int guestScore;
            switch (totalFull - rl.bestFull[g]) {
            case 0:
                guestScore = (int)std::ceil(totalScore * 1.3);
                break;
            default:
                int delta = std::abs(totalFull - rl.bestFull[g]);
                guestScore = (int)std::ceil(totalScore * (1 - 0.05 * delta));
            }
            ans += guestScore;
            if (log & 0x1)
                std::cout << "第" << g + 1 << "位客人：" << totalFull << " / "
                          << rl.bestFull[g] << " -> " << guestScore << ""
                          << std::endl;
        }
#ifdef MEASURE_TIME
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
        banquetRuleTime += finish.tv_sec - start.tv_sec +
                           (finish.tv_nsec - start.tv_nsec) * 1e-9;
#endif
        return ans;
    } else if (MODE == 2 || MODE == 0) {
        ActivityBuff activityBuff;
        auto p = &activityBuff;
        activityRule(p);
        if (MODE == 0)
            p = NULL;
        int energy = 0;
        int r = 0;
        for (int i = 0; i < NUM_CHEFS; i++) {
            if ((log & 0x10)) {
                std::cout << "VERBOSE************" << std::endl;
                s[i]->print();
                std::cout << "************" << std::endl;
            }
            int scoreCache = 0;
            if (log & 0x1)
                std::cout << "厨师：" << s[i]->name << std::endl << "菜谱：";
            auto skills = s.getSkills();

            for (int j = 0; j < DISH_PER_CHEF; j++) {
                if (log & 0x1)
                    std::cout << s.recipe[r]->name << "；";
                scoreCache +=
                    getPrice(skills[i], *s.recipe[r++], p, (log & 0x10));
            }
            energy += scoreCache;
            if (log & 0x1)
                std::cout << " -> " << scoreCache << std::endl;
        }
        return energy;
    } else {
        std::cout
            << "config.hpp中MODE设置错误。0为正常营业，1为宴会，2为限时任务"
            << std::endl;
        exit(1);
    }
}
void swap(Recipe *&a, Recipe *&b) {
    Recipe *temp = a;
    a = b;
    b = temp;
}

double f::exponential_multiplicative(int stepMax, int step, double tMax,
                                     double tMin) {
    return tMax * std::exp(-step / 1000.0);
}
double f::linear(int stepMax, int step, double tMax, double tMin) {
    return (tMax - tMin) * (1 - step / (double)stepMax) + tMin;
}
double f::t_dist_fast(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step * step / 300000.0);
}
double f::t_dist_slow(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step * step / 3000000.0);
}
double f::linear_mul(int stepMax, int step, double tMax, double tMin) {
    return tMin + (tMax - tMin) / (1 + step / 100000.0);
}
double f::zipf(int stepMax, int step, double tMax, double tMin) {
    double t = tMin + (tMax - tMin) / std::pow(step + 1, 0.2);
    // std::cout << t << std::endl;
    return t;
}
double f::one_over_n(int stepMax, int step, double tMax, double tMin) {
    return tMax / std::pow(step + 1, 0.1);
}

States perfectTool(const RuleInfo &rl, States s) {
    for (int i = 0; i < NUM_CHEFS; i++) {
        if (s.getTool(i) == NO_TOOL)
            continue;
        s.modifyTool(i, NOT_EQUIPPED);
        int max = sumPrice(rl, s);
        ToolEnum bestTool = NOT_EQUIPPED;
        for (int j = ABILITY_ENUM_START; j < ABILITY_ENUM_END; j++) {
            s.modifyTool(i, ToolEnum(j));
            int temp = sumPrice(rl, s);
            if (temp > max) {
                max = temp;
                bestTool = ToolEnum(j);
            }
        }
        s.modifyTool(i, bestTool);
    }
    return s;
}
States perfectChef(const RuleInfo &rl, States s, CList *c) {
    // perform a one-shot deviation from current state
    States newS = s;
    States bestS = s;
    int bestSscore = sumPrice(rl, bestS);
    for (int i = 0; i < NUM_CHEFS; i++) {
        for (auto chef : *c) {
            if (newS.repeatedChef(&chef, i)) {
                continue;
            }
            newS.setChef(i, chef);
            States pS = perfectTool(rl, newS);
            int pSs = sumPrice(rl, pS);

            if (pSs > bestSscore) {
                bestS = pS;
                bestSscore = pSs;
            }
        }
    }
    return bestS;
}
