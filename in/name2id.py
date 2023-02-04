# 使用说明：
# 在相同文件夹下增加in.txt文件，内容如：（括号可加可不加）
#
# Chef: 姒露-切(100)
# Recipe 0: 佛跳墙1: 火腿蜜瓜拼盘2: 鸳鸯火锅
# Chef: 大天狗-切(100)
# Recipe 0: BBQ烧烤1: 莓果思慕雪2: 玫瑰鸡翅
# Chef: 珊画-炸(30)
# Recipe 0: 灵芝空间1: 森林烩藜麦2: 酥炸蚝配鱼子酱
#
# 然后运行即可

import json
import re


with open("../data/data.min.json") as f:
    data = json.load(f)
recipes = data["recipes"]
chefs = data["chefs"]
# { STIRFRY, BAKE, BOIL, STEAM, FRY, KNIFE }


def tool2id(tool):
    if tool == "炒":
        return 0
    elif tool == "烤":
        return 1
    elif tool == "煮":
        return 2
    elif tool == "蒸":
        return 3
    elif tool == "炸":
        return 4
    elif tool == "切":
        return 5
    else:
        raise Exception("Unknown tool: " + tool)


def find_chef_name(name):
    for i in range(len(chefs)):
        if chefs[i]["name"] == name:
            return chefs[i]["chefId"]
    raise Exception("Unknown name: " + name)


def find_recipe_name(name):
    for i in range(len(recipes)):
        if recipes[i]["name"] == name:
            return recipes[i]["recipeId"]
    raise Exception("Unknown name: " + name)


def getChef(ls):
    m1 = "厨师：(.+)\-(.)\((\d+)\)"
    m2 = "厨师：(.+)\("
    for l in ls:
        chef = re.search(m1, l)
        if chef:
            print(find_chef_name(chef.group(1)),
                  tool2id(chef.group(2)), end=" ")
            return
        chef = re.search(m2, l)
        if chef:
            print(find_chef_name(chef.group(1)),
                  -2, end=" ")


def getRecipe(ls):
    m = "菜谱：(.+)；(.+)；(.+)"
    for l in ls:
        recipe = re.match(m, l)
        if recipe:
            print(find_recipe_name(recipe.group(1)), find_recipe_name(
                recipe.group(2)), find_recipe_name(recipe.group(3)), end=" ")


with open("in.txt") as f:
    ls = f.readlines()
    getChef(ls)
    print()
    getRecipe(ls)
    print()
