// 归途列车限时任务
#ifndef ACTIVITYRULE_HPP
#define ACTIVITYRULE_HPP
#include "Calculator.hpp"
#include "functions.hpp"

/**
 * @brief
 *
 * @param rule
 * @param s
 * @return int : 过线分数
 */
void activityRule(ActivityBuff *buff) {

    buff->materialBuff[13] = 100;
    buff->materialBuff[30] = 100;
    buff->rarityBuff[4] = -100;
    buff->rarityBuff[5] = -100;
}
const int activityThreshold = 15000;
#endif
