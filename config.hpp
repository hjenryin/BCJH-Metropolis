#ifndef CONFIG_HPP
#define CONFIG_HPP

#define NUM_GUESTS 3
#define DISH_PER_CHEF 3
#define CHEFS_PER_GUEST 3
#define NUM_CHEFS (NUM_GUESTS * CHEFS_PER_GUEST)

#define MODE 1 // 1:宴会模式; 2:限时任务
#define TARGET_SCORE_APPROXIMATE 4650000
// 预期达到的分数
// 如果是限时任务，这里输分数线
// 如果是宴会模式，不用特别准，差两倍/一半以内都可以

#define ITER_CHEF 10000   // 选择厨师时的迭代次数
#define ITER_RECIPE 10000 // 对于每一组厨师，选择菜谱时的迭代次数

#define AVOID_CHEF_1 true
#define AVOID_CHEF_2 true
#define AVOID_CHEF_3 true // 计算时不考虑三火厨师

// ************************************************** //
// ** 以下内容不建议修改，除非你清楚地知道这是什么意思 ** //
// ************************************************** //

#define T_MAX_CHEF TARGET_SCORE_APPROXIMATE / 100 // 选择厨师时的初始温度
#define T_MAX_RECIPE                                                           \
    TARGET_SCORE_APPROXIMATE / 50 // 对于每一组厨师，选择菜谱时的初始温度

// ************************************* //
// ** 以下内容不要修改，会造成程序出问题 ** //
// ************************************* //

#if MODE == 2
#define SEARCH_TARGET_SCORE                                                    \
    TARGET_SCORE_APPROXIMATE // 限时任务时，达到这个分数就可以停止搜索
#endif

#endif
