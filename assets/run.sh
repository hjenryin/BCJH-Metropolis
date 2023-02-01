if [ -f "data/userData.json" ]; then
    echo "编译中……"
    mkdir build && cd ./build && cmake ../ > /dev/null && make -j$(nproc) > /dev/null
    ./bcjh -s && ./bcjh -s 
else
    echo "userData.json不存在。"
fi