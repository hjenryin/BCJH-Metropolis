#ifndef CONFIG_HPP
#define CONFIG_HPP

#define NUM_CHEFS 3
#define DISH_PER_CHEF 3
#define T_MAX_CHEF 20000 // 选择厨师时的初始温度
#define T_MAX_RECIPE 20000 // 对于每一组厨师，选择菜谱时的初始温度
#define ITER_CHEF 10000     // 选择厨师时的迭代次数
#define ITER_RECIPE 10000 // 对于每一组厨师，选择菜谱时的迭代次数
#define BANQUET true     // 宴会模式
#define AVOID_CHEF_1 true
#define AVOID_CHEF_2 true
#define AVOID_CHEF_3 true // 计算时不考虑三火厨师
#endif
