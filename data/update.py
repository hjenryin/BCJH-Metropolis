import argparse
import os
import requests
import platform

args=argparse.ArgumentParser()
args.add_argument('-i', '--id',type=int)
args=args.parse_args()
def UTF8ToGBK(file):
    with open(file, 'r', encoding='utf-8') as f:
        content = f.read()
    with open(file, 'w', encoding='gbk') as f:
        f.write(content)
def download(url,fn):
    f = requests.get(url) 
    with open(fn, "wb") as file:
        file.write(f.content)
    if platform.system()=="Windows":
        UTF8ToGBK(fn)
if args.id:
    print(args.id)
    download(f"https://bcjh.xyz/api/download_data?id={args.id}",'userData.json')

download("https://bcjh.xyz/api/get_banquet_rule",'ruleData.json')

    
    
    