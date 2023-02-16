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
    int materialAdd[] = {3, 4, 5, 7, 8, 9, 38, 39};
    int materialSub[] = {2, 24, 37, 41, 42};
    for (int i : materialAdd) {
        buff->materialBuff[i] = 200;
    }
    for (int i : materialSub) {
        buff->materialBuff[i] = -200;
    }
    buff->rarityBuff[4] = 100;
    buff->rarityBuff[5] = -200;
    buff->rarityBuff[3] = 300;
    buff->flavorBuff[SALTY] = 200;
    buff->flavorBuff[SOUR] = 100;
}
const int activityThreshold = 24000;
#endif
