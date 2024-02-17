# coding=utf8
import os
import json

print("正在检查更新...")
ret = os.system(
    "wget --timeout=10 --tries=1 -O data/data.new.json https://yuwenxifan.github.io/bcjhMobile/data/data.min.json > /dev/null 2>&1"
)
if ret == 0:
    print("从图鉴网获取最新数据成功。")
    new=json.load(open("data/data.new.json"))
    old=json.load(open("data/data.min.json"))
    same=(len(new["recipes"])==len(old["recipes"])) and (len(new["chefs"])==len(old["chefs"]))
    if same:
        print("本地数据已是最新。")
        os.system("rm data/data.new.json")
    else:
        print("本地数据不是最新，将更新。")
        os.system("mv data/data.new.json data/data.min.json")
else:
    print("从图鉴网获取最新数据失败，将使用本地数据。")
