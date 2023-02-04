#ifndef TOOLEQUIPPED_HPP
#define TOOLEQUIPPED_HPP
#include "src/Chef.hpp"
#include "src/Values.hpp"

void toolEquipped(Chef *chef) {

    return; // 以下为示例代码，可自行修改。完成后注释此行。

    ///////////// 不希望任何厨师装备厨具：

    chef->tool = NO_TOOL;

    ///////////// 自定义厨师装备的厨具：

    auto skill = chef->skill; // 这句不用管，放着就好。

    if (chef->name == "达浪") {             // 金锄头
        skill.ability.add(30);              // 全技法+30
        skill.materialBuff.vegetable += 15; // 蔬菜售价+15%
        chef->tool = NO_TOOL;               // 不允许装备其他厨具
    }
    if (chef->name == "宋书汐汐") {   // 幸福礼花炮
        skill.ability.stirfry += 150; // 炒技法+150
        skill.abilityBuff.bake += 25; // 烤售价+25%
        chef->tool = NO_TOOL;         // 不允许装备其他厨具
    }
    if (chef->name == "柔可") {
        chef->skill.abilityBuff.knife += 20;
        chef->skill.materialBuff.creation += 15;
        chef->tool = NO_TOOL;
    }
}
#endif
