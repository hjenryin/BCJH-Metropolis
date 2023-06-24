# 基于模拟退火的爆炒江湖宴会计算器

![](https://img.shields.io/github/downloads/hjenryin/BCJH-Metropolis/total) ![](https://img.shields.io/github/forks/hjenryin/BCJH-Metropolis) ![](https://img.shields.io/github/stars/hjenryin/BCJH-Metropolis)

## 更新公告

* 速度、效果大提升。在我个人的数据中，风云宴平均耗时10秒（搜到分数线就停止）。
* 默认多线程。相同时间，运行一次，搜索8次（取决于电脑配置）。
* 参数选择：`ITER_CHEF`可以设大一点，反正搜到就停止。`ITER_RECIPE`可以小一点（一两千），会自动学习比较好的菜谱。`TARGET_SCORE_APPROXIMATE` 需要准确输入希望达到的分数线（毕竟搜到就停）。

## 使用方法（本地编译）

如果在windows平台，需要使用MSBuild编译，并将 `data/data.min.json` 改为ANSI编码。如有条件，强烈建议这种本地运行（运行可加速、可多次重复）。
_**另外，诚挚希望有能力的朋友们帮忙实现在客户端运行！**_

- 在 `./config.hpp` 中设置参数，方法参见[此文](https://github.com/hjenryin/BCJH-Metropolis/blob/main/assets/config.md)。
- 在 `./data` 下保存 `data.min.json` （[由此下载](https://foodgame.github.io/data/data.min.json)）和白菜菊花个人数据 `userData.json`。
- 编译、运行：
  - 在根目录新建 `build` 文件夹并 `cd` 至build
  - ```
    # linux
    cmake .. --config Release 
    make

    # windows (Visual Studio取决于安装的版本)
    cmake .. -G "Visual Studio 17 2022" --config Release
    msbuild /m
    ```
  - 使用 `./bcjh` (linux) 或 `./bcjh.exe` (windows) 运行。

- 如果需要，在 `build/toolEquipped.csv` 配置已经戴上不愿碎掉的厨具。（配置方法参见[此处](https://github.com/hjenryin/BCJH-Metropolis#配置厨具)）（理论上应该放在 `data/toolEquipped.csv`，但不知为何识别不了。）

-   （可选）可视化：运行结束之后就可以在 `./out/history.png` 和 `./out/recipe.png` 看到自己厨师和最后一道菜的历史路径。如果结果不理想，可以调整 `./config.hpp` 中的参数，以及 `./src/functions.hpp .cpp` 中 `namespace f` 里的退火函数。

  ![样例](./out/history.png)
- 最后在命令行就可以得到输出结果了！（注意顺序）（得到厨师-技法，表示对应新手池厨具，后面的括号如果是0表示不用装厨具）

## 使用方法（云端）

- 注册Github账号，fork此仓库。
- 配置 `config.hpp` （参见[此文](https://github.com/hjenryin/BCJH-Metropolis/blob/main/assets/config.md)）。
- 将白菜菊花下载的个人数据(`userData.txt`)重命名为 `userData.json`并上传到 `data/`文件夹内。如果有规则更新，此时将会自动更新。
- 上传完之后，前往Actions页面（如图所示）。这一步如果Actions里面没有东西，试着删掉文件重新上传一下。有些人确实出现了这个问题，但我也没有复现出来，都是正常的……说不定删掉重新弄一下就好了orz

  ![to_action](assets/to_action.png)
  ![pick_workflow](assets/pick_workflow.png)
  ![result](assets/result.png)
- 等待完成后即可查看得到的几个比较好的结果。（注：无法保证得到的是最优解。）（注意顺序）（得到厨师-技法（数值），如果数值是0就表示不用装厨具，否则就装新手池对应厨具。）

## 使用方法（本地安装包）

前往[Releases · hjenryin/BCJH-Metropolis](https://github.com/hjenryin/BCJH-Metropolis/releases)下载最新 `bcjh.zip`，解压后输入白菜菊花id即可使用。

### 配置厨具

使用 `Microsoft Excel` 打开 `厨具样例.csv`。可以看到，第一行是标题，下面是样例数据。

- 如果你所有厨师都可以装厨具，请保留第一行，删掉其他内容，重命名为 `toolEquipped.csv` 保存在原地。
- 如果你有多个账号，可以像样例一样用空行分隔，重命名为 `toolEquipped.csv` 保存在原地。
- 如果你不希望任何初始带厨具，你可以什么都不做（只要 `toolEquipped.csv` 不存在就可以）。

请在对应的格子内输入数字，规律如下：

- `技法-10%` ：填 `*0.9`；`技法+100` ：填 `100`；`技法-100` ：填 `-100`；
- `售价+10%` ，则填 `10`；`售价-5%` ，则填 `-5`。

## 局限性

- 如果使用正常营业模式，可以预知会出现以下问题：

  - “场上所有厨师技法加成”仅对本人生效。
- 如果使用限时任务模式，计算器的速度会下降，目前尚不清楚是什么原因造成的。
- 即使是在宴会模式下，也可以预知会出现一下问题：

  - **无法得到最优解！只能得到一个比较好的解，有助于开阔思路。**
  - 无法设置专精。
  - 无法选择菜品数量（默认拉满）。
  - banquetRule.hpp的生成时，可能遇到不认识的规则。
  - 合成套餐和原套餐的矛盾无法避免。
  - 一些规则奇奇怪怪的厨师（应该就汤圆年糕）的技能没有实现。
- 可能有一定门槛。（这可能有助于防止这类辅助工具的滥用导致分数膨胀？）（你问我为什么不用其他语言写？python一个晚上就写好了，结果因为有涉及json读写很多类型没法推断，jit用不了，算这个太慢了，所以就用c++写了）

## 工作原理

采用两层模拟退火来最大化总能量。第一层为三个厨师，其能量用第二层模拟退火来估计。也就是说，这套方法理论上也能算厨神（只要能够在非常快的时间内，算出一个厨神面板的得分），但是加上厨神的食材限制工作量有点大……以后再说吧。（希望以后的宴会不要有菜品限制）

### 如果你也想为此仓库添砖加瓦……

- c++实现厨神算分
- 以及任何你觉得有意义的事！

### 最后，欢迎大家提pr！有什么问题也可以在issue里面讨论！

这算是我写的第一个项目，本人本科在读，水平有限，写的不好的地方也请大佬们斧正！
