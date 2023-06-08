def changeEncoding(file, src,dst):
    with open(file, 'r', encoding=src) as f:
        content = f.read()
    with open(file, 'w', encoding=dst) as f:
        f.write(content)
    print("Encoding of file " + file + " has been changed from " + src + " to " + dst)

folder="./src"
other_files=["config.hpp","toolEquipped.hpp"]
import os
# Iterate through the file in the folder and its subfolders and change the encoding
for root, dirs, files in os.walk(folder):
    for file in files:
        print(file)
        if file.endswith("pp"):
            file = os.path.join(root, file)
            changeEncoding(file,  "GB2312","utf-8")

for file in other_files:
    changeEncoding(file,"GB2312","utf-8")