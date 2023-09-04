# 基于模拟退火的爆炒江湖宴会计算器

![](https://img.shields.io/github/downloads/hjenryin/BCJH-Metropolis/total) ![](https://img.shields.io/github/forks/hjenryin/BCJH-Metropolis) ![](https://img.shields.io/github/stars/hjenryin/BCJH-Metropolis)

## 在线运行

访问[run.bcjh.xyz](run.bcjh.xyz)。参见[此仓库](https://github.com/yuwenxifan/BCJH-Metropolis-html)。

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

### 更改迭代次数

如有需要，可以更改 `点此运行.bat` 中47行的数字来设置迭代次数。

## 本地编译

如果在windows平台，需要使用MSBuild编译，并将 `data/data.min.json` 改为ANSI编码。如有条件，强烈建议这种本地运行（运行可加速、可多次重复）。
_**另外，诚挚希望有能力的朋友们帮忙实现在客户端运行！**_

- 在 `./config.hpp` 中设置目标分数 `TARGET_SCORE_APPROXIMATE`，可以偏大但不要偏小。
- 在 `./data` 下运行 `python update.py -i xxxx`，`xxxx`是白菜菊花ID。
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
- 如果需要，在 `data/toolEquipped.csv` 配置已经戴上不愿碎掉的厨具。（配置方法参见[此处](https://github.com/hjenryin/BCJH-Metropolis#配置厨具)）
- 最后在命令行就可以得到输出结果了！（注意顺序）（得到厨师-技法，表示对应新手池厨具）

## Github云端编译（近期没有维护，可能又意想不到的结果）

- 注册Github账号，fork此仓库。
- 配置 `config.hpp` （参见[此文](https://github.com/hjenryin/BCJH-Metropolis/blob/main/assets/config.md)）。
- 将白菜菊花下载的个人数据(`userData.txt`)重命名为 `userData.json`并上传到 `data/`文件夹内。如果有规则更新，此时将会自动更新。
- 上传完之后，前往Actions页面（如图所示）。这一步如果Actions里面没有东西，试着删掉文件重新上传一下。有些人确实出现了这个问题，但我也没有复现出来，都是正常的……说不定删掉重新弄一下就好了orz

  ![to_action](assets/to_action.png)
  ![pick_workflow](assets/pick_workflow.png)
  ![result](assets/result.png)
- 等待完成后即可查看得到的几个比较好的结果。（注：无法保证得到的是最优解。）（注意顺序）（得到厨师-技法（数值），如果数值是0就表示不用装厨具，否则就装新手池对应厨具。）

## 局限性

- 已知的问题：

  - **无法得到最优解！只能得到一个比较好的解，有助于开阔思路。**
  - 无法设置专精。
  - 无法选择菜品数量（默认拉满）。
- 可能有一定门槛。（这可能有助于防止这类辅助工具的滥用导致分数膨胀？）（你问我为什么不用其他语言写？python一个晚上就写好了，结果因为有涉及json读写很多类型没法推断，jit用不了，算这个太慢了，所以就用c++写了）

## 工作原理

采用两层模拟退火来最大化总能量。第一层为三个厨师，其能量用第二层模拟退火来估计。也就是说，这套方法理论上也能算厨神（只要能够在非常快的时间内，算出一个厨神面板的得分），但是加上厨神的食材限制工作量有点大……以后再说吧。（希望以后的宴会不要有菜品限制）

### 如果你也想为此仓库添砖加瓦……

- c++实现厨神算分
- 以及任何你觉得有意义的事！

### 最后，欢迎大家提pr！有什么问题也可以在issue里面讨论！

这算是我写的第一个项目，本人本科在读，水平有限，写的不好的地方也请大佬们斧正！
