@REM This file should be encoded in GB2312.
echo 修改配置、了解更多请至https://github.com/hjenryin/BCJH-Metropolis/ 以便在线运行
echo 有疑问请至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/q-a 提问
echo 有任何想法可至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/general 讨论
echo 现已更新汤圆、乌龙等奇奇怪怪的技能，如果发现有问题，欢迎到 https://github.com/hjenryin/BCJH-Metropolis/issues/new 提出~

echo 正在获取宴会规则
powershell -command curl -o "ruleData.json" "https://bcjh.xyz/api/get_banquet_rule"
iconv -f utf-8 -t gbk ruleData.json > tmp 2>NUL
if %ERRORLEVEL% EQU 0 (
    del ruleData.json
    ren tmp ruleData.json
) 
echo 宴会规则获取完毕
@REM powershell -command curl -o "data.min.json" "https://raw.githubusercontent.com/foodgame/foodgame.github.io/master/data/data.min.json"
set "bcjhid="
set /p bcjhid=如果需要重新下载用户配置，请输入白菜菊花id；否则直接回车：

if "%bcjhid%"=="" (
    echo 未输入id，跳过下载用户配置
) 

if not "%bcjhid%"=="" (
    echo 正在下载用户配置
    powershell -command curl -o "userData.json" "https://bcjh.xyz/api/download_data?id=%bcjhid%"
    powershell -command $json=$(iwr -uri "https://bcjh.xyz/api/download_data?id=%bcjhid%" ^| ConvertFrom-Json^) ^; if ($json.result -eq 'True'^) { Write-Host 下载成功 -ForegroundColor:Green } else { Write-Host 下载失败，请检查id是否正确 -BackgroundColor:Red -ForegroundColor:White ^}
    iconv -f utf-8 -t gbk userData.json > tmp 2>NUL
    if %ERRORLEVEL% EQU 0 (
        del userData.json
        ren tmp userData.json
    )
)
echo=
echo=查看厨具修改教程，请参考 https://github.com/hjenryin/BCJH-Metropolis#配置厨具
.\bcjh.exe
echo 按任意键关闭
pause >NUL