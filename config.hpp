#ifndef CONFIG_HPP
#define CONFIG_HPP

#define NUM_GUESTS 2
#define DISH_PER_CHEF 3
#define CHEFS_PER_GUEST 3
#define NUM_CHEFS (NUM_GUESTS * CHEFS_PER_GUEST)

#define MODE 1 // 1:宴会模式; 2:限时任务
#define TARGET_SCORE_APPROXIMATE 2980000
// 这里输分数线，达标之后将停止搜索

#define ITER_CHEF 5000   // 选择厨师时的迭代次数
#define ITER_RECIPE 1000 // 对于每一组厨师，选择菜谱时的迭代次数

#define AVOID_CHEF_1 true
#define AVOID_CHEF_2 true
#define AVOID_CHEF_3 true // 计算时不考虑三火厨师

// ************************************************** //
// ** 以下内容不建议修改，除非你清楚地知道这是什么意思 ** //
// ************************************************** //

#define T_MAX_CHEF TARGET_SCORE_APPROXIMATE / 100 // 选择厨师时的初始温度
#define T_MAX_RECIPE                                                           \
    TARGET_SCORE_APPROXIMATE / 400 // 对于每一组厨师，选择菜谱时的初始温度

#define SEARCH_TARGET_SCORE                                                    \
    TARGET_SCORE_APPROXIMATE // 限时任务时，达到这个分数就可以停止搜索
#endif
