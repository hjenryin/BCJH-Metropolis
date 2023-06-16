import os
fl=os.listdir(".")
for f in fl:
    if f.endswith(".cpp") or f.endswith(".hpp"):
        with open(f,"r",encoding="utf-8") as f1:
            lines=f1.readlines()
        with open(f,"w",encoding="utf-8") as f2:
            for line in lines:
                if line.find("system(")!=-1:
                    continue
                f2.write(line)