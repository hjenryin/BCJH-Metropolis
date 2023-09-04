#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifndef EMSCRIPTEN
#define NUM_GUESTS 2
#endif
#define DISH_PER_CHEF 3
#define CHEFS_PER_GUEST 3
#define NUM_CHEFS (NUM_GUESTS * CHEFS_PER_GUEST)
#define NUM_DISHES (NUM_CHEFS * DISH_PER_CHEF)
// #define DEBUG_INTEGRITY
#define TARGET_SCORE_APPROXIMATE 3170000
// 这里输分数线，达标之后将停止搜索

// #define VIS_HISTORY
// #define DEBUG_INTEGRITY
#ifdef EMSCRIPTEN
#define EMSCRIPTEN_PROGRESS
#endif

// ************************************************** //
// ** 以下内容不建议修改，除非你清楚地知道这是什么意思 ** //
// ************************************************** //

#define NO_FORMAT "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define UNDERLINE "\033[4m"

#define T_MAX_CHEF_orig TARGET_SCORE_APPROXIMATE / 100 // 选择厨师时的初始温度
#define T_MAX_RECIPE_orig                                                      \
    TARGET_SCORE_APPROXIMATE / 400 // 对于每一组厨师，选择菜谱时的初始温度

#endif
