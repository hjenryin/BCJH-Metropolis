import os

print("正在检查更新...")
ret = os.system(
    "wget --timeout=5 --tries=1 -O data/data.new.json https://raw.githubusercontent.com/foodgame/foodgame.github.io/master/data/data.min.json > /dev/null 2>&1")
if ret == 0:
    print("从图鉴网获取最新数据成功。")
    diff = os.system("diff -q data/data.min.json data/data.new.json")
    if diff == 0:
        print("本地数据已是最新。")
        os.system("rm data/data.new.json")
    else:
        print("本地数据不是最新，将更新。")
        os.system("mv data/data.new.json data/data.min.json")
else:
    print("从图鉴网获取最新数据失败，将使用本地数据。")
