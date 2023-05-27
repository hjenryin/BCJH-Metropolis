git pull
python3 data/checkUpdate.py

# 暂时去掉自动更新规则
# python3 assets/generateRule.py > src/rule_tmp.hpp
# if [ $? -eq 0 ]; then
#     mv src/rule_tmp.hpp src/banquetRule.hpp
# else
#     rm src/rule_tmp.hpp
#     echo "自动生成规则失败"
# fi

