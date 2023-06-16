import argparse
import os

args=argparse.ArgumentParser()
args.add_argument("-p","--path",type=str,default=".",help="Path to the folder containing files to be changed")
args=args.parse_args()
file=args.path

with open(file,"r",encoding="utf-8") as f1:
    lines=f1.readlines()
with open(file,"w",encoding="gbk") as f2:
    for line in lines:
        f2.write(line)
        
        