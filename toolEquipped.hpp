#ifndef TOOLEQUIPPED_HPP
#define TOOLEQUIPPED_HPP
#include "src/Chef.hpp"
#include "src/Values.hpp"

// 关于技法：
// 例：skill.ability.add(30);     // 全技法+30
// 例：skill.ability.bake += 150; // 烤技法+150,
//   bake 烤；knife 切；stirfry 炒；fry 炸；steam 蒸；boil 煮

// 关于技法售价：
// 例：skill.abilityBuff.bake += 25; // 烤售价+25%

// 关于口味售价：
// 例：skill.flavorBuff.sweet += 30; // 甜售价+30%
//   sweeet 甜；spicy 辣；salty 咸；tasty 鲜；sour 酸；bitter 苦

// 关于食材售价：
// 例：skill.materialBuff.vegetable += 15; // 蔬菜售价+15%
//   vegetable 菜类；meat 肉类；fish 鱼类；creation 面类

// 关于金币获得：
// 例：skill.coinBuff += 30; // 金币获得+30%

void toolEquipped(Chef *chef) {
    // return; // 以下为示例代码，可自行修改。完成后注释此行。
    if (chef->name == "达浪") {             // 羊毛牛皮太鼓
        chef->skill.abilityBuff.fry += 35;  // 炸售价+35%
        chef->skill.materialBuff.meat += 5; // 肉菜售价+5%
        chef->skill.ability.fry *= 0.75;    // 炸技法-25%
        chef->tool = NO_TOOL;               // 不允许装备其他厨具
    }
    if (chef->name == "姒露") {              // 千里江山图
        chef->skill.ability.bake += 80;      // 烤技法+80
        chef->skill.flavorBuff.bitter += 30; // 苦售价+30%
        chef->tool = NO_TOOL;                // 不允许装备其他厨具
    }
    if (chef->name == "宋书汐汐") {         // 玉石墨笔桶
        chef->skill.ability.stirfry += 150; // 炒技法+150
        chef->skill.abilityBuff.fry += 20;  // 炸售价+20%
        chef->tool = NO_TOOL;               // 不允许装备其他厨具
    }
    if (chef->name == "小卧龙") {            // 圣光加护三头叉
        chef->skill.abilityBuff.knife += 35; // 切售价+35%
        chef->skill.materialBuff.meat += 5;  // 肉菜售价+5%
        chef->skill.ability.knife *= 0.75;   // 切技法-25%
        chef->tool = NO_TOOL;                // 不允许装备其他厨具
    }
    if (chef->name == "麦田姬") {            // 千年泡菜坛
        chef->skill.abilityBuff.steam += 40; // 蒸售价+40%
        chef->skill.materialBuff.fish -= 15; // 鱼菜售价-15%
        chef->skill.ability.steam *= 0.9;    // 蒸技法-10%
        chef->tool = NO_TOOL;                // 不允许装备其他厨具
    }
    if (chef->name == "黑柏") {             // 三弦曲项琵琶
        chef->skill.abilityBuff.boil += 25; // 煮售价+25%
        chef->skill.materialBuff.meat += 5; // 肉菜售价+5%
        chef->skill.ability.boil *= 0.85;   // 煮技法-15%
        chef->tool = NO_TOOL;               // 不允许装备其他厨具
    }
    if (chef->name == "麻雀") {          // 闪亮夜视镜
        chef->skill.ability.steam -= 50; // 蒸技法-50
        chef->skill.ability.boil -= 50;  // 煮技法-50
        chef->skill.ability.fry += 200;  // 炸技法+200
        chef->tool = NO_TOOL;            // 不允许装备其他厨具
    }
}

#endif
