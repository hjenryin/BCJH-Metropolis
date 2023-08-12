import argparse
import os

args=argparse.ArgumentParser()
args.add_argument('-i', '--id',type=int)
args=args.parse_args()
def UTF8ToGBK(file):
    with open(file, 'r', encoding='utf-8') as f:
        content = f.read()
    with open(file, 'w', encoding='gbk') as f:
        f.write(content)
if args.id:
    print(args.id)
    os.system(f'powershell -command curl -o "userData.json" "https://bcjh.xyz/api/download_data?id={args.id}"')
    UTF8ToGBK('userData.json')
os.system('powershell -command curl -o "ruleData.json" "https://bcjh.xyz/api/get_banquet_rule"')
UTF8ToGBK('ruleData.json')

    
    
    