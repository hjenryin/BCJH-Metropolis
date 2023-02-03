git pull
python3 data/checkUpdate.py
python3 assets/generateRule.py > rule_tmp.hpp
if [ $? -eq 0 ]; then
    mv rule_tmp.hpp rule.hpp
else
    rm rule_tmp.hpp
    echo "自动生成规则失败"
fi

