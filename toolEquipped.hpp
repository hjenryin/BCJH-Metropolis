#ifndef TOOLEQUIPPED_HPP
#define TOOLEQUIPPED_HPP
#include "src/Chef.hpp"

void toolEquipped(Chef *chef) {
//    return; // 以下为示例代码，可自行修改。完成后注释此行。
    if (chef->name == "姒露") {         // 银锅铲
        chef->skill.ability.stirfry += 80; // 炒技法+80
        chef->skill.materialBuff.vegetable += 10; // 蔬菜售价+10%
        chef->tool = NO_TOOL;               // 不允许装备其他厨具
    }

}

#endif
