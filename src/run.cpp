#include "run.hpp"
Result run(const RuleInfo &rl, const CList &chefList, RList &recipeList,
           int log, bool silent, int seed
#ifdef EMSCRIPTEN_PROGRESS
           ,
           emscripten::val postProgress
#endif
           ,
           int threadId) {
    auto chefListPtr = new CList(chefList);

    for (auto &chef : *chefListPtr) {
        chef.recipeLearned = new std::vector<Recipe *>();
    }

    srand(seed);
    SARunner saRunner(&rl, chefListPtr, &recipeList, true, f::t_dist_slow,
                      threadId);

    auto s = saRunner.run(NULL,
#ifdef EMSCRIPTEN_PROGRESS
                          postProgress,
#endif
                          silent);
    // *s = perfectChef(rl, *s, chefListPtr);
    int score = sumPrice(rl, s, log, false);
    debugIntegrity(s);
    for (auto &chef : *chefListPtr) {
        delete chef.recipeLearned;
    }
    delete chefListPtr;
    return Result{score, seed, s, ""};
}

std::tuple<RList, CList> loadJson(const Json::Value &gameData,
                                  const Json::Value &userData
#ifndef _WIN32
                                  ,
                                  bool allowTool
#endif
) {
    RList recipeList;
    CList chefList;
    std::cout << "正在读取厨师与菜谱..." << std::endl;

    try {
        Recipe::initRarityBuff(userData["userUltimate"]);
        Skill::loadJson(gameData["skills"]);
        Chef::initBuff(userData["userUltimate"]);
        Chef::loadAppendChef(chefList, 5, gameData, userData
#ifndef _WIN32
                             ,
                             allowTool
#endif
        );
        int chefRarity = 4;
        do {
            Chef::loadAppendChef(chefList, chefRarity, gameData, userData
#ifndef _WIN32
                                 ,
                                 allowTool
#endif
            );
            chefRarity--;
        } while (chefList.size() <= NUM_CHEFS && chefRarity >= 0);
        recipeList = loadRecipe(gameData, userData);
        if (chefList.size() <= NUM_CHEFS) {

            std::cout << NoChefException(chefList.size()).what() << std::endl;
            exit(1);
        }
    } catch (const Json::RuntimeError &) {
        std::cout << "json文件格式不正确。如果文件内容是手动复制的，确认文件已"
                     "经复制完整。\n";
    } catch (const Json::LogicError &) {
        std::cout << "json文件格式不正确。请确认文件来自白菜菊花而非图鉴网。\n";
    };
    std::cout << "读取文件成功。" << std::endl;
    return {std::move(recipeList), std::move(chefList)};
}
